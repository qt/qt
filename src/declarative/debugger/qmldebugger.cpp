/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmldebugger.h"
#include <QtGui/qtreewidget.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qplaintextedit.h>
#include <QTextBlock>
#include <QtGui/qtabwidget.h>
#include <QtDeclarative/qmlbindablevalue.h>
#include <QtDeclarative/qmldebuggerstatus.h>
#include <private/qmlboundsignal_p.h>
#include <private/qmlcontext_p.h>
#include <private/qmlengine_p.h>
#include <private/qmlexpression_p.h>
#include <private/qmlobjecttree_p.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qurl.h>
#include <QtGui/qsplitter.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qtablewidget.h>
#include <QtGui/qevent.h>
#include <QtDeclarative/qmlexpression.h>
#include <private/qmlpropertyview_p.h>
#include <private/qmlwatches_p.h>

QT_BEGIN_NAMESPACE

QmlDebugger::QmlDebugger(QWidget *parent)
: QWidget(parent), m_tree(0), m_warnings(0), m_watchTable(0), m_watches(0), 
  m_properties(0), m_text(0), m_highlightedItem(0)
{
    QHBoxLayout *layout = new QHBoxLayout;
    setLayout(layout);
    QSplitter *splitter = new QSplitter(this);
    layout->addWidget(splitter);

    QWidget *treeWid = new QWidget(this);
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(0, 0, 0, 0);
    treeWid->setLayout(vlayout);
    splitter->addWidget(treeWid);

    m_tree = new QmlObjectTree(treeWid);
    m_tree->setHeaderHidden(true);
    QObject::connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *)));
    QObject::connect(m_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *)));
    QObject::connect(m_tree, SIGNAL(addWatch(QObject*,QString)), this, SLOT(addWatch(QObject*,QString)));
    vlayout->addWidget(m_tree);

    QPushButton *pb = new QPushButton(tr("Refresh"), treeWid);
    QObject::connect(pb, SIGNAL(clicked()), this, SLOT(refresh()));
    vlayout->addWidget(pb);

    QTabWidget *tabs = new QTabWidget(this);

    m_text = new QPlainTextEdit(this);
    m_text->setReadOnly(true);
    tabs->addTab(m_text, tr("File"));

    m_warnings = new QTreeWidget(this);
    m_warnings->setHeaderHidden(true);
    tabs->addTab(m_warnings, tr("Warnings"));

    m_watches = new QmlWatches(this);
    m_watchTable = new QTableView(this);
    m_watchTable->setSelectionMode(QTableWidget::NoSelection);
    m_watchTable->setModel(m_watches);
    tabs->addTab(m_watchTable, tr("Watches"));

    m_properties = new QmlPropertyView(m_watches, this);
    QObject::connect(m_properties, SIGNAL(objectClicked(quint32)), 
                     this, SLOT(highlightObject(quint32)));
    tabs->addTab(m_properties, tr("Properties"));
    tabs->setCurrentWidget(m_properties);

    splitter->addWidget(tabs);
    splitter->setStretchFactor(1, 2);

    setGeometry(0, 100, 800, 600);
}

void QmlDebugger::itemDoubleClicked(QTreeWidgetItem *)
{
}

void QmlDebugger::addWatch(QObject *obj, const QString &expr)
{
    QmlContext *ctxt = qmlContext(obj);
    if(ctxt) {
        QmlExpressionObject *e= new QmlExpressionObject(ctxt, expr, obj, m_watches);
        m_watches->addWatch(e);
    }
}

void QmlDebugger::highlightObject(quint32 id)
{
    QHash<quint32, QTreeWidgetItem *>::ConstIterator iter = m_items.find(id);
    if (m_highlightedItem) {
        m_highlightedItem->setBackground(0, QPalette().base());
        m_highlightedItem = 0;
    }

    if (iter != m_items.end())  {
        m_highlightedItem = *iter;
        m_highlightedItem->setBackground(0, QColor("cyan"));
        m_tree->expandItem(m_highlightedItem);
        m_tree->scrollToItem(m_highlightedItem);
    } 
}

void QmlDebugger::itemClicked(QTreeWidgetItem *i)
{
    QmlDebuggerItem *item = static_cast<QmlDebuggerItem *>(i);

    if(m_selectedItem) {
        QmlDebuggerStatus *debug = 
            qobject_cast<QmlDebuggerStatus *>(m_selectedItem);
        Q_ASSERT(debug);
        debug->setSelectedState(false);
        m_selectedItem = 0;
    }

    if(item->object) {
        QmlDebuggerStatus *debug = 
            qobject_cast<QmlDebuggerStatus *>(item->object);
        if(debug) {
            debug->setSelectedState(true);
            m_selectedItem = item->object;
        }

        m_properties->setObject(item->object);
    } 

    if(item->url.scheme() == QLatin1String("file")) {
        QString f = item->url.toLocalFile();
        QFile file(f);
        file.open(QIODevice::ReadOnly);
        QByteArray ba = file.readAll();
        QTextStream stream(ba, QIODevice::ReadOnly);
        const QString code = stream.readAll();

        m_text->setPlainText(code);

        if(item->startLine != -1) {
            QTextDocument *document = m_text->document();
            QTextCharFormat format;
            format.setForeground(Qt::lightGray);

            {
                QTextCursor cursor(document);
                cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, item->endLine);
                cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
                cursor.setCharFormat(format);
            }

            {
                QTextCursor cursor(document);
                cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, item->startLine - 1);
                cursor.setCharFormat(format);
            }

            {
                QTextCursor cursor(document);
                cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, item->startLine - 1);
                m_text->setTextCursor(cursor);
                m_text->centerCursor();
            }


        }

    }
}

bool QmlDebugger::makeItem(QObject *obj, QmlDebuggerItem *item)
{
    bool rv = true;

    QString text;

    item->object = obj;

    if(QmlBindableValue *bv = qobject_cast<QmlBindableValue *>(obj)) {
        QmlExpressionPrivate *p = bv->d;

        text = bv->property().name() + QLatin1String(": ") + bv->expression();
        item->setForeground(0, Qt::green);
        item->bindableValue = bv;

        if(p->log) {
            QTreeWidgetItem *warningItem = 0;

            for(int ii = 0; ii < p->log->count(); ++ii) {
                const QmlExpressionLog &log = p->log->at(ii);

                QString variant; QDebug d(&variant); d << log.result();

                if(!log.warnings().isEmpty()) {

                    if(!warningItem) {
                        warningItem = new QTreeWidgetItem(m_warnings);
                        warningItem->setText(0, bv->expression());
                    }

                    QTreeWidgetItem *entry = new QTreeWidgetItem(warningItem);
                    entry->setExpanded(true);

                    entry->setText(0, variant);

                    foreach(const QString &warning, log.warnings()) {
                        QTreeWidgetItem *w = new QTreeWidgetItem(entry);
                        w->setText(0, warning);
                    }
                }

            }

        }

        delete item;
        return false;

    } else if(qobject_cast<QmlBoundSignal *>(obj)) {
        delete item;
        return false;
    } else {
        QmlContext *context = qmlContext(obj);
        QmlContext *parentContext = qmlContext(obj->parent());

        text = QLatin1String(obj->metaObject()->className());

        if(context && context != parentContext) {
            QmlContextPrivate *p = static_cast<QmlContextPrivate *>(QObjectPrivate::get(context));

            QString toolTipString;
            if(!p->url.toString().isEmpty()) {
                item->url = p->url;
                toolTipString = QLatin1String("URL: ") + p->url.toString();
            }

            if(!p->typeName.isEmpty()) {
                if(!toolTipString.isEmpty()) 
                    toolTipString.prepend(QLatin1Char('\n'));
                toolTipString.prepend(tr("Root type: ") + text);
                text = QString::fromAscii(p->typeName);
            }

            if(!toolTipString.isEmpty())
                item->setToolTip(0, toolTipString);

            item->setForeground(0, QColor("orange"));

            if(p->startLine != -1) {
                item->startLine = p->startLine;
                item->endLine = p->endLine;
            }

        } else {
            item->setExpanded(true);
        }

        if(!context)
            item->setForeground(0, Qt::lightGray);
    }

    m_items.insert(m_watches->objectId(obj), item);
    item->setText(0, text);

    return rv;
}

void QmlDebugger::buildTree(QObject *obj, QmlDebuggerItem *parent)
{
    QObjectList children = obj->children();

    for (int ii = 0; ii < children.count(); ++ii) {
        QmlDebuggerItem *item = new QmlDebuggerItem(parent);
        if(makeItem(children.at(ii), item))
            buildTree(children.at(ii), item);
    }
}

void QmlDebugger::refresh()
{
    setDebugObject(m_object);
}

bool operator<(const QPair<quint32, QPair<int, QString> > &lhs,
               const QPair<quint32, QPair<int, QString> > &rhs)
{
    return lhs.first < rhs.first;
}

void QmlDebugger::setCanvas(QSimpleCanvas *c)
{
    Q_UNUSED(c);
}

void QmlDebugger::setDebugObject(QObject *obj)
{
    m_tree->clear();
    m_warnings->clear();
    m_items.clear();
    m_highlightedItem = 0;

    m_object = obj;
    if(!obj)
        return;

    QmlDebuggerItem *item = new QmlDebuggerItem(m_tree);
    makeItem(obj, item);
    buildTree(obj, item);
    item->setExpanded(true);
}

QT_END_NAMESPACE

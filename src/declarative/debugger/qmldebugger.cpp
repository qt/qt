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
#include <QtDeclarative/qmlbindablevalue.h>
#include <private/qmlboundsignal_p.h>
#include <private/qmlcontext_p.h>
#include <private/qmlengine_p.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qurl.h>
#include <QtGui/qsplitter.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qevent.h>

QmlDebugger::QmlDebugger(QWidget *parent)
: QWidget(parent), m_tree(0)
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

    m_tree = new QTreeWidget(treeWid);
    m_tree->setHeaderHidden(true);
    QObject::connect(m_tree, SIGNAL(itemPressed(QTreeWidgetItem *, int)), this, SLOT(itemPressed(QTreeWidgetItem *)));
    vlayout->addWidget(m_tree);

    QPushButton *pb = new QPushButton("Refresh", treeWid);
    QObject::connect(pb, SIGNAL(clicked()), this, SLOT(refresh()));
    vlayout->addWidget(pb);

    m_text = new QPlainTextEdit(this);
    m_text->setReadOnly(true);
    splitter->addWidget(m_text);
    splitter->setStretchFactor(1, 2);
}

class QmlDebuggerItem : public QTreeWidgetItem
{
public:
    QmlDebuggerItem(QTreeWidget *wid)
        : QTreeWidgetItem(wid), startLine(-1), endLine(-1)
    {
    }

    QmlDebuggerItem(QTreeWidgetItem *item)
        : QTreeWidgetItem(item), startLine(-1), endLine(-1)
    {
    }

    int startLine;
    int endLine;
    QUrl url;

    QPointer<QmlBindableValue> bindableValue;
};

void QmlDebugger::itemPressed(QTreeWidgetItem *i)
{
    QmlDebuggerItem *item = static_cast<QmlDebuggerItem *>(i);

    if(item->bindableValue) {

        QString str;

        QmlExpressionPrivate *p = item->bindableValue->d;
        if(p->log) {
            QString str;
            QDebug d(&str);
            for(int ii = 0; ii < p->log->count(); ++ii) {
                d << p->log->at(ii).result() << "\n";
                QStringList warnings = p->log->at(ii).warnings();
                foreach(const QString &warning, warnings)
                    d << "    " << warning << "\n";
            }
            m_text->setPlainText(str);

        } else {
            m_text->setPlainText("No history");
        }

    } else if(item->url.scheme() == QLatin1String("file")) {
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
                cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, item->startLine - 1);
                cursor.setCharFormat(format);
            }

            {
                QTextCursor cursor(document);
                cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, item->endLine);
                cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
                cursor.setCharFormat(format);
            }

            {
                QTextCursor cursor(document);
                cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                cursor.setCharFormat(QTextCharFormat());
            }

            {
                QTextCursor cursor(document);
                cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, item->startLine - 1);
                m_text->setTextCursor(cursor);
                m_text->centerCursor();
            }
        }

    }
}

static bool makeItem(QObject *obj, QmlDebuggerItem *item)
{
    bool rv = true;

    QString text;

    if(QmlBindableValue *bv = qobject_cast<QmlBindableValue *>(obj)) {
        text = bv->property().name() + ": " + bv->expression();
        item->setForeground(0, Qt::green);
        item->bindableValue = bv;
    } else if(QmlBoundSignal *bs = qobject_cast<QmlBoundSignal *>(obj)) {
        QMetaMethod method = obj->parent()->metaObject()->method(bs->index());
        QByteArray sig = method.signature();
        if(!sig.isEmpty())
            text = sig + ": ";
        text += bs->expression();
        item->setForeground(0, Qt::blue);
        rv = false;
    } else {
        QmlContext *context = qmlContext(obj);
        QmlContext *parentContext = qmlContext(obj->parent());

        text = QLatin1String(obj->metaObject()->className());

        if(context && context != parentContext) {
            QmlContextPrivate *p = static_cast<QmlContextPrivate *>(QObjectPrivate::get(context));

            QString toolTipString;
            if(!p->url.toString().isEmpty()) {
                item->url = p->url;
                toolTipString = "URL: " + p->url.toString();
            }

            if(!p->typeName.isEmpty()) {
                if(!toolTipString.isEmpty()) 
                    toolTipString.prepend("\n");
                toolTipString.prepend("Root type: " + text);
                text = p->typeName;
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

    item->setText(0, text);

    return rv;
}

static void buildTree(QObject *obj, QmlDebuggerItem *parent)
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

void QmlDebugger::setDebugObject(QObject *obj)
{
    m_tree->clear();

    m_object = obj;
    if(!obj)
        return;

    QmlDebuggerItem *item = new QmlDebuggerItem(m_tree);
    makeItem(obj, item);
    buildTree(obj, item);
    item->setExpanded(true);
    setGeometry(0, 100, 800, 600);
}


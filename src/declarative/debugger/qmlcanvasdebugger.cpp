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

#include <private/qmlcanvasdebugger_p.h>
#include <private/qmlwatches_p.h>
#include <QtDeclarative/qsimplecanvas.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qsplitter.h>
#include <QtGui/qtreewidget.h>

#include <QtDeclarative/qfxrect.h>

QmlCanvasDebugger::QmlCanvasDebugger(QmlWatches *w, QWidget *parent)
: QWidget(parent), m_watches(w), m_tree(0), m_canvas(0), m_debugCanvas(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    QPushButton *pb = new QPushButton("Refresh", this);
    QObject::connect(pb, SIGNAL(clicked()), this, SLOT(refresh()));
    layout->addWidget(pb);

    QSplitter *splitter = new QSplitter(this);

    m_tree = new QTreeWidget(this);
    QObject::connect(m_tree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(itemExpanded(QTreeWidgetItem*)));
    QObject::connect(m_tree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(itemCollapsed(QTreeWidgetItem*)));
    QObject::connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*)));
    m_canvas = new QSimpleCanvas(QSimpleCanvas::SimpleCanvas, this);
    splitter->addWidget(m_tree);
    splitter->addWidget(m_canvas);
    splitter->setStretchFactor(1, 2);
    layout->addWidget(splitter);
}

void QmlCanvasDebugger::refresh()
{
    setCanvas(m_debugCanvas);
}

class QmlCanvasDebuggerItem : public QTreeWidgetItem
{
public:
    QmlCanvasDebuggerItem(QTreeWidget *tree)
        : QTreeWidgetItem(tree), me(0)
    {
    }

    QmlCanvasDebuggerItem(QTreeWidgetItem *item)
        : QTreeWidgetItem(item), me(0)
    {
    }

    QPointer<QObject> them;
    QFxRect *me;
};

void QmlCanvasDebugger::itemExpanded(QTreeWidgetItem *i)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);
    if(item->me)
        item->me->setOpacity(1);
}

void QmlCanvasDebugger::itemClicked(QTreeWidgetItem *i)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);
    if(item->them)
        emit objectClicked(m_watches->objectId(item->them));
}

void QmlCanvasDebugger::itemCollapsed(QTreeWidgetItem *i)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);
    if(item->me)
        item->me->setOpacity(0);
}

void QmlCanvasDebugger::clone(QTreeWidgetItem *item, QSimpleCanvasItem *me, QSimpleCanvasItem *them)
{
    const QList<QSimpleCanvasItem *> &children = them->children();

    foreach(QSimpleCanvasItem *child, children) {
        QmlCanvasDebuggerItem *childItem = new QmlCanvasDebuggerItem(item);
        childItem->setText(0, QmlWatches::objectToString(child));
        childItem->setExpanded(true);

        QFxRect *rect = new QFxRect;
        rect->setParent(me);
        rect->setX(child->x());
        rect->setY(child->y());
        rect->setWidth(child->width());
        rect->setHeight(child->height());

        if(child->hasActiveFocus())
            rect->setColor(QColor(0, 0, 0, 10));
        else if(child->options() & QSimpleCanvasItem::IsFocusPanel)
            rect->setColor(QColor(0, 255, 0, 10));
        else if(child->options() & QSimpleCanvasItem::IsFocusRealm)
            rect->setColor(QColor(0, 0, 255, 10));
        else
            rect->setColor(QColor(255, 0, 0, 10));

        childItem->them = child;
        childItem->me = rect;

        clone(childItem, rect, child);
    }
}

void QmlCanvasDebugger::setCanvas(QSimpleCanvas *canvas)
{
    QList<QSimpleCanvasItem *> children = m_canvas->root()->children();
    qDeleteAll(children);
    m_tree->clear();

    m_debugCanvas = canvas;
    if(!m_debugCanvas)
        return;

    QTreeWidgetItem *root = new QmlCanvasDebuggerItem(m_tree);
    root->setText(0, "Root");
    root->setExpanded(true);
    clone(root, m_canvas->root(), m_debugCanvas->root());
}


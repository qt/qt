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
#include <QtGui/qspinbox.h>
#include <QtGui/qsplitter.h>
#include <QtGui/qtreewidget.h>

#include <QtDeclarative/qfxrect.h>
#include <QtDeclarative/qfximage.h>

QmlCanvasDebugger::QmlCanvasDebugger(QmlWatches *w, QWidget *parent)
: QWidget(parent), m_watches(w), m_tree(0), m_canvas(0), m_canvasRoot(0), m_debugCanvas(0),
  m_selected(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    setLayout(layout);
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->addStretch(2);
    hlayout->setSpacing(0);
    layout->addLayout(hlayout);
    QSpinBox *x = new QSpinBox(this);
    x->setSingleStep(50);
    x->setMaximum(10000);
    x->setMinimum(-10000);
    QObject::connect(x, SIGNAL(valueChanged(int)), this, SLOT(setX(int)));
    QSpinBox *y = new QSpinBox(this);
    y->setSingleStep(50);
    y->setMaximum(10000);
    y->setMinimum(-10000);
    QObject::connect(y, SIGNAL(valueChanged(int)), this, SLOT(setY(int)));
    hlayout->addWidget(x);
    hlayout->addWidget(y);
    QPushButton *pb = new QPushButton("Refresh", this);
    QObject::connect(pb, SIGNAL(clicked()), this, SLOT(refresh()));
    hlayout->addWidget(pb);

    QSplitter *splitter = new QSplitter(this);

    m_tree = new QTreeWidget(this);
    QObject::connect(m_tree, SIGNAL(itemExpanded(QTreeWidgetItem*)), 
                     this, SLOT(itemExpanded(QTreeWidgetItem*)));
    QObject::connect(m_tree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), 
                     this, SLOT(itemCollapsed(QTreeWidgetItem*)));
    QObject::connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), 
                     this, SLOT(itemClicked(QTreeWidgetItem*)));
    m_canvas = new QSimpleCanvas(QSimpleCanvas::SimpleCanvas, this);
    m_canvasRoot = new QSimpleCanvasItem;
    m_canvasRoot->setParent(m_canvas->root());
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
        : QTreeWidgetItem(tree), me(0), img(0)
    {
    }

    QmlCanvasDebuggerItem(QTreeWidgetItem *item)
        : QTreeWidgetItem(item), me(0), img(0)
    {
    }

    QPointer<QObject> them;
    QFxRect *me;
    QFxImage *img;
};

void QmlCanvasDebugger::itemExpanded(QTreeWidgetItem *i)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);
    if(item->me)
        item->me->setOpacity(1);
}

void QmlCanvasDebugger::setOpacityRecur(QTreeWidgetItem *i, qreal op)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);
    if(item->img)
        item->img->setOpacity(op);

    for(int ii = 0; ii < item->childCount(); ++ii)
        setOpacityRecur(item->child(ii), op);
}

void QmlCanvasDebugger::itemClicked(QTreeWidgetItem *i)
{
    QmlCanvasDebuggerItem *item = static_cast<QmlCanvasDebuggerItem *>(i);
    if(item->them)
        emit objectClicked(m_watches->objectId(item->them));

    if(m_selected) {
        setOpacityRecur(m_selected, 0);
        m_selected = 0;
    }
        
    m_selected = item;
    setOpacityRecur(m_selected, 1);
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
        rect->setZ(child->z());
        rect->setWidth(child->width());
        rect->setHeight(child->height());
        rect->setTransformOrigin(child->transformOrigin());
        rect->setScale(child->scale());
        rect->setFlip(child->flip());
        rect->setTransform(child->transform());

        if(child->hasActiveFocus())
            rect->setColor(QColor(0, 0, 0, 10));
        else if(child->options() & QSimpleCanvasItem::IsFocusPanel)
            rect->setColor(QColor(0, 255, 0, 10));
        else if(child->options() & QSimpleCanvasItem::IsFocusRealm)
            rect->setColor(QColor(0, 0, 255, 10));
        else
            rect->setColor(QColor(255, 0, 0, 10));

        if(child->width() > 0 && child->height() > 0) {
            QPixmap pix(child->width(), child->height());
            pix.fill(QColor(0,0,0,0));
            QPainter p(&pix);
            child->paintContents(p);
            QFxImage *img = new QFxImage;
            img->setParent(rect);
            img->setWidth(child->width());
            img->setHeight(child->height());
            img->setPixmap(pix);
            img->setOpacity(0);
            childItem->img = img;
        }

        childItem->them = child;
        childItem->me = rect;

        clone(childItem, rect, child);
    }
}

void QmlCanvasDebugger::setX(int x)
{
    m_canvasRoot->setX(x);
}

void QmlCanvasDebugger::setY(int y)
{
    m_canvasRoot->setY(y);
}

void QmlCanvasDebugger::setCanvas(QSimpleCanvas *canvas)
{
    QList<QSimpleCanvasItem *> children = m_canvasRoot->children();
    qDeleteAll(children);
    m_tree->clear();
    m_selected = 0;

    m_debugCanvas = canvas;
    if(!m_debugCanvas)
        return;

    QTreeWidgetItem *root = new QmlCanvasDebuggerItem(m_tree);
    root->setText(0, "Root");
    root->setExpanded(true);
    clone(root, m_canvasRoot, m_debugCanvas->root());
}


/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsAnchorLayout>
#include <QtGui>

static QGraphicsProxyWidget *createItem(const QSizeF &minimum = QSizeF(100.0, 100.0),
                                   const QSizeF &preferred = QSize(150.0, 100.0),
                                   const QSizeF &maximum = QSizeF(200.0, 100.0),
                                   const QString &name = "0")
{
    QGraphicsProxyWidget *w = new QGraphicsProxyWidget;
    w->setWidget(new QPushButton(name));
    w->setData(0, name);
    w->setMinimumSize(minimum);
    w->setPreferredSize(preferred);
    w->setMaximumSize(maximum);

    return w;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QGraphicsScene scene;
    scene.setSceneRect(0, 0, 800, 480);

    QSizeF minSize(30, 100);
    QSizeF prefSize(210, 100);
    QSizeF maxSize(300, 100);

    QGraphicsProxyWidget *a = createItem(minSize, prefSize, maxSize, "A");
    QGraphicsProxyWidget *b = createItem(minSize, prefSize, maxSize, "B");
    QGraphicsProxyWidget *c = createItem(minSize, prefSize, maxSize, "C");
    QGraphicsProxyWidget *d = createItem(minSize, prefSize, maxSize, "D");
    QGraphicsProxyWidget *e = createItem(minSize, prefSize, maxSize, "E");
    QGraphicsProxyWidget *f = createItem(QSizeF(30, 50), QSizeF(150, 50), maxSize, "F");
    QGraphicsProxyWidget *g = createItem(QSizeF(30, 50), QSizeF(30, 100), maxSize, "G");

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;

    QGraphicsWidget *w = new QGraphicsWidget(0, Qt::Window);
    w->setPos(20, 20);
    w->setLayout(l);

    // vertical
    l->addAnchor(a, Qt::AnchorTop, l, Qt::AnchorTop);
    l->setAnchorSpacing(a, Qt::AnchorTop, l, Qt::AnchorTop, 0);
    l->addAnchor(b, Qt::AnchorTop, l, Qt::AnchorTop);
    l->setAnchorSpacing(b, Qt::AnchorTop, l, Qt::AnchorTop, 0);

    l->addAnchor(c, Qt::AnchorTop, a, Qt::AnchorBottom);
    l->setAnchorSpacing(c, Qt::AnchorTop, a, Qt::AnchorBottom, 0);
    l->addAnchor(c, Qt::AnchorTop, b, Qt::AnchorBottom);
    l->setAnchorSpacing(c, Qt::AnchorTop, b, Qt::AnchorBottom, 0);
    l->addAnchor(c, Qt::AnchorBottom, d, Qt::AnchorTop);
    l->setAnchorSpacing(c, Qt::AnchorBottom, d, Qt::AnchorTop, 0);
    l->addAnchor(c, Qt::AnchorBottom, e, Qt::AnchorTop);
    l->setAnchorSpacing(c, Qt::AnchorBottom, e, Qt::AnchorTop, 0);

    l->addAnchor(d, Qt::AnchorBottom, l, Qt::AnchorBottom);
    l->setAnchorSpacing(d, Qt::AnchorBottom, l, Qt::AnchorBottom, 0);
    l->addAnchor(e, Qt::AnchorBottom, l, Qt::AnchorBottom);
    l->setAnchorSpacing(e, Qt::AnchorBottom, l, Qt::AnchorBottom, 0);

    l->addAnchor(c, Qt::AnchorTop, f, Qt::AnchorTop);
    l->setAnchorSpacing(c, Qt::AnchorTop, f, Qt::AnchorTop, 0);
    l->addAnchor(c, Qt::AnchorVerticalCenter, f, Qt::AnchorBottom);
    l->setAnchorSpacing(c, Qt::AnchorVerticalCenter, f, Qt::AnchorBottom, 0);
    l->addAnchor(f, Qt::AnchorBottom, g, Qt::AnchorTop);
    l->setAnchorSpacing(f, Qt::AnchorBottom, g, Qt::AnchorTop, 0);
    l->addAnchor(c, Qt::AnchorBottom, g, Qt::AnchorBottom);
    l->setAnchorSpacing(c, Qt::AnchorBottom, g, Qt::AnchorBottom, 0);

    // horizontal
    l->addAnchor(l, Qt::AnchorLeft, a, Qt::AnchorLeft);
    l->setAnchorSpacing(l, Qt::AnchorLeft, a, Qt::AnchorLeft, 0);
    l->addAnchor(l, Qt::AnchorLeft, d, Qt::AnchorLeft);
    l->setAnchorSpacing(l, Qt::AnchorLeft, d, Qt::AnchorLeft, 0);
    l->addAnchor(a, Qt::AnchorRight, b, Qt::AnchorLeft);
    l->setAnchorSpacing(a, Qt::AnchorRight, b, Qt::AnchorLeft, 0);

    l->addAnchor(a, Qt::AnchorRight, c, Qt::AnchorLeft);
    l->setAnchorSpacing(a, Qt::AnchorRight, c, Qt::AnchorLeft, 0);
    l->addAnchor(c, Qt::AnchorRight, e, Qt::AnchorLeft);
    l->setAnchorSpacing(c, Qt::AnchorRight, e, Qt::AnchorLeft, 0);

    l->addAnchor(b, Qt::AnchorRight, l, Qt::AnchorRight);
    l->setAnchorSpacing(b, Qt::AnchorRight, l, Qt::AnchorRight, 0);
    l->addAnchor(e, Qt::AnchorRight, l, Qt::AnchorRight);
    l->setAnchorSpacing(e, Qt::AnchorRight, l, Qt::AnchorRight, 0);
    l->addAnchor(d, Qt::AnchorRight, e, Qt::AnchorLeft);
    l->setAnchorSpacing(d, Qt::AnchorRight, e, Qt::AnchorLeft, 0);

    l->addAnchor(l, Qt::AnchorLeft, f, Qt::AnchorLeft);
    l->setAnchorSpacing(l, Qt::AnchorLeft, f, Qt::AnchorLeft, 0);
    l->addAnchor(l, Qt::AnchorLeft, g, Qt::AnchorLeft);
    l->setAnchorSpacing(l, Qt::AnchorLeft, g, Qt::AnchorLeft, 0);
    l->addAnchor(f, Qt::AnchorRight, g, Qt::AnchorRight);
    l->setAnchorSpacing(f, Qt::AnchorRight, g, Qt::AnchorRight, 0);

    scene.addItem(w);
    scene.setBackgroundBrush(Qt::darkGreen);
    QGraphicsView *view = new QGraphicsView(&scene);
    view->show();

    return app.exec();
}

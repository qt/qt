/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "flippablepad.h"
#include "padnavigator.h"
#include "splashitem.h"
#include "ui_form.h"

#include <QtGui/QtGui>
#ifndef QT_NO_OPENGL
#include <QtOpenGL/QtOpenGL>
#endif

PadNavigator::PadNavigator(const QSize &size, QWidget *parent)
    : QGraphicsView(parent)
{
    // Prepare splash item
    SplashItem *splash = new SplashItem;
    splash->setZValue(1);

    // Prepare pad item
    FlippablePad *pad = new FlippablePad(size);
    QGraphicsRotation *flipRotation = new QGraphicsRotation(pad);
    flipRotation->setAxis(Qt::YAxis);
    QGraphicsRotation *xRotation = new QGraphicsRotation(pad);
    xRotation->setAxis(Qt::YAxis);
    QGraphicsRotation *yRotation = new QGraphicsRotation(pad);
    yRotation->setAxis(Qt::XAxis);
    pad->setTransformations(QList<QGraphicsTransform *>()
                            << flipRotation
                            << xRotation << yRotation);

    // Prepare backitem
    form = new Ui_Form;
    QWidget *widget = new QWidget;
    form->setupUi(widget);
    form->hostName->setFocus();
    QGraphicsProxyWidget *backItem = new QGraphicsProxyWidget(pad);
    backItem->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    backItem->setWidget(widget);
    backItem->setTransform(QTransform()
                           .rotate(180, Qt::YAxis)
                           .translate(-backItem->rect().width()/2, -backItem->rect().height()/2));
    backItem->setFocus();
    backItem->setVisible(false);

    // Prepare selection item
    RoundRectItem *selectionItem = new RoundRectItem(QRectF(-60, -60, 120, 120),
                                                     Qt::gray, pad);
    selectionItem->setZValue(0.5);

    // Selection animation setup
    QPropertyAnimation *smoothXSelection = new QPropertyAnimation(selectionItem, "x");
    smoothXSelection->setDuration(125);
    smoothXSelection->setEasingCurve(QEasingCurve::InOutQuad);
    QPropertyAnimation *smoothYSelection = new QPropertyAnimation(selectionItem, "y");
    smoothYSelection->setDuration(125);
    smoothYSelection->setEasingCurve(QEasingCurve::InOutQuad);
    QPropertyAnimation *smoothXRotation = new QPropertyAnimation(xRotation, "angle");
    smoothXRotation->setDuration(125);
    smoothXRotation->setEasingCurve(QEasingCurve::InOutQuad);
    QPropertyAnimation *smoothYRotation = new QPropertyAnimation(yRotation, "angle");
    smoothYRotation->setDuration(125);
    smoothYRotation->setEasingCurve(QEasingCurve::InOutQuad);
    QPropertyAnimation *smoothScale = new QPropertyAnimation(pad, "scale");
    smoothScale->setDuration(500);
    smoothScale->setKeyValueAt(0, qVariantValue<qreal>(1.0));
    smoothScale->setKeyValueAt(0.5, qVariantValue<qreal>(0.7));
    smoothScale->setKeyValueAt(1, qVariantValue<qreal>(1.0));
    smoothScale->setEasingCurve(QEasingCurve::InOutQuad);

    // Flip animation setup
    QPropertyAnimation *smoothFlipRotation = new QPropertyAnimation(flipRotation, "angle");
    smoothFlipRotation->setDuration(500);
    smoothFlipRotation->setEasingCurve(QEasingCurve::InOutQuad);
    QPropertyAnimation *flipSmoothXRotation = new QPropertyAnimation(xRotation, "angle");
    flipSmoothXRotation->setDuration(500);
    flipSmoothXRotation->setEasingCurve(QEasingCurve::InOutQuad);
    QPropertyAnimation *flipSmoothYRotation = new QPropertyAnimation(yRotation, "angle");
    flipSmoothYRotation->setDuration(500);
    flipSmoothYRotation->setEasingCurve(QEasingCurve::InOutQuad);
    QPropertyAnimation *setBackItemVisibleAnim = new QPropertyAnimation(backItem, "visible");
    setBackItemVisibleAnim->setDuration(0);
    QPropertyAnimation *setSelectionItemVisibleAnim = new QPropertyAnimation(selectionItem, "visible");
    setSelectionItemVisibleAnim->setDuration(0);
    QPropertyAnimation *setFillAnim = new QPropertyAnimation(pad, "fill");
    setFillAnim->setDuration(0);
    QSequentialAnimationGroup *setVisibleAnimation = new QSequentialAnimationGroup;
    setVisibleAnimation->addPause(250);
    setVisibleAnimation->addAnimation(setBackItemVisibleAnim);
    setVisibleAnimation->addAnimation(setSelectionItemVisibleAnim);
    setVisibleAnimation->addAnimation(setFillAnim);
    QParallelAnimationGroup *flipAnimation = new QParallelAnimationGroup(this);
    flipAnimation->addAnimation(smoothFlipRotation);
    flipAnimation->addAnimation(smoothScale);
    flipAnimation->addAnimation(flipSmoothXRotation);
    flipAnimation->addAnimation(flipSmoothYRotation);
    flipAnimation->addAnimation(setVisibleAnimation);

    QPropertyAnimation *smoothSplashMove = new QPropertyAnimation(splash, "y");
    smoothSplashMove->setEasingCurve(QEasingCurve::InQuad);
    smoothSplashMove->setDuration(250);
    QPropertyAnimation *smoothSplashOpacity = new QPropertyAnimation(splash, "opacity");
    smoothSplashOpacity->setDuration(250);

    // Build the state machine
    QStateMachine *stateMachine = new QStateMachine(this);
    QState *splashState = new QState(stateMachine);
    QState *frontState = new QState(stateMachine);
    QHistoryState *historyState = new QHistoryState(frontState);
    QState *backState = new QState(stateMachine);
    frontState->assignProperty(splash, "opacity", 0.0);
    frontState->assignProperty(flipRotation, "angle", qVariantValue<qreal>(0.0));
    frontState->assignProperty(backItem, "visible", false);
    frontState->assignProperty(selectionItem, "visible", true);
    frontState->assignProperty(pad, "fill", false);
    backState->assignProperty(flipRotation, "angle", qVariantValue<qreal>(180.0));
    backState->assignProperty(xRotation, "angle", qVariantValue<qreal>(0.0));
    backState->assignProperty(yRotation, "angle", qVariantValue<qreal>(0.0));
    backState->assignProperty(selectionItem, "visible", false);
    backState->assignProperty(backItem, "visible", true);
    backState->assignProperty(pad, "fill", true);
    stateMachine->addDefaultAnimation(smoothXRotation);
    stateMachine->addDefaultAnimation(smoothYRotation);
    stateMachine->addDefaultAnimation(smoothXSelection);
    stateMachine->addDefaultAnimation(smoothYSelection);
    stateMachine->setInitialState(splashState);

    // Transitions
    QEventTransition *anyKeyTransition = new QEventTransition(this, QEvent::KeyPress, splashState);
    anyKeyTransition->setTargetState(frontState);
    anyKeyTransition->addAnimation(smoothSplashMove);
    anyKeyTransition->addAnimation(smoothSplashOpacity);

    QKeyEventTransition *enterTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Enter, backState);
    enterTransition->setTargetState(historyState);
    QKeyEventTransition *returnTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Return, backState);
    returnTransition->setTargetState(historyState);
    QKeyEventTransition *backEnterTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Enter, frontState);
    backEnterTransition->setTargetState(backState);
    QKeyEventTransition *backReturnTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Return, frontState);
    backReturnTransition->setTargetState(backState);
    enterTransition->addAnimation(flipAnimation);
    returnTransition->addAnimation(flipAnimation);
    backEnterTransition->addAnimation(flipAnimation);
    backReturnTransition->addAnimation(flipAnimation);

    // Substates, one for each icon
    int columns = size.width();
    int rows = size.height();
    QVector< QVector< QState * > > stateGrid;
    stateGrid.resize(rows);
    for (int y = 0; y < rows; ++y) {
        stateGrid[y].resize(columns);
        for (int x = 0; x < columns; ++x) {
            QState *state = new QState(frontState);
            stateGrid[y][x] = state;
        }
    }
    frontState->setInitialState(stateGrid[0][0]);
    selectionItem->setPos(pad->iconAt(0, 0)->pos());

    // Enable key navigation using state transitions
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            QState *state = stateGrid[y][x];
            QKeyEventTransition *rightTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Right, state);
            rightTransition->setTargetState(stateGrid[y][(x + 1) % columns]);
            QKeyEventTransition *leftTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Left, state);
            leftTransition->setTargetState(stateGrid[y][((x - 1) + columns) % columns]);
            QKeyEventTransition *downTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Down, state);
            downTransition->setTargetState(stateGrid[(y + 1) % rows][x]);
            QKeyEventTransition *upTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Up, state);
            upTransition->setTargetState(stateGrid[((y - 1) + rows) % rows][x]);

            RoundRectItem *icon = pad->iconAt(x, y);
            state->assignProperty(xRotation, "angle", -icon->x() / 6.0);
            state->assignProperty(yRotation, "angle", icon->y() / 6.0);
            state->assignProperty(selectionItem, "x", icon->x());
            state->assignProperty(selectionItem, "y", icon->y());
            frontState->assignProperty(icon, "visible", true);
            backState->assignProperty(icon, "visible", false);

            QPropertyAnimation *setVisibleAnim = new QPropertyAnimation(icon, "visible");
            setVisibleAnim->setDuration(0);
            setVisibleAnimation->addAnimation(setVisibleAnim);
        }
    }

    // Setup scene
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QPixmap(":/images/blue_angle_swirl.jpg"));
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->addItem(pad);
    scene->setSceneRect(scene->itemsBoundingRect());
    setScene(scene);

    // Adjust splash item
    splash->setPos(-splash->boundingRect().width() / 2, scene->sceneRect().top());
    frontState->assignProperty(splash, "y", splash->y() - 100.0);
    scene->addItem(splash);

    // Setup view
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumSize(50, 50);
    setViewportUpdateMode(FullViewportUpdate);
    setCacheMode(CacheBackground);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform
                   | QPainter::TextAntialiasing);
#ifndef QT_NO_OPENGL
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif

    stateMachine->start();
}

PadNavigator::~PadNavigator()
{
    delete form;
}

void PadNavigator::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

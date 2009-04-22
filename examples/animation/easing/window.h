/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "ui_form.h"
#include "animation.h"

class PixmapItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    PixmapItem(const QPixmap &pix) : QGraphicsPixmapItem(pix)
    {
    }
};

class Window : public QWidget {
    Q_OBJECT
public:
    Window(QWidget *parent = 0);
private slots:
    void curveChanged(int row);
    void pathChanged(int index);
    void periodChanged(double);
    void amplitudeChanged(double);
    void overshootChanged(double);
    
private:
    void createCurveIcons();
    void startAnimation();
    
    Ui::Form m_ui;
    QGraphicsScene m_scene;
    PixmapItem *m_item;
    Animation *m_anim;
    QSize m_iconSize;


};

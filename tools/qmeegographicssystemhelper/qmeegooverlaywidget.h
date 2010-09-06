/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef QMEEGOOVERLAYWIDGET_H
#define QMEEGOOVERLAYWIDGET_H

#include <QWidget>

//! A widget automatically scaling it's content.
/*!
*/

class QMeeGoOverlayWidget : public QWidget
{
public:
    //! Constructs a new scaling widget.
    /*!
     The real surface used for this widget will have the specified
     width and height.
    */
    QMeeGoOverlayWidget(int surfaceWidth, int surfaceHeight, QWidget *parent = 0);


    //! Event filtering function. 
    /*!
      Converts coordinates for mouse/touch event. Do not
      call manually.
    */
    bool eventFilter(QObject *obj, QEvent *event);

    //! Standard override.
    /*!
     The surface scaling on the target paint device is being
     set when the widget is displayed for the first time.
    */
    virtual void showEvent(QShowEvent *event);

private:
    //! Converts coordinates between real & virtual area of the widget.
    QPoint convertPoint(const QPoint &p);

    int sw; /// Surface real width.
    int sh; /// Surface real height.
    float scaleW; /// Width scaling factor.
    float scaleH; /// Height scaling factor.
};

#endif

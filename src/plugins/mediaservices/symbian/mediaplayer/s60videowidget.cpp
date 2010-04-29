/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "s60videowidget.h"
#include <QtGui/private/qwidget_p.h>
#include <QEvent>
#include <coemain.h>    // For CCoeEnv

QT_BEGIN_NAMESPACE

QBlackWidget::QBlackWidget(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);
    setPalette(QPalette(Qt::black));
#if QT_VERSION >= 0x040601 && !defined(__WINSCW__)
    qt_widget_private(this)->extraData()->nativePaintMode = QWExtra::ZeroFill;
    qt_widget_private(this)->extraData()->receiveNativePaintEvents = true;
#endif
}

QBlackWidget::~QBlackWidget()
{
}

void QBlackWidget::beginNativePaintEvent(const QRect& /*controlRect*/)
{
    emit beginVideoWindowNativePaint();
}

void QBlackWidget::endNativePaintEvent(const QRect& /*controlRect*/)
{
    CCoeEnv::Static()->WsSession().Flush();
    emit endVideoWindowNativePaint();
}

void QBlackWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // Do nothing
}

S60VideoWidgetControl::S60VideoWidgetControl(QObject *parent)
    : QVideoWidgetControl(parent)
    , m_widget(0)
    , m_aspectRatioMode(Qt::KeepAspectRatio)
{
    m_widget = new QBlackWidget();
    connect(m_widget, SIGNAL(beginVideoWindowNativePaint()), this, SIGNAL(beginVideoWindowNativePaint()));
    connect(m_widget, SIGNAL(endVideoWindowNativePaint()), this, SIGNAL(endVideoWindowNativePaint()));
    m_widget->installEventFilter(this);
    m_widget->winId();
}

S60VideoWidgetControl::~S60VideoWidgetControl()
{
    delete m_widget;
}

QWidget *S60VideoWidgetControl::videoWidget()
{
    return m_widget;
}

Qt::AspectRatioMode S60VideoWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void S60VideoWidgetControl::setAspectRatioMode(Qt::AspectRatioMode ratio)
{
    if (m_aspectRatioMode == ratio)
        return;

    m_aspectRatioMode = ratio;
    emit widgetUpdated();
}

bool S60VideoWidgetControl::isFullScreen() const
{
    return m_widget->isFullScreen();
}

void S60VideoWidgetControl::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(fullScreen);
}

int S60VideoWidgetControl::brightness() const
{
    return 0;
}

void S60VideoWidgetControl::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int S60VideoWidgetControl::contrast() const
{
     return 0;
}

void S60VideoWidgetControl::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int S60VideoWidgetControl::hue() const
{
    return 0;
}

void S60VideoWidgetControl::setHue(int hue)
{
    Q_UNUSED(hue);
}

int S60VideoWidgetControl::saturation() const
{
    return 0;
}

void S60VideoWidgetControl::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}

bool S60VideoWidgetControl::eventFilter(QObject *object, QEvent *e)
{
    if (object == m_widget) {
        if (   e->type() == QEvent::Resize
            || e->type() == QEvent::Move
            || e->type() == QEvent::WinIdChange
            || e->type() == QEvent::ParentChange
            || e->type() == QEvent::Show)
            emit widgetUpdated();
    }
    return false;
}

WId S60VideoWidgetControl::videoWidgetWId()
{
    if (m_widget->internalWinId())
        return m_widget->internalWinId();

    if (m_widget->effectiveWinId())
        return m_widget->effectiveWinId();

    return NULL;
}

void S60VideoWidgetControl::videoStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState) {
#if QT_VERSION <= 0x040600 && !defined(FF_QT)
        qt_widget_private(m_widget)->extraData()->disableBlit = false;
#endif
        m_widget->repaint();
    } else if (state == QMediaPlayer::PlayingState) {
#if QT_VERSION <= 0x040600 && !defined(FF_QT)
        qt_widget_private(m_widget)->extraData()->disableBlit = true;
#endif
    }
}

QT_END_NAMESPACE

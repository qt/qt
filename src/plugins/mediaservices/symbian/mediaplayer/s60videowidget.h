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

#ifndef S60VIDEOWIDGET_H
#define S60VIDEOWIDGET_H

#include <qvideowidgetcontrol.h>
#include <qmediaplayer.h>

QT_BEGIN_NAMESPACE

class QBlackWidget : public QWidget
{
    Q_OBJECT

public:
    QBlackWidget(QWidget *parent = 0);
    virtual ~QBlackWidget();

signals:
    void beginVideoWindowNativePaint();
    void endVideoWindowNativePaint();

public slots:
    void beginNativePaintEvent(const QRect&);
    void endNativePaintEvent(const QRect&);

protected:
    void paintEvent(QPaintEvent *event);
};

class S60VideoWidgetControl : public QVideoWidgetControl
{
    Q_OBJECT

public:
    S60VideoWidgetControl(QObject *parent = 0);
    virtual ~S60VideoWidgetControl();

    // from QVideoWidgetControl
    QWidget *videoWidget();
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode ratio);
    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);
    int brightness() const;
    void setBrightness(int brightness);
    int contrast() const;
    void setContrast(int contrast);
    int hue() const;
    void setHue(int hue);
    int saturation() const;
    void setSaturation(int saturation);

    // from QObject
    bool eventFilter(QObject *object, QEvent *event);

    //new methods
    WId videoWidgetWId();

signals:
    void widgetUpdated();
    void beginVideoWindowNativePaint();
    void endVideoWindowNativePaint();

private slots:
    void videoStateChanged(QMediaPlayer::State state);

private:
    QBlackWidget *m_widget;
    Qt::AspectRatioMode m_aspectRatioMode;
};

QT_END_NAMESPACE


#endif // S60VIDEOWIDGET_H

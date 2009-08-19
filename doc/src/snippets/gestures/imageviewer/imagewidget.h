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

#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPixmap>

#include <QtGui>

#include "tapandholdgesture.h"

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    ImageWidget(QWidget *parent = 0);

    void openDirectory(const QString &path);

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void timerEvent(QTimerEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);

//! [imagewidget-slots]
private slots:
    void gestureTriggered();
    void gestureFinished();
    void gestureCancelled();
//! [imagewidget-slots]

private:
    void updateImage();
    QImage loadImage(const QString &fileName);
    void loadImage();
    void setZoomedIn(bool zoomed);
    void goNextImage();
    void goPrevImage();
    void goToImage(int index);

    QPanGesture *panGesture;
    TapAndHoldGesture *tapAndHoldGesture;

    QString path;
    QStringList files;
    int position;

    QImage prevImage, nextImage;
    QImage currentImage;
    QPixmap pixmap;
    QTransform transformation;

    bool zoomedIn;
    int horizontalOffset;
    int verticalOffset;

    bool zoomed;
    qreal zoom;
    bool rotated;
    qreal angle;

    struct TouchFeedback
    {
        bool tapped;
        QPoint position;
        bool sliding;
        QPoint slidingStartPosition;
        QBasicTimer tapTimer;
        int tapState;
        bool doubleTapped;
        int tapAndHoldState;

        enum { MaximumNumberOfTouches = 5 };
        QPoint touches[MaximumNumberOfTouches];

        inline TouchFeedback() { reset(); }
        inline void reset()
        {
            tapped = false;
            sliding = false;
            tapTimer.stop();
            tapState = 0;
            doubleTapped = false;
            tapAndHoldState = 0;
            for (int i = 0; i < MaximumNumberOfTouches; ++i) {
                touches[i] = QPoint();
            }
        }
    } touchFeedback;
    QBasicTimer feedbackFadeOutTimer;
};

#endif

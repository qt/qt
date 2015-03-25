/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMOUSE_QWS_H
#define QMOUSE_QWS_H

#include <QtCore/qobject.h>
#include <QtGui/qpolygon.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QWSMouseHandlerPrivate;
class QScreen;

class Q_GUI_EXPORT QWSPointerCalibrationData
{
public:
    enum Location { TopLeft = 0, BottomLeft = 1, BottomRight = 2, TopRight = 3,
                    Center = 4, LastLocation = Center };
    QPoint devPoints[5];
    QPoint screenPoints[5];
};

class Q_GUI_EXPORT QWSMouseHandler
{
public:
    explicit QWSMouseHandler(const QString &driver = QString(),
                             const QString &device = QString());
    virtual ~QWSMouseHandler();

    virtual void clearCalibration() {}
    virtual void calibrate(const QWSPointerCalibrationData *) {}
    virtual void getCalibration(QWSPointerCalibrationData *) const {}

    virtual void resume() = 0;
    virtual void suspend() = 0;

    void limitToScreen(QPoint &pt);
    void mouseChanged(const QPoint& pos, int bstate, int wheel = 0);
    const QPoint &pos() const { return mousePos; }

    void setScreen(const QScreen *screen);

protected:
    QPoint &mousePos;
    QWSMouseHandlerPrivate *d_ptr;
};


class Q_GUI_EXPORT QWSCalibratedMouseHandler : public QWSMouseHandler
{
public:
    explicit QWSCalibratedMouseHandler(const QString &driver = QString(),
                                       const QString &device = QString());

    virtual void clearCalibration();
    virtual void calibrate(const QWSPointerCalibrationData *);
    virtual void getCalibration(QWSPointerCalibrationData *) const;

protected:
    bool sendFiltered(const QPoint &, int button);
    QPoint transform(const QPoint &);

    void readCalibration();
    void writeCalibration();
    void setFilterSize(int);

private:
    int a, b, c;
    int d, e, f;
    int s;
    QPolygon samples;
    int currSample;
    int numSamples;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMOUSE_QWS_H

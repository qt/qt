/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef QMOUSEINTEGRITY_QWS_H
#define QMOUSEINTEGRITY_QWS_H

#include <QtGui/qmouse_qws.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_QWS_MOUSE_INTEGRITY

class QSocketNotifier;
class QIntMousePrivate;

class QIntMouseHandler : public QObject, public QWSCalibratedMouseHandler {
    Q_OBJECT
public:
    QIntMouseHandler(const QString &driver = QString(),
            const QString &device = QString());
    ~QIntMouseHandler();

    void resume();
    void suspend();

    void calibrate(const QWSPointerCalibrationData *data);
    void clearCalibration();

private:
    QIntMousePrivate *d;
private Q_SLOTS:
    void readMouseData(int x, int y, int buttons);
};
#endif // QT_NO_QWS_MOUSE_INTEGRITY

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMOUSEINTEGRITY_QWS_H

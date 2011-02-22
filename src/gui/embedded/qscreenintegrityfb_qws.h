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

#ifndef QSCREENINTEGRITYFB_QWS_H
#define QSCREENINTEGRITYFB_QWS_H

#include <QtGui/qscreen_qws.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_QWS_INTEGRITYFB

class QIntfbScreenPrivate;

class Q_GUI_EXPORT QIntfbScreen : public QScreen
{
public:
    explicit QIntfbScreen(int display_id);
    virtual ~QIntfbScreen();
    virtual bool initDevice();
    virtual bool connect(const QString &displaySpec);
    virtual void disconnect();
    virtual void shutdownDevice();
    virtual void save();
    virtual void restore();
    virtual void setMode(int nw,int nh,int nd);
    virtual void setDirty(const QRect& r);
    virtual void blank(bool);
    static void setBrightness(int b);

private:
    QIntfbScreenPrivate *d_ptr;
};

#endif // QT_NO_QWS_INTEGRITYFB

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSCREENINTEGRITYFB_QWS_H

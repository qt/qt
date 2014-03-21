/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWSPROTOCOLITEM_QWS_H
#define QWSPROTOCOLITEM_QWS_H

/*********************************************************************
 *
 * QWSCommand base class - only use derived classes from that
 *
 *********************************************************************/

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QIODevice;

struct QWSProtocolItem
{
    // ctor - dtor
    QWSProtocolItem(int t, int len, char *ptr) : type(t),
        simpleLen(len), rawLen(-1), deleteRaw(false), simpleDataPtr(ptr),
        rawDataPtr(0), bytesRead(0) { }
    virtual ~QWSProtocolItem();

    // data
    int type;
    int simpleLen;
    int rawLen;
    bool deleteRaw;

    // functions
#ifndef QT_NO_QWS_MULTIPROCESS
    void write(QIODevice *s);
    bool read(QIODevice *s);
#endif
    void copyFrom(const QWSProtocolItem *item);

    virtual void setData(const char *data, int len, bool allocateMem = true);

    char *simpleDataPtr;
    char *rawDataPtr;
    // temp variables
    int bytesRead;
};

// This should probably be a method on QWSProtocolItem, but this way avoids
// changing the API of this apparently public header
// size = (int)type + (int)rawLenSize + simpleLen + rawLen
#define QWS_PROTOCOL_ITEM_SIZE( item ) \
    (2 * sizeof(int)) + ((item).simpleDataPtr ? (item).simpleLen : 0) + ((item).rawDataPtr ? (item).rawLen : 0)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QWSPROTOCOLITEM_QWS_H

/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include "qbaselinetest.h"
#include "baselineprotocol.h"

namespace QBaselineTest {

bool connected = false;
bool triedConnecting = false;
BaselineProtocol proto;


bool checkImage(const QImage& img, const char *name, quint16 checksum, QByteArray *msg, bool *error)
{
    QByteArray itemName;
    bool hasName = qstrlen(name);
    const char *tag = QTest::currentDataTag();
    if (qstrlen(tag)) {
        itemName = tag;
        if (hasName)
            itemName.append('_').append(name);
    } else {
        itemName = hasName ? name : "default_name";
    }

    *msg = "Baseline check of image '" + itemName + "': ";

    if (!triedConnecting) {
        triedConnecting = true;
        if (!proto.connect(QTest::testObject()->metaObject()->className())) {
            *msg += "Failed to connect to baseline server: " + proto.errorMessage().toLatin1();
            *error = true;
            return true;
        }
        connected = true;
    }
    if (!connected) {
        *msg = "Not connected to baseline server.";
        *error = true;
        return true;
    }

    ImageItem item;
    item.itemName = QLatin1String(itemName);
    item.itemChecksum = checksum;
    item.testFunction = QLatin1String(QTest::currentTestFunction());
    ImageItemList list;
    list.append(item);
    if (!proto.requestBaselineChecksums(QLatin1String(QTest::currentTestFunction()), &list) || list.isEmpty()) {
        *msg = "Communication with baseline server failed: " + proto.errorMessage().toLatin1();
        *error = true;
        return true;
    }
    item.image = img;
    item.imageChecksums.prepend(ImageItem::computeChecksum(img));
    QByteArray srvMsg;
    switch (list.at(0).status) {
    case ImageItem::IgnoreItem :
        qDebug() << msg->constData() << "Ignored, blacklisted on server.";
        return true;
        break;
    case ImageItem::BaselineNotFound:
        if (proto.submitNewBaseline(item, &srvMsg))
            qDebug() << msg->constData() << "Baseline not found on server. New baseline uploaded.";
        else
            qDebug() << msg->constData() << "Baseline not found on server. Uploading of new baseline failed:" << srvMsg;
        return true;
        break;
    case ImageItem::Ok:
        break;
    default:
        qWarning() << "Unexpected reply from baseline server.";
        return true;
        break;
    }
    *error = false;
    // The actual comparison of the given image with the baseline:
    if (list.at(0).imageChecksums.contains(item.imageChecksums.at(0)))
        return true;
    proto.submitMismatch(item, &srvMsg);
    *msg += "Mismatch. See report:\n   " + srvMsg;
    return false;
}

}

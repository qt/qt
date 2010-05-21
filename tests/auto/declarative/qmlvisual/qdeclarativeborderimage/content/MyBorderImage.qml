/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

import Qt 4.7

Item {
    property alias horizontalMode: image.horizontalTileMode
    property alias verticalMode: image.verticalTileMode
    property alias source: image.source
    property alias antialiased: image.smooth

    property int minWidth
    property int minHeight
    property int maxWidth
    property int maxHeight
    property int margin

    id: container
    width: 240; height: 240

    BorderImage {
        id: image; x: container.width / 2 - width / 2; y: container.height / 2 - height / 2

        SequentialAnimation on width {
            loops: Animation.Infinite
            NumberAnimation { from: container.minWidth; to: container.maxWidth; duration: 2000; easing.type: "InOutQuad"}
            NumberAnimation { from: container.maxWidth; to: container.minWidth; duration: 2000; easing.type: "InOutQuad" }
        }

        SequentialAnimation on height {
            loops: Animation.Infinite
            NumberAnimation { from: container.minHeight; to: container.maxHeight; duration: 2000; easing.type: "InOutQuad"}
            NumberAnimation { from: container.maxHeight; to: container.minHeight; duration: 2000; easing.type: "InOutQuad" }
        }

        border.top: container.margin
        border.left: container.margin
        border.bottom: container.margin
        border.right: container.margin
    }
}

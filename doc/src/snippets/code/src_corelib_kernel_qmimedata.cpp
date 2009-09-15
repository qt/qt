/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
void MyWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MyWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        foreach (QUrl url, event->mimeData()->urls()) {
            ...
        }
    }
}
//! [0]


//! [1]
QByteArray csvData = ...;

QMimeData *mimeData = new QMimeData;
mimeData->setData("text/csv", csvData);
//! [1]


//! [2]
void MyWidget::dropEvent(QDropEvent *event)
{
    const MyMimeData *myData =
            qobject_cast<const MyMimeData *>(event->mimeData());
    if (myData) {
        // access myData's data directly (not through QMimeData's API)
    }
}
//! [2]


//! [3]
application/x-qt-windows-mime;value="<custom type>"
//! [3]


//! [4]
application/x-qt-windows-mime;value="FileGroupDescriptor"
application/x-qt-windows-mime;value="FileContents"
//! [4]


//! [5]
if (event->mimeData()->hasImage()) {
    QImage image = qvariant_cast<QImage>(event->mimeData()->imageData());
    ...
}
//! [5]


//! [6]
mimeData->setImageData(QImage("beautifulfjord.png"));
//! [6]


//! [7]
if (event->mimeData()->hasColor()) {
    QColor color = qvariant_cast<QColor>(event->mimeData()->colorData());
    ...
}
//! [7]

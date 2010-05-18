/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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


#include <qdeclarativeextensionplugin.h>

#include <qdeclarativeengine.h>
#include <qdeclarativecontext.h>
#include <qdeclarative.h>
#include <qdeclarativeitem.h>
#include <qdeclarativeimageprovider.h>
#include <qdeclarativeview.h>
#include <QImage>
#include <QPainter>

/*
   This example illustrates using a QDeclarativeImageProvider to serve
   images asynchronously.
*/

//![0]
class ColorImageProvider : public QDeclarativeImageProvider
{
public:
    // This is run in a low priority thread.
    QImage request(const QString &id, QSize *size, const QSize &req_size)
    {
        if (size) *size = QSize(100,50);
        QImage image(
                req_size.width() > 0 ? req_size.width() : 100,
                req_size.height() > 0 ? req_size.height() : 50,
                QImage::Format_RGB32);
        image.fill(QColor(id).rgba());
        QPainter p(&image);
        QFont f = p.font();
        f.setPixelSize(30);
        p.setFont(f);
        p.setPen(Qt::black);
        if (req_size.isValid())
            p.scale(req_size.width()/100.0, req_size.height()/50.0);
        p.drawText(QRectF(0,0,100,50),Qt::AlignCenter,id);
        return image;
    }
};


class ImageProviderExtensionPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri) {
        Q_UNUSED(uri);

    }

    void initializeEngine(QDeclarativeEngine *engine, const char *uri) {
        Q_UNUSED(uri);

        engine->addImageProvider("colors", new ColorImageProvider);

        QStringList dataList;
        dataList.append("image://colors/red");
        dataList.append("image://colors/green");
        dataList.append("image://colors/blue");
        dataList.append("image://colors/brown");
        dataList.append("image://colors/orange");
        dataList.append("image://colors/purple");
        dataList.append("image://colors/yellow");

        QDeclarativeContext *ctxt = engine->rootContext();
        ctxt->setContextProperty("myModel", QVariant::fromValue(dataList));
    }

};

#include "imageprovider.moc"

Q_EXPORT_PLUGIN(ImageProviderExtensionPlugin);
//![0]


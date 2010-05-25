/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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


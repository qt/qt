/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QtGui/qapplication.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qsgview.h>
#include <QtDeclarative/qsgpainteditem.h>

class MyPaintItem : public QSGPaintedItem
{
    Q_OBJECT
public:
    virtual void paint(QPainter *p)
    {
        QRectF rect(0, 0, width(), height());
        rect.adjust(10, 10, -10, -10);
        p->setPen(QPen(Qt::black, 20));
        p->setBrush(Qt::yellow);
        p->drawEllipse(rect);
        p->setPen(Qt::black);
        p->setFont(QFont(QLatin1String("Times"), qRound(rect.height() / 2)));
        p->drawText(rect, Qt::AlignCenter, QLatin1String(":-)"));
    }
};

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    qmlRegisterType<MyPaintItem>("MyModule", 1, 0, "MyPaintItem");

    QGLFormat f = QGLFormat::defaultFormat();
    f.setSampleBuffers(true);
    QSGView view(f);
    view.setResizeMode(QSGView::SizeRootObjectToView);
    view.setSource(QUrl::fromLocalFile("myfile.qml"));
    view.show();
    view.raise();

    return app.exec();
}

#include "main.moc"

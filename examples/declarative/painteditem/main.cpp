/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt scene graph research project.
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

#include <QtGui/qapplication.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qsgview.h>
#include <private/qsgpainteditem_p.h>

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

    QSGView view;
    view.setSource(QUrl::fromLocalFile("myfile.qml"));
    view.show();
    view.raise();

    return app.exec();
}

#include "main.moc"

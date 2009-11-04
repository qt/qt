/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qtest.h>
#include <QTextDocument>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlgraphicsborderimage_p.h>
#include <private/qmlgraphicsimagebase_p.h>
#include <private/qmlgraphicsscalegrid_p_p.h>

class tst_qmlgraphicsborderimage : public QObject

{
    Q_OBJECT
public:
    tst_qmlgraphicsborderimage();

private slots:
    void simple();
    void resized();
    void smooth();
    void tileModes();
    void sciFile();

private:
    QmlEngine engine;
};

tst_qmlgraphicsborderimage::tst_qmlgraphicsborderimage()
{
}

void tst_qmlgraphicsborderimage::simple()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\" }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QVERIFY(obj->width() == 120);
    QVERIFY(obj->height() == 120);
    QVERIFY(obj->horizontalTileMode() == QmlGraphicsBorderImage::Stretch);
    QVERIFY(obj->verticalTileMode() == QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::resized()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QVERIFY(obj->width() == 300);
    QVERIFY(obj->height() == 300);
    QVERIFY(obj->horizontalTileMode() == QmlGraphicsBorderImage::Stretch);
    QVERIFY(obj->verticalTileMode() == QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::smooth()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; smooth: true; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QVERIFY(obj->width() == 300);
    QVERIFY(obj->height() == 300);
    QVERIFY(obj->smoothTransform() == true);
    QVERIFY(obj->horizontalTileMode() == QmlGraphicsBorderImage::Stretch);
    QVERIFY(obj->verticalTileMode() == QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::tileModes()
{
    {
        QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 100; height: 300; horizontalTileMode: BorderImage.Repeat; verticalTileMode: BorderImage.Repeat }";
        QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
        QVERIFY(obj != 0);
        QVERIFY(obj->width() == 100);
        QVERIFY(obj->height() == 300);
        QVERIFY(obj->horizontalTileMode() == QmlGraphicsBorderImage::Repeat);
        QVERIFY(obj->verticalTileMode() == QmlGraphicsBorderImage::Repeat);

        delete obj;
    }
    {
        QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 300; height: 150; horizontalTileMode: BorderImage.Round; verticalTileMode: BorderImage.Round }";
        QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
        QVERIFY(obj != 0);
        QVERIFY(obj->width() == 300);
        QVERIFY(obj->height() == 150);
        QVERIFY(obj->horizontalTileMode() == QmlGraphicsBorderImage::Round);
        QVERIFY(obj->verticalTileMode() == QmlGraphicsBorderImage::Round);

        delete obj;
    }
}

void tst_qmlgraphicsborderimage::sciFile()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors-round.sci\"; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QVERIFY(obj->width() == 300);
    QVERIFY(obj->height() == 300);
    QVERIFY(obj->border()->left() == 10);
    QVERIFY(obj->border()->top() == 20);
    QVERIFY(obj->border()->right() == 30);
    QVERIFY(obj->border()->bottom() == 40);
    QVERIFY(obj->horizontalTileMode() == QmlGraphicsBorderImage::Round);
    QVERIFY(obj->verticalTileMode() == QmlGraphicsBorderImage::Repeat);

    delete obj;
}

QTEST_MAIN(tst_qmlgraphicsborderimage)

#include "tst_qmlgraphicsborderimage.moc"

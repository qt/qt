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
    void noSource();
    void simple();
    void resized();
    void smooth();
    void tileModes();
    void sciFile();
    void invalidSciFile();

private:
    QmlEngine engine;
};

tst_qmlgraphicsborderimage::tst_qmlgraphicsborderimage()
{
}

void tst_qmlgraphicsborderimage::noSource()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"\" }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->source(), QUrl());
    QCOMPARE(obj->width(), 0.);
    QCOMPARE(obj->height(), 0.);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::simple()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\" }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->source(), QUrl("file://" SRCDIR "/data/colors.png"));
    QCOMPARE(obj->width(), 120.);
    QCOMPARE(obj->height(), 120.);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::resized()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::smooth()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; smooth: true; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->smooth(), true);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);

    delete obj;
}

void tst_qmlgraphicsborderimage::tileModes()
{
    {
        QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 100; height: 300; horizontalTileMode: BorderImage.Repeat; verticalTileMode: BorderImage.Repeat }";
        QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
        QVERIFY(obj != 0);
        QCOMPARE(obj->width(), 100.);
        QCOMPARE(obj->height(), 300.);
        QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Repeat);
        QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Repeat);

        delete obj;
    }
    {
        QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors.png\"; width: 300; height: 150; horizontalTileMode: BorderImage.Round; verticalTileMode: BorderImage.Round }";
        QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
        QVERIFY(obj != 0);
        QCOMPARE(obj->width(), 300.);
        QCOMPARE(obj->height(), 150.);
        QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Round);
        QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Round);

        delete obj;
    }
}

void tst_qmlgraphicsborderimage::sciFile()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/colors-round.sci\"; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->border()->left(), 10);
    QCOMPARE(obj->border()->top(), 20);
    QCOMPARE(obj->border()->right(), 30);
    QCOMPARE(obj->border()->bottom(), 40);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Round);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Repeat);

    delete obj;
}

void tst_qmlgraphicsborderimage::invalidSciFile()
{
    QString componentStr = "import Qt 4.6\nBorderImage { source: \"" SRCDIR "/data/invalid.sci\"; width: 300; height: 300 }";
    QmlComponent component(&engine, componentStr.toLatin1(), QUrl("file://"));
    QmlGraphicsBorderImage *obj = qobject_cast<QmlGraphicsBorderImage*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->width(), 300.);
    QCOMPARE(obj->height(), 300.);
    QCOMPARE(obj->status(), QmlGraphicsImageBase::Error);
    QCOMPARE(obj->horizontalTileMode(), QmlGraphicsBorderImage::Stretch);
    QCOMPARE(obj->verticalTileMode(), QmlGraphicsBorderImage::Stretch);

    delete obj;
}

QTEST_MAIN(tst_qmlgraphicsborderimage)

#include "tst_qmlgraphicsborderimage.moc"

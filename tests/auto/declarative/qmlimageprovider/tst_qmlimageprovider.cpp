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
#include <QtTest/QtTest>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlimageprovider.h>
#include <private/qmlgraphicsimage_p.h>

// QmlImageProvider::request() is run in an idle thread where possible
// Be generous in our timeout.
#define TRY_WAIT(expr) \
    do { \
        for (int ii = 0; ii < 10; ++ii) { \
            if ((expr)) break; \
            QTest::qWait(100); \
        } \
        QVERIFY((expr)); \
    } while (false)


class tst_qmlimageprovider : public QObject
{
    Q_OBJECT
public:
    tst_qmlimageprovider()
    {
    }

private slots:
    void imageSource();
    void imageSource_data();
    void removeProvider();

private:
    QmlEngine engine;
};

class TestProvider : public QmlImageProvider
{
public:
    QImage request(const QString &id) {
        QImage image;
        image.load(SRCDIR "/data/" + id);
        return image;
    }
};

void tst_qmlimageprovider::imageSource_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("error");

    QTest::newRow("exists") << "image://test/exists.png" << "";
    QTest::newRow("missing") << "image://test/no-such-file.png"
        << "\"Failed to get image from provider: image://test/no-such-file.png\" ";
    QTest::newRow("unknown provider") << "image://bogus/exists.png"
        << "\"Failed to get image from provider: image://bogus/exists.png\" ";
}
    
void tst_qmlimageprovider::imageSource()
{
    QFETCH(QString, source);
    QFETCH(QString, error);

    if (!error.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, error.toUtf8());

    engine.addImageProvider("test", new TestProvider);
    QVERIFY(engine.imageProvider("test") != 0);

    QString componentStr = "import Qt 4.6\nImage { source: \"" + source + "\" }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlGraphicsImage *obj = qobject_cast<QmlGraphicsImage*>(component.create());
    QVERIFY(obj != 0);

    TRY_WAIT(obj->status() == QmlGraphicsImage::Loading);

    QCOMPARE(obj->source(), QUrl(source));

    if (error.isEmpty()) {
        TRY_WAIT(obj->status() == QmlGraphicsImage::Ready);
        QCOMPARE(obj->width(), 100.);
        QCOMPARE(obj->height(), 100.);
        QCOMPARE(obj->fillMode(), QmlGraphicsImage::Stretch);
        QCOMPARE(obj->progress(), 1.0);
    } else {
        TRY_WAIT(obj->status() == QmlGraphicsImage::Error);
    }

    delete obj;
}

void tst_qmlimageprovider::removeProvider()
{
    engine.addImageProvider("test2", new TestProvider);
    QVERIFY(engine.imageProvider("test2") != 0);

    // add provider, confirm it works
    QString componentStr = "import Qt 4.6\nImage { source: \"image://test2/exists1.png\" }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlGraphicsImage *obj = qobject_cast<QmlGraphicsImage*>(component.create());
    QVERIFY(obj != 0);

    TRY_WAIT(obj->status() == QmlGraphicsImage::Loading);
    TRY_WAIT(obj->status() == QmlGraphicsImage::Ready);

    QCOMPARE(obj->width(), 100.0);

    // remove the provider and confirm
    QString error("\"Failed to get image from provider: image://test2/exists2.png\" ");
    QTest::ignoreMessage(QtWarningMsg, error.toUtf8());

    engine.removeImageProvider("test2");

    obj->setSource(QUrl("image://test2/exists2.png"));

    TRY_WAIT(obj->status() == QmlGraphicsImage::Loading);
    TRY_WAIT(obj->status() == QmlGraphicsImage::Error);

    delete obj;
}


QTEST_MAIN(tst_qmlimageprovider)

#include "tst_qmlimageprovider.moc"

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
#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlfontloader_p.h>
#include "../../../shared/util.h"

class tst_qmlfontloader : public QObject

{
    Q_OBJECT
public:
    tst_qmlfontloader();

private slots:
    void noFont();
    void namedFont();
    void localFont();
    void failLocalFont();
    void webFont();
    void failWebFont();

private slots:

private:
    QmlEngine engine;
};

tst_qmlfontloader::tst_qmlfontloader()
{
}

void tst_qmlfontloader::noFont()
{
    QString componentStr = "import Qt 4.6\nFontLoader { }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlFontLoader *fontObject = qobject_cast<QmlFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QCOMPARE(fontObject->name(), QString(""));
    QCOMPARE(fontObject->source(), QUrl(""));
    QTRY_VERIFY(fontObject->status() == QmlFontLoader::Null);

    delete fontObject;
}

void tst_qmlfontloader::namedFont()
{
    QString componentStr = "import Qt 4.6\nFontLoader { name: \"Helvetica\" }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlFontLoader *fontObject = qobject_cast<QmlFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QCOMPARE(fontObject->source(), QUrl(""));
    QCOMPARE(fontObject->name(), QString("Helvetica"));
    QTRY_VERIFY(fontObject->status() == QmlFontLoader::Ready);
}

void tst_qmlfontloader::localFont()
{
    QString componentStr = "import Qt 4.6\nFontLoader { source: \"" SRCDIR  "/data/tarzeau_ocr_a.ttf\" }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlFontLoader *fontObject = qobject_cast<QmlFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QVERIFY(fontObject->source() != QUrl(""));
    QTRY_COMPARE(fontObject->name(), QString("OCRA"));
    QTRY_VERIFY(fontObject->status() == QmlFontLoader::Ready);
}

void tst_qmlfontloader::failLocalFont()
{
    QString componentStr = "import Qt 4.6\nFontLoader { source: \"" + QUrl::fromLocalFile(SRCDIR "/data/dummy.ttf").toString() + "\" }";
    QTest::ignoreMessage(QtWarningMsg, QString("Cannot load font:  QUrl( \"" + QUrl::fromLocalFile(SRCDIR "/data/dummy.ttf").toString() + "\" )  ").toUtf8().constData());
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlFontLoader *fontObject = qobject_cast<QmlFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QVERIFY(fontObject->source() != QUrl(""));
    QTRY_COMPARE(fontObject->name(), QString(""));
    QTRY_VERIFY(fontObject->status() == QmlFontLoader::Error);
}

void tst_qmlfontloader::webFont()
{
    QString componentStr = "import Qt 4.6\nFontLoader { source: \"http://www.princexml.com/fonts/steffmann/Starburst.ttf\" }";
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlFontLoader *fontObject = qobject_cast<QmlFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QVERIFY(fontObject->source() != QUrl(""));
    QTRY_COMPARE(fontObject->name(), QString("Starburst"));
    QTRY_VERIFY(fontObject->status() == QmlFontLoader::Ready);
}

void tst_qmlfontloader::failWebFont()
{
    QString componentStr = "import Qt 4.6\nFontLoader { source: \"http://wrong.address.com/Starburst.ttf\" }";
    QTest::ignoreMessage(QtWarningMsg, "Cannot load font:  QUrl( \"http://wrong.address.com/Starburst.ttf\" )  ");
    QmlComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QmlFontLoader *fontObject = qobject_cast<QmlFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QVERIFY(fontObject->source() != QUrl(""));
    QTRY_COMPARE(fontObject->name(), QString(""));
    QTRY_VERIFY(fontObject->status() == QmlFontLoader::Error);
}

QTEST_MAIN(tst_qmlfontloader)

#include "tst_qmlfontloader.moc"

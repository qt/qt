/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativefontloader_p.h>
#include "../../../shared/util.h"
#include "../shared/testhttpserver.h"

#define SERVER_PORT 14448

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativefontloader : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativefontloader();

private slots:
    void noFont();
    void namedFont();
    void localFont();
    void failLocalFont();
    void webFont();
    void redirWebFont();
    void failWebFont();

private slots:

private:
    QDeclarativeEngine engine;
    TestHTTPServer server;
};

tst_qdeclarativefontloader::tst_qdeclarativefontloader() :
    server(SERVER_PORT)
{
    server.serveDirectory(SRCDIR "/data");
    Q_ASSERT(server.isValid());
}

void tst_qdeclarativefontloader::noFont()
{
    QString componentStr = "import Qt 4.7\nFontLoader { }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeFontLoader *fontObject = qobject_cast<QDeclarativeFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QCOMPARE(fontObject->name(), QString(""));
    QCOMPARE(fontObject->source(), QUrl(""));
    QTRY_VERIFY(fontObject->status() == QDeclarativeFontLoader::Null);

    delete fontObject;
}

void tst_qdeclarativefontloader::namedFont()
{
    QString componentStr = "import Qt 4.7\nFontLoader { name: \"Helvetica\" }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeFontLoader *fontObject = qobject_cast<QDeclarativeFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QCOMPARE(fontObject->source(), QUrl(""));
    QCOMPARE(fontObject->name(), QString("Helvetica"));
    QTRY_VERIFY(fontObject->status() == QDeclarativeFontLoader::Ready);
}

void tst_qdeclarativefontloader::localFont()
{
    QString componentStr = "import Qt 4.7\nFontLoader { source: \"" SRCDIR  "/data/tarzeau_ocr_a.ttf\" }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeFontLoader *fontObject = qobject_cast<QDeclarativeFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QVERIFY(fontObject->source() != QUrl(""));
    QTRY_COMPARE(fontObject->name(), QString("OCRA"));
    QTRY_VERIFY(fontObject->status() == QDeclarativeFontLoader::Ready);
}

void tst_qdeclarativefontloader::failLocalFont()
{
    QString componentStr = "import Qt 4.7\nFontLoader { source: \"" + QUrl::fromLocalFile(SRCDIR "/data/dummy.ttf").toString() + "\" }";
    QTest::ignoreMessage(QtWarningMsg, QString("file::2:1: QML FontLoader: Cannot load font: \"" + QUrl::fromLocalFile(SRCDIR "/data/dummy.ttf").toString() + "\"").toUtf8().constData());
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeFontLoader *fontObject = qobject_cast<QDeclarativeFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QVERIFY(fontObject->source() != QUrl(""));
    QTRY_COMPARE(fontObject->name(), QString(""));
    QTRY_VERIFY(fontObject->status() == QDeclarativeFontLoader::Error);
}

void tst_qdeclarativefontloader::webFont()
{
    QString componentStr = "import Qt 4.7\nFontLoader { source: \"http://localhost:14448/tarzeau_ocr_a.ttf\" }";
    QDeclarativeComponent component(&engine);

    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeFontLoader *fontObject = qobject_cast<QDeclarativeFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QVERIFY(fontObject->source() != QUrl(""));
    QTRY_COMPARE(fontObject->name(), QString("OCRA"));
    QTRY_VERIFY(fontObject->status() == QDeclarativeFontLoader::Ready);
}

void tst_qdeclarativefontloader::redirWebFont()
{
    server.addRedirect("olddir/oldname.ttf","../tarzeau_ocr_a.ttf");

    QString componentStr = "import Qt 4.7\nFontLoader { source: \"http://localhost:14448/olddir/oldname.ttf\" }";
    QDeclarativeComponent component(&engine);

    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeFontLoader *fontObject = qobject_cast<QDeclarativeFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QVERIFY(fontObject->source() != QUrl(""));
    QTRY_COMPARE(fontObject->name(), QString("OCRA"));
    QTRY_VERIFY(fontObject->status() == QDeclarativeFontLoader::Ready);
}

void tst_qdeclarativefontloader::failWebFont()
{
    QString componentStr = "import Qt 4.7\nFontLoader { source: \"http://localhost:14448/nonexist.ttf\" }";
    QTest::ignoreMessage(QtWarningMsg, "file::2:1: QML FontLoader: Cannot load font: \"http://localhost:14448/nonexist.ttf\"");
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeFontLoader *fontObject = qobject_cast<QDeclarativeFontLoader*>(component.create());

    QVERIFY(fontObject != 0);
    QVERIFY(fontObject->source() != QUrl(""));
    QTRY_COMPARE(fontObject->name(), QString(""));
    QTRY_VERIFY(fontObject->status() == QDeclarativeFontLoader::Error);
}

QTEST_MAIN(tst_qdeclarativefontloader)

#include "tst_qdeclarativefontloader.moc"

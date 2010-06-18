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
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDebug>
#include <QDir>
#include <QFile>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_parserstress : public QObject
{
    Q_OBJECT
public:
    tst_parserstress() {}

private slots:
    void ecmascript_data();
    void ecmascript();

private:
    static QStringList findJSFiles(const QDir &);
    QDeclarativeEngine engine;
};

QStringList tst_parserstress::findJSFiles(const QDir &d)
{
    QStringList rv;

    QStringList files = d.entryList(QStringList() << QLatin1String("*.js"),
                                    QDir::Files);
    foreach (const QString &file, files) {
        if (file == "browser.js")
            continue;
        rv << d.absoluteFilePath(file);
    }

    QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot |
                                   QDir::NoSymLinks);
    foreach (const QString &dir, dirs) {
        QDir sub = d;
        sub.cd(dir);
        rv << findJSFiles(sub);
    }

    return rv;
}

void tst_parserstress::ecmascript_data()
{
#ifdef Q_OS_SYMBIAN    
    QDir dir("tests");
#else
    QDir dir(SRCDIR);
    dir.cdUp();
    dir.cdUp();
    dir.cd("qscriptjstestsuite");
    dir.cd("tests");
#endif
    QStringList files = findJSFiles(dir);

    QTest::addColumn<QString>("file");
    foreach (const QString &file, files) {
        QTest::newRow(qPrintable(file)) << file;
    }
}

void tst_parserstress::ecmascript()
{
    QFETCH(QString, file);

    QFile f(file);
    QVERIFY(f.open(QIODevice::ReadOnly));

    QByteArray data = f.readAll();

    QVERIFY(!data.isEmpty());

    QString dataStr = QString::fromUtf8(data);

    QString qml = "import Qt 4.7\n";
            qml+= "\n";
            qml+= "QtObject {\n";
            qml+= "    property int test\n";
            qml+= "    test: {\n";
            qml+= dataStr + "\n";
            qml+= "        return 1;\n";
            qml+= "    }\n";
            qml+= "    function stress() {\n";
            qml+= dataStr;
            qml+= "    }\n";
            qml+= "}\n";

    QByteArray qmlData = qml.toUtf8();

    QDeclarativeComponent component(&engine);
    
    component.setData(qmlData, QUrl::fromLocalFile(SRCDIR + QString("/dummy.qml")));

    QFileInfo info(file);

    if (info.fileName() == QLatin1String("regress-352044-02-n.js")) {
        QVERIFY(component.isError());

        QCOMPARE(component.errors().length(), 2);

        QCOMPARE(component.errors().at(0).description(), QString("Expected token `;'"));
        QCOMPARE(component.errors().at(0).line(), 66);

        QCOMPARE(component.errors().at(1).description(), QString("Expected token `;'"));
        QCOMPARE(component.errors().at(1).line(), 142);

    } else {

        QVERIFY(!component.isError());
    }
}


QTEST_MAIN(tst_parserstress)

#include "tst_parserstress.moc"

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
#include <QDir>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_moduleqt47 : public QObject
{
    Q_OBJECT
public:
    tst_moduleqt47();

private slots:
    void create();

    void accidentalImport_data();
    void accidentalImport();

private:
    QStringList findFiles(const QDir &d);

    QDeclarativeEngine engine;
    QStringList excludedFiles;
};

tst_moduleqt47::tst_moduleqt47()
{
    excludedFiles << "tests/auto/declarative/moduleqt47/data/importqt47.qml"
                  << "doc/src/declarative/whatsnew.qdoc";
}

void tst_moduleqt47::create()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/importqt47.qml"));
    QObject *obj = qobject_cast<QObject*>(c.create());

    QVERIFY(obj != 0);
    delete obj;
}

QStringList tst_moduleqt47::findFiles(const QDir &d)
{
    QStringList rv;

    QStringList files = d.entryList(QStringList() << QLatin1String("*.qml") << QLatin1String("*.qdoc"), QDir::Files);
    foreach (const QString &file, files) {

        QString absFile = d.absoluteFilePath(file);

        bool skip = false;
        for (int ii = 0; !skip && ii < excludedFiles.count(); ++ii) 
            skip = (absFile.endsWith(excludedFiles.at(ii)));

        if (!skip)
            rv << absFile;
    }

    QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach (const QString &dir, dirs) {
        QDir sub = d;
        sub.cd(dir);
        rv << findFiles(sub);
    }

    return rv;
}

void tst_moduleqt47::accidentalImport_data()
{
    QTest::addColumn<QString>("file");
    QStringList files = findFiles(QDir(SRCDIR "/../../../../"));

    foreach(const QString &file, files) 
        QTest::newRow(qPrintable(file)) << file;
}

void tst_moduleqt47::accidentalImport()
{
    QFETCH(QString, file);

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
        return;
    QByteArray data = f.readAll();

    QVERIFY(!data.contains("import Qt 4"));
}

QTEST_MAIN(tst_moduleqt47)

#include "tst_moduleqt47.moc"

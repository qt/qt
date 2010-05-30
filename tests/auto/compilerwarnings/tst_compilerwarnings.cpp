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


#include <qcoreapplication.h>
#include <qprocess.h>
#include <qtemporaryfile.h>
#include <qdebug.h>

#include <QtTest/QtTest>

#ifdef QT_NO_PROCESS
QTEST_NOOP_MAIN
#else

#include <stdlib.h>

QT_USE_NAMESPACE

class tst_CompilerWarnings: public QObject
{
    Q_OBJECT

private slots:
    void warnings_data();
    void warnings();

private:
    bool shouldIgnoreWarning(QString const&);
};

#if 0
/*
    Return list of all documented qfeatures (QT_NO_*)
 */
static QStringList getFeatures()
{
    QStringList srcDirs;
    srcDirs << QString::fromLocal8Bit(qgetenv("QTDIR"))
            << QString::fromLocal8Bit(qgetenv("QTSRCDIR"));

    QString featurefile;
    foreach (QString dir, srcDirs) {
        QString str = dir + "/src/corelib/global/qfeatures.txt";
        if (QFile::exists(str)) {
            featurefile = str;
            break;
        }
    }

    if (featurefile.isEmpty()) {
        qWarning("Unable to find qfeatures.txt");
        return QStringList();
    }

    QFile file(featurefile);
    if (!file.open(QIODevice::ReadOnly)) {
	qWarning("Unable to open feature file '%s'", qPrintable(featurefile));
	return QStringList();
    }

    QStringList features;
    QTextStream s(&file);
    QRegExp regexp("Feature:\\s+(\\w+)\\s*");
    for (QString line = s.readLine(); !s.atEnd(); line = s.readLine()) {
        if (regexp.exactMatch(line))
            features << regexp.cap(1);
    }

    return features;
}
#endif

void tst_CompilerWarnings::warnings_data()
{
    QTest::addColumn<QStringList>("cflags");

    QTest::newRow("standard") << QStringList();

#if 0
#ifdef Q_WS_QWS
    QStringList features = getFeatures();
    foreach (QString feature, features) {
        QStringList args;
        QString macro = QString("QT_NO_%1").arg(feature);
        args << (QString("-D%1").arg(macro));
        QTest::newRow(qPrintable(macro)) << args;
    }
#endif
#endif
}

void tst_CompilerWarnings::warnings()
{
    QFETCH(QStringList, cflags);

#if !defined(Q_CC_INTEL) && defined(Q_CC_GNU) && __GNUC__ == 3
    QSKIP("gcc 3.x outputs too many bogus warnings", SkipAll);
#endif

    static QString tmpFile;
    if (tmpFile.isEmpty()) {
        QTemporaryFile tmpQFile;
        tmpQFile.open();
        tmpFile = tmpQFile.fileName();
        tmpQFile.close();
    }
    static QString tmpSourceFile;
    bool openResult = true;
    const QString tmpBaseName("XXXXXX-test.cpp");
    QString templatePath = QDir::temp().absoluteFilePath(tmpBaseName);
    QFile tmpQSourceFile(templatePath);
    if (tmpSourceFile.isEmpty()) {
        tmpQSourceFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
        tmpSourceFile = tmpQSourceFile.fileName();
        QFile cppSource(":/test_cpp.txt");
        bool openResult = cppSource.open(QIODevice::ReadOnly);
        if (openResult)
        {
            QTextStream in(&cppSource);
            QTextStream out(&tmpQSourceFile);
            out << in.readAll();
        }
    }
    tmpQSourceFile.close();
    QVERIFY2(openResult, "Need resource temporary \"test_cpp.txt\"");

    QStringList args;
    QString compilerName;

    static QString qtDir = QString::fromLocal8Bit(qgetenv("QTDIR"));
    QVERIFY2(!qtDir.isEmpty(), "This test needs $QTDIR");

    args << cflags;
#if !defined(Q_CC_INTEL) && defined(Q_CC_GNU)
    compilerName = "g++";
    args << "-I" + qtDir + "/include";
    args << "-I/usr/X11R6/include/";
#ifdef Q_OS_HPUX
    args << "-I/usr/local/mesa/aCC-64/include";
#endif
    args << "-c";
    args << "-Wall" << "-Wold-style-cast" << "-Woverloaded-virtual" << "-pedantic" << "-ansi"
         << "-Wno-long-long" << "-Wshadow" << "-Wpacked" << "-Wunreachable-code"
         << "-Wundef" << "-Wchar-subscripts" << "-Wformat-nonliteral" << "-Wformat-security"
         << "-Wcast-align"
         << "-o" << tmpFile
         << tmpSourceFile;
#elif defined(Q_CC_XLC)
    compilerName = "xlC_r";
    args << "-I" + qtDir + "/include"
# if QT_POINTER_SIZE == 8
         << "-q64"
# endif
         << "-c" << "-o" << tmpFile
         << "-info=all"
         << tmpSourceFile;
#elif defined(Q_CC_MSVC)
    compilerName = "cl";
    args << "-I" + qtDir + "/include"
         << "-nologo" << "-W3"
         << tmpSourceFile;
#elif defined (Q_CC_SUN)
    compilerName = "CC";
    // +w or +w2 outputs too much bogus
    args << "-I" + qtDir + "/include"
# if QT_POINTER_SIZE == 8
         << "-xarch=v9"
# endif
         << "-o" << tmpFile
         << tmpSourceFile;
#elif defined (Q_CC_HPACC)
    compilerName = "aCC";
    args << "-I" + qtDir + "/include"
         << "-I/usr/local/mesa/aCC-64/include"
         << "-I/opt/graphics/OpenGL/include"
# if QT_POINTER_SIZE == 8 && !defined __ia64
         << "+DA2.0W"
# endif
         // aCC generates too much bogus.
         << "-DQT_NO_STL" << "-c" << "-w"
         << "-o" << tmpFile
         << tmpSourceFile;
#elif defined(Q_CC_MIPS)
    compilerName = "CC";
    args << "-I" + qtDir + "/include"
         << "-c"
         << "-woff" << "3303" // const qualifier on return
         << "-o" << tmpFile
         << tmpSourceFile;
#else
    QSKIP("Test not implemented for this compiler", SkipAll);
#endif

    QProcess proc;
    proc.start(compilerName, args, QIODevice::ReadOnly);
    QVERIFY2(proc.waitForFinished(6000000), proc.errorString().toLocal8Bit());

#ifdef Q_CC_MSVC
    QString errs = QString::fromLocal8Bit(proc.readAllStandardOutput().constData());
    if (errs.startsWith(tmpBaseName))
        errs = errs.mid(tmpBaseName.size()).simplified();;
#else
    QString errs = QString::fromLocal8Bit(proc.readAllStandardError().constData());
#endif
    QStringList errList;
    if (!errs.isEmpty()) {
        errList = errs.split("\n");
        qDebug() << "Arguments:" << args;
        QStringList validErrors;
        foreach (QString const& err, errList) {
            bool ignore = shouldIgnoreWarning(err);
            qDebug() << err << (ignore ? " [ignored]" : "");
            if (!ignore) {
                validErrors << err;
            }
        }
        errList = validErrors;
    }
    QCOMPARE(errList.count(), 0); // verbose info how many lines of errors in output

    tmpQSourceFile.remove();
}

bool tst_CompilerWarnings::shouldIgnoreWarning(QString const& warning)
{
    if (warning.isEmpty()) {
        return true;
    }

    // icecc outputs warnings if some icecc node breaks
    if (warning.startsWith("ICECC[")) {
        return true;
    }

    // Add more bogus warnings here

    return false;
}

QTEST_APPLESS_MAIN(tst_CompilerWarnings)

#include "tst_compilerwarnings.moc"
#endif // QT_NO_PROCESS

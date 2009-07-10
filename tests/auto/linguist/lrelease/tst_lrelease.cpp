#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QByteArray>

#include <QtTest/QtTest>

class tst_lrelease : public QObject
{
    Q_OBJECT
private:

private slots:
    void translate();
    void mixedcodecs();
    void compressed();
    void dupes();

private:
    void doCompare(const QStringList &actual, const QString &expectedFn);
};

void tst_lrelease::doCompare(const QStringList &actual, const QString &expectedFn)
{
    QFile file(expectedFn);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QStringList expected = QString(file.readAll()).trimmed().remove('\r').split('\n');

    int i = 0, ei = expected.size(), gi = actual.size();
    for (; ; i++) {
        if (i == gi) {
            if (i == ei)
                return;
            gi = 0;
            break;
        } else if (i == ei) {
            ei = 0;
            break;
        } else if (!QRegExp(expected.at(i)).exactMatch(actual.at(i))) {
            while ((ei - 1) >= i && (gi - 1) >= i &&
                     (QRegExp(expected.at(ei - 1)).exactMatch(actual.at(gi - 1))))
                ei--, gi--;
            break;
        }
    }
    QByteArray diff;
    for (int j = qMax(0, i - 3); j < i; j++)
        diff += expected.at(j) + '\n';
    diff += "<<<<<<< got\n";
    for (int j = i; j < gi; j++) {
        diff += actual.at(j) + '\n';
        if (j >= i + 5) {
            diff += "...\n";
            break;
        }
    }
    diff += "=========\n";
    for (int j = i; j < ei; j++) {
        diff += expected.at(j) + '\n';
        if (j >= i + 5) {
            diff += "...\n";
            break;
        }
    }
    diff += ">>>>>>> expected\n";
    for (int j = ei; j < qMin(ei + 3, expected.size()); j++)
        diff += expected.at(j) + '\n';
    QFAIL(qPrintable("Output for " + expectedFn + " does not meet expectations:\n" + diff));
}

void tst_lrelease::translate()
{
    QVERIFY(!QProcess::execute("lrelease testdata/translate.ts"));

    QTranslator translator;
    QVERIFY(translator.load("testdata/translate.qm"));
    qApp->installTranslator(&translator);

    QCOMPARE(QObject::tr("\nnewline at the start"), QString("\nNEWLINE AT THE START"));
    QCOMPARE(QObject::tr("newline at the end\n"),   QString("NEWLINE AT THE END\n"));
    QCOMPARE(QObject::tr("newline and space at the end\n "),   QString("NEWLINE AND SPACE AT THE END\n "));
    QCOMPARE(QObject::tr("space and newline at the end \n"),   QString("SPACE AND NEWLINE AT THE END \n"));
    QCOMPARE(QObject::tr("\ttab at the start and newline at the end\n"),   QString("\tTAB AT THE START AND NEWLINE AT THE END\n"));
    QCOMPARE(QObject::tr("\n\tnewline and tab at the start"), QString("\n\tNEWLINE AND TAB AT THE START"));
    QCOMPARE(QObject::tr(" \tspace and tab at the start"), QString(" \tSPACE AND TAB AT THE START"));
    QCOMPARE(QObject::tr(" string that does not exist"), QString(" string that does not exist"));

    QCOMPARE(QCoreApplication::translate("CubeForm", "Test"), QString::fromAscii("BBBB"));
    QCOMPARE(QCoreApplication::translate("", "Test", "Empty context"), QString("AAAA"));

    // Test plurals
    QString txed = QCoreApplication::translate("Plurals", "There are %n houses", 0, QCoreApplication::UnicodeUTF8, 0);
    QCOMPARE(QString::fromAscii("[%1]").arg(txed), QString("[There are 0 houses]"));
    QCOMPARE(QCoreApplication::translate("Plurals", "There are %n houses", 0, QCoreApplication::UnicodeUTF8, 1), QString("There is 1 house"));
    QCOMPARE(QCoreApplication::translate("Plurals", "There are %n houses", 0, QCoreApplication::UnicodeUTF8, 2), QString("There are 2 houses"));
    QCOMPARE(QCoreApplication::translate("Plurals", "There are %n houses", 0, QCoreApplication::UnicodeUTF8, 3), QString("There are 3 houses"));


    // More plurals
    QCOMPARE(tr("There are %n cars", "More Plurals", 0) , QString("There are 0 cars"));
    QCOMPARE(tr("There are %n cars", "More Plurals", 1) , QString("There is 1 car"));
    QCOMPARE(tr("There are %n cars", "More Plurals", 2) , QString("There are 2 cars"));
    QCOMPARE(tr("There are %n cars", "More Plurals", 3) , QString("There are 3 cars"));


    QCOMPARE(QCoreApplication::translate("no_en", "Kj\370r K\345re, kj\346re"), QString::fromAscii("Drive K\345re, dear"));
    QCOMPARE(QCoreApplication::translate("en_no", "Drive K\345re, dear"), QString::fromAscii("Kj\370r K\345re, kj\346re"));
    QCOMPARE(QCoreApplication::translate("en_ch", "Chinese symbol:"), QString::fromAscii("Chinese symbol:%1").arg(QChar(0x7c1f)));

//    printf("halo\r\nhallo");
  //  QCOMPARE(tr("This\r\nwill fail"), QString("THIS\nWILL FAIL"));    // \r\n =  0d 0a

    QCOMPARE(tr("Completely random string"),
             QString::fromLatin1("Super-lange Uebersetzung mit Schikanen\x9c"
                                 "Mittlere Uebersetung\x9c"
                                 "Kurze Uebers."));

    qApp->removeTranslator(&translator);
}

void tst_lrelease::mixedcodecs()
{
    QVERIFY(!QProcess::execute("lrelease testdata/mixedcodecs-ts11.ts"));
    QVERIFY(!QProcess::execute("lrelease testdata/mixedcodecs-ts20.ts"));
    QVERIFY(!QProcess::execute("cmp testdata/mixedcodecs-ts11.qm testdata/mixedcodecs-ts20.qm"));
    QTranslator translator;
    QVERIFY(translator.load("testdata/mixedcodecs-ts11.qm"));
    qApp->installTranslator(&translator);

    QCOMPARE(QCoreApplication::translate("FooBar", "this contains an umlaut \xfc &uuml;"),
             QString::fromAscii("random stuff with umlaut"));
    QCOMPARE(QCoreApplication::translate("FooBar", "umlaut \xc3\xbc &uuml; in utf8"),
             QString::fromAscii("more random stuff with umlaut"));
}

void tst_lrelease::compressed()
{
    QVERIFY(!QProcess::execute("lrelease -compress testdata/compressed.ts"));

    QTranslator translator;
    QVERIFY(translator.load("testdata/compressed.qm"));
    qApp->installTranslator(&translator);

    QCOMPARE(QCoreApplication::translate("Context1", "Foo"), QString::fromAscii("in first context"));
    QCOMPARE(QCoreApplication::translate("Context2", "Bar"), QString::fromAscii("in second context"));

    QCOMPARE(QCoreApplication::translate("Action1", "Component Name"), QString::fromAscii("translation in first context"));
    QCOMPARE(QCoreApplication::translate("Action2", "Component Name"), QString::fromAscii("translation in second context"));
    QCOMPARE(QCoreApplication::translate("Action3", "Component Name"), QString::fromAscii("translation in third context"));

}

void tst_lrelease::dupes()
{
    QProcess proc;
    proc.start("lrelease testdata/dupes.ts");
    QVERIFY(proc.waitForFinished());
    QVERIFY(proc.exitStatus() == QProcess::NormalExit);
    doCompare(QString(proc.readAllStandardError()).trimmed().remove('\r').split('\n'), "testdata/dupes.errors");
}

QTEST_MAIN(tst_lrelease)
#include "tst_lrelease.moc"

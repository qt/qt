#include <qtest.h>
#include <QDebug>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qnumberformat.h>
#include <QtDeclarative/qmlnumberformatter.h>

class tst_numberformat : public QObject
{
    Q_OBJECT
public:
    tst_numberformat();
   
    void init() {}
    void initTestCase() {}

    void cleanup() {}
    void cleanupTestCase() {}

private slots:
    void number_data();
    void number();

    void text_data();
    void text();

private:
    QStringList strings;
    QList<float> numbers;
    QStringList formats;
    QStringList texts;
};

tst_numberformat::tst_numberformat()
{
    strings << "100.0"
            << "12345"
            << "1234567"
            << "0.123"
            << "0.9999"
            << "0.989"
            << "1"
            << "1.0"
            << "1.01";

    numbers << 100
            << 12345
            << 1234567
            << 0.123
            << 0.9999
            << 0.989
            << 1
            << 1.0
            << 1.01;

    formats << ""
            << "0000"
            << "0000.00"
            << "##"
            << "##.##"
            << "#0.00#"
            << "##,##0.##"
            << "(000) 000 - 000"
            << "00000,000.0000";

    //US locale only.
    texts << "100.000000"
          << "12345.000000"
          << "1234567.000000"
          << "0.123000"
          << "0.999900"
          << "0.989000"
          << "1.000000"
          << "1.000000"
          << "1.010000" //end ""
          << "0100"
          << "12345"
          << "1234567"
          << "0000"
          << "0001"
          << "0001"
          << "0001"
          << "0001"
          << "0001" // end "0000"
          << "0100.00"
          << "12345.00"
          << "1234567.00"
          << "0000.12"
          << "0001.00"
          << "0000.99"
          << "0001.00"
          << "0001.00"
          << "0001.01" // end "0000.00"
          << "100"
          << "12345"
          << "1234567"
          << "0"
          << "1"
          << "1"
          << "1"
          << "1"
          << "1" // end "##"
          << "100"//start "##.##"
          << "12345"
          << "1234567"
          << "0.12"
          << "1"
          << "0.99"
          << "1"
          << "1"
          << "1.01" // end "##.##" -- ### EXPECT FAIL ### QNumberFormat::formatDecimal() bug
          << "100.00" //start "#0.00#"
          << "12345.00"
          << "1234567.00"
          << "0.123"
          << "1.00"
          << "0.989"
          << "1.00"
          << "1.00"
          << "1.01" //end "#0.00#"
          << "100" //start "##,##0.##"
          << "12,345"
          << "1,234,567"
          << "0.12"
          << "1"
          << "0.99"
          << "1"
          << "1"
          << "1.01" //end "##,##0.##" -- ### EXPECT FAIL ### QNumberFormat::formatDecimal() bug
          << "(000) 000 - 100" //start "(000) 000 - 000"
          << "(000) 012 - 345"
          << "(001) 234 - 567"
          << "(000) 000 - 000"
          << "(000) 000 - 001"
          << "(000) 000 - 001"
          << "(000) 000 - 001"
          << "(000) 000 - 001"
          << "(000) 000 - 001" // end "(000) 000 - 000"
          << "00,000,100.0000" // start "00000,000.0000"
          << "00,012,345.0000" 
          << "01,234,567.0000"
          << "00,000,000.1230"
          << "00,000,000.9999"
          << "00,000,000.9890"
          << "00,000,001.0000"
          << "00,000,001.0000"
          << "00,000,001.0100"; // end 

    qDebug() << "strings.size()" << strings.size()
             << "\nformats.size()" << formats.size()
             << "texts.size()" << texts.size();
}

void tst_numberformat::number_data()
{
    QTest::addColumn<QString>("string");    
    QTest::addColumn<float>("number");

    for (int i = 0; i < strings.size(); i++)
        QTest::newRow(QString::number(i).toAscii()) << strings.at(i) << numbers.at(i);
}

void tst_numberformat::number()
{
    // ### tests the conversion from string to float
    QFETCH(QString, string);
    QFETCH(float, number);

    QString componentStr = QString("NumberFormatter { number: \"") + string + QString("\" }");

    QmlEngine engine;
    QmlComponent formatterComponent(&engine, componentStr.toAscii());
    QmlNumberFormatter *formatter = qobject_cast<QmlNumberFormatter*>(formatterComponent.create());
    QVERIFY(formatter != 0);
    QCOMPARE((float)formatter->number(), number);
    //qDebug() << formatter->format() << formatter->text();
    QVERIFY(formatter->format().isEmpty());
    QVERIFY(formatter->text() == QString("%1").arg(number, -1, 'f', -1));
}

void tst_numberformat::text_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("text");

    for (int j=0; j < formats.size(); j++)
    {
        for (int i=0; i < strings.size(); i++)
        {
            QTest::newRow(QString("%1, %2").arg(strings.at(i)).arg(formats.at(j)).toAscii())
                << strings.at(i) << formats.at(j) << texts.at(j*formats.size()+i);
        }
    }

}

void tst_numberformat::text()
{
    QFETCH(QString, string);
    QFETCH(QString, format);
    QFETCH(QString, text);

    QString componentStr = QString("NumberFormatter { number: \"") + string + QString("\"; format: \"") + format + QString("\" }");

    QmlEngine engine;
    QmlComponent formatterComponent(&engine, componentStr.toAscii());
    QmlNumberFormatter *formatter = qobject_cast<QmlNumberFormatter*>(formatterComponent.create());
    QVERIFY(formatter != 0);

    QCOMPARE(formatter->format(), format);
    QCOMPARE(formatter->text(), text);
}

QTEST_MAIN(tst_numberformat)

#include "tst_numberformatter.moc"

#include <QTest>
#include <QtXml>
#include <QmlComponent>

class tst_qmlxmlparser : public QObject
{
    Q_OBJECT

public:
    enum XmlParser { ParserQXmlStreamReader = 0, ParserQmlComponent };

private slots:
    void parse();
    void parse_data();

private:
    QString readFile(QString const&);
};

/*
    Benchmark the parsing of Qml, using QXmlStreamReader as a baseline.
    This gives an idea of the overhead Qml imposes over the top of plain XML parsing.
*/
void tst_qmlxmlparser::parse()
{
    QFETCH(QString, xml);
    QFETCH(int,     parser);

    if (parser == ParserQXmlStreamReader) {
        QBENCHMARK {
            QXmlStreamReader parser(xml);
            while (!parser.atEnd()) parser.readNext();
        }
    }
    else if (parser == ParserQmlComponent) {
        QByteArray ba = xml.toLatin1();
        QBENCHMARK {
            QmlComponent qmlc(ba);
        }
    }
}

void tst_qmlxmlparser::parse_data()
{
    QTest::addColumn<QString>("xml");
    QTest::addColumn<int>    ("parser");

    QTest::newRow("empty-qxml")
        << QString()
        << (int)ParserQXmlStreamReader;

    QTest::newRow("empty-qml")
        << QString()
        << (int)ParserQmlComponent;

    {
        QString xml = QString::fromLatin1("<Item prop1=\"foo\"><Prop2 value=\"bar\"/></Item>");

        QTest::newRow("simple-qxml")
            << xml
            << (int)ParserQXmlStreamReader;

        QTest::newRow("simple-qml")
            << xml
            << (int)ParserQmlComponent;
    }

    {
        QString xml = readFile("concept2.xml");

        QTest::newRow("concept2-qxml")
            << xml
            << (int)ParserQXmlStreamReader;

        QTest::newRow("concept2-qml")
            << xml
            << (int)ParserQmlComponent;
    }
}

QString tst_qmlxmlparser::readFile(QString const& filename)
{
#define _STR(X) #X
#define STR(X) _STR(X)
    QFile file(QString("%1/testdata/%2").arg(STR(SRCDIR)).arg(filename));
    if (!file.open(QIODevice::ReadOnly)) {
        qFatal("Could not open %s: %s", qPrintable(filename), qPrintable(file.errorString()));
    }
    QByteArray ba = file.readAll();
    return QString::fromLatin1(ba);
}


QTEST_MAIN(tst_qmlxmlparser)

#include "main.moc"

#include <qtest.h>
#include <QTextDocument>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxtext.h>
#include <QFontMetrics>

class tst_qfxtext : public QObject

{
    Q_OBJECT
public:
    tst_qfxtext();

private slots:
    void text();
    void width();
    void wrap();
    void elide();

    // ### these tests may be trivial    
    void hAlign();
    void vAlign();
    void font();
    void style();
    void color();

private:
    QStringList standard;
    QStringList richText;

    QStringList hAlignmentStrings;
    QStringList vAlignmentStrings;

    QList<Qt::Alignment> vAlignments;
    QList<Qt::Alignment> hAlignments;

    QStringList styleStrings;
    QList<QFxText::TextStyle> styles;

    QStringList colorStrings;

    QmlEngine engine;
};

tst_qfxtext::tst_qfxtext()
{
    standard << "the quick brown fox jumped over the lazy dog"
             << "the quick brown fox\n jumped over the lazy dog";

    richText << "<i>the <b>quick</b> brown <a href=\"http://www.google.com\">fox</a> jumped over the <b>lazy</b> dog</i>"
             << "<i>the <b>quick</b> brown <a href=\"http://www.google.com\">fox</a><br>jumped over the <b>lazy</b> dog</i>";

    hAlignmentStrings << "AlignLeft"
                      << "AlignRight"
                      << "AlignHCenter";

    vAlignmentStrings << "AlignTop"
                      << "AlignBottom"
                      << "AlignVCenter";

    hAlignments << Qt::AlignLeft
                << Qt::AlignRight
                << Qt::AlignHCenter;

    vAlignments << Qt::AlignTop
                << Qt::AlignBottom
                << Qt::AlignVCenter;

    styleStrings << "Normal"
                 << "Outline"
                 << "Raised"
                 << "Sunken";

    styles << QFxText::Normal
           << QFxText::Outline
           << QFxText::Raised
           << QFxText::Sunken;

    colorStrings << "aliceblue"
                 << "antiquewhite"
                 << "aqua"
                 << "darkkhaki"
                 << "darkolivegreen"
                 << "dimgray"
                 << "palevioletred"
                 << "lightsteelblue"
                 << "#000000"
                 << "#AAAAAA"
                 << "#FFFFFF"
                 << "#2AC05F";
                 //
                 // need a different test to do alpha channel test
                 // << "#AA0011DD"
                 // << "#00F16B11";
                 // 
}

void tst_qfxtext::text()
{
    {
        QmlComponent textComponent(&engine, "<Text text=\"\" />");
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->text(), QString(""));
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "<Text>" + standard.at(i) + "</Text>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->text(), standard.at(i));
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "<Text><![CDATA[" + richText.at(i) + "]]></Text>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->text(), richText.at(i));
    }
}

void tst_qfxtext::width()
{
    // uses Font metrics to find the width for standard and document to find the width for rich
    {
        QmlComponent textComponent(&engine, "<Text text=\"\"/>");
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->width(), 0);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetrics fm(f);
        int metricWidth = fm.size(Qt::TextExpandTabs && Qt::TextShowMnemonic, standard.at(i)).width();

        QString componentStr = "<Text>" + standard.at(i) + "</Text>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->width(), metricWidth);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QTextDocument document;
        document.setHtml(richText.at(i));
        document.setDocumentMargin(0);

        int documentWidth = document.idealWidth();

        QString componentStr = "<Text><![CDATA[" + richText.at(i) + "]]></Text>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->width(), documentWidth);
    }
}

void tst_qfxtext::wrap()
{
    // XXX Poor coverage - should at least be testing an expected height.

    // for specified width and wrap set true
    {
        QmlComponent textComponent(&engine, "<Text text=\"\" wrap=\"true\" width=\"300\"/>");
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->width(), 300);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "<Text wrap=\"true\" width=\"300\">" + standard.at(i) + "</Text>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->width(), 300);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "<Text wrap=\"true\" width=\"300\"><![CDATA[" + richText.at(i) + "]]></Text>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->width(), 300);
    }

}

void tst_qfxtext::elide()
{
    for (Qt::TextElideMode m = Qt::ElideLeft; m<=Qt::ElideNone; m=Qt::TextElideMode(int(m)+1)) {
        const char* elidename[]={"ElideLeft", "ElideRight", "ElideMiddle", "ElideNone"};
        QString elide = "elide=\""+QString(elidename[int(m)])+"\"";

        // XXX Poor coverage.

        {
            QmlComponent textComponent(&engine, ("<Text text=\"\" "+elide+" width=\"300\"/>").toLatin1());
            QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

            QCOMPARE(textObject->width(), 300);
        }

        for (int i = 0; i < standard.size(); i++)
        {
            QString componentStr = "<Text "+elide+" width=\"300\">" + standard.at(i) + "</Text>";
            QmlComponent textComponent(&engine, componentStr.toLatin1());
            QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

            QCOMPARE(textObject->width(), 300);
        }

        // richtext - does nothing
        for (int i = 0; i < richText.size(); i++)
        {
            QString componentStr = "<Text "+elide+" width=\"300\"><![CDATA[" + richText.at(i) + "]]></Text>";
            QmlComponent textComponent(&engine, componentStr.toLatin1());
            QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

            QCOMPARE(textObject->width(), 300);
        }
    }
}

//the alignment tests may be trivial o.oa
void tst_qfxtext::hAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "<Text hAlign=\"" + hAlignmentStrings.at(j) + "\">" + standard.at(i) + "</Text>";
            QmlComponent textComponent(&engine, componentStr.toLatin1());
            QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

            QCOMPARE((int)textObject->hAlign(), (int)hAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "<Text hAlign=\"" + hAlignmentStrings.at(j) + "\"><![CDATA[" + richText.at(i) + "]]></Text>";
            QmlComponent textComponent(&engine, componentStr.toLatin1());
            QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

            QCOMPARE((int)textObject->hAlign(), (int)hAlignments.at(j));
        }
    }

}

void tst_qfxtext::vAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "<Text vAlign=\"" + vAlignmentStrings.at(j) + "\">" + standard.at(i) + "</Text>";
            QmlComponent textComponent(&engine, componentStr.toLatin1());
            QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

            QCOMPARE((int)textObject->vAlign(), (int)vAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "<Text vAlign=\"" + vAlignmentStrings.at(j) + "\"><![CDATA[" + richText.at(i) + "]]></Text>";
            QmlComponent textComponent(&engine, componentStr.toLatin1());
            QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

            QCOMPARE((int)textObject->vAlign(), (int)vAlignments.at(j));
        }
    }

}

void tst_qfxtext::font()
{
    //test size, then bold, then italic, then family
    { 
        QString componentStr = "<Text font.size=\"40\" text=\"Hello World\"/>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->font()->size(), qreal(40));
        QCOMPARE(textObject->font()->bold(), false);
        QCOMPARE(textObject->font()->italic(), false);
    }

    { 
        QString componentStr = "<Text font.bold=\"true\" text=\"Hello World\"/>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->font()->bold(), true);
        QCOMPARE(textObject->font()->italic(), false);
    }

    { 
        QString componentStr = "<Text font.italic=\"true\" text=\"Hello World\"/>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->font()->italic(), true);
        QCOMPARE(textObject->font()->bold(), false);
    }
 
    { 
        QString componentStr = "<Text font.family=\"Helvetica\" text=\"Hello World\"/>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->font()->family(), QString("Helvetica"));
        QCOMPARE(textObject->font()->bold(), false);
        QCOMPARE(textObject->font()->italic(), false);
    }

    { 
        QString componentStr = "<Text font.family=\"\" text=\"Hello World\"/>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->font()->family(), QString(""));
    }
}

void tst_qfxtext::style()
{
    //test style
    for (int i = 0; i < styles.size(); i++)
    { 
        QString componentStr = "<Text style=\"" + styleStrings.at(i) + "\" text=\"Hello World\"/>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE((int)textObject->style(), (int)styles.at(i));
        QCOMPARE(textObject->styleColor(), QColor());
    }
}

void tst_qfxtext::color()
{
    //test style
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "<Text color=\"" + colorStrings.at(i) + "\" text=\"Hello World\"/>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->color(), QColor(colorStrings.at(i)));
        QCOMPARE(textObject->styleColor(), QColor());
    }

    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "<Text styleColor=\"" + colorStrings.at(i) + "\" text=\"Hello World\"/>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->styleColor(), QColor(colorStrings.at(i)));
        // default color to black?
        QCOMPARE(textObject->color(), QColor("black"));
    }
    
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        for (int j = 0; j < colorStrings.size(); j++)
        {
            QString componentStr = "<Text color=\"" + colorStrings.at(i) + "\" styleColor=\"" + colorStrings.at(j) + "\" text=\"Hello World\"/>";
            QmlComponent textComponent(&engine, componentStr.toLatin1());
            QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

            QCOMPARE(textObject->color(), QColor(colorStrings.at(i)));
            QCOMPARE(textObject->styleColor(), QColor(colorStrings.at(j)));
        }
    }
    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "<Text color=\"" + colorStr + "\" text=\"Hello World\"/>";
        QmlComponent textComponent(&engine, componentStr.toLatin1());
        QFxText *textObject = qobject_cast<QFxText*>(textComponent.create());

        QCOMPARE(textObject->color(), testColor);
    }
}
QTEST_MAIN(tst_qfxtext)

#include "tst_qfxtext.moc"

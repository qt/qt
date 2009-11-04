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
#include <QTextDocument>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlgraphicstext_p.h>
#include <QFontMetrics>

class tst_qmlgraphicstext : public QObject

{
    Q_OBJECT
public:
    tst_qmlgraphicstext();

private slots:
    void text();
    void width();
    void wrap();
    void elide();
    void textFormat();

    // ### these tests may be trivial    
    void horizontalAlignment();
    void verticalAlignment();
    void font();
    void style();
    void color();
    void smooth();

private:
    QStringList standard;
    QStringList richText;

    QStringList horizontalAlignmentmentStrings;
    QStringList verticalAlignmentmentStrings;

    QList<Qt::Alignment> verticalAlignmentments;
    QList<Qt::Alignment> horizontalAlignmentments;

    QStringList styleStrings;
    QList<QmlGraphicsText::TextStyle> styles;

    QStringList colorStrings;

    QmlEngine engine;
};

tst_qmlgraphicstext::tst_qmlgraphicstext()
{
    standard << "the quick brown fox jumped over the lazy dog"
             << "the quick brown fox\n jumped over the lazy dog";

    richText << "<i>the <b>quick</b> brown <a href=\\\"http://www.google.com\\\">fox</a> jumped over the <b>lazy</b> dog</i>"
             << "<i>the <b>quick</b> brown <a href=\\\"http://www.google.com\\\">fox</a><br>jumped over the <b>lazy</b> dog</i>";

    horizontalAlignmentmentStrings << "AlignLeft"
                      << "AlignRight"
                      << "AlignHCenter";

    verticalAlignmentmentStrings << "AlignTop"
                      << "AlignBottom"
                      << "AlignVCenter";

    horizontalAlignmentments << Qt::AlignLeft
                << Qt::AlignRight
                << Qt::AlignHCenter;

    verticalAlignmentments << Qt::AlignTop
                << Qt::AlignBottom
                << Qt::AlignVCenter;

    styleStrings << "Normal"
                 << "Outline"
                 << "Raised"
                 << "Sunken";

    styles << QmlGraphicsText::Normal
           << QmlGraphicsText::Outline
           << QmlGraphicsText::Raised
           << QmlGraphicsText::Sunken;

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

void tst_qmlgraphicstext::text()
{
    {
        QmlComponent textComponent(&engine, "import Qt 4.6\nText { text: \"\" }", QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->text(), QString(""));
        QVERIFY(textObject->width() == 0);

        delete textObject;
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nText { text: \"" + standard.at(i) + "\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->text(), standard.at(i));
        QVERIFY(textObject->width() > 0);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nText { text: \"" + richText.at(i) + "\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QString expected = richText.at(i);
        QCOMPARE(textObject->text(), expected.replace("\\\"", "\""));
        QVERIFY(textObject->width() > 0);
    }
}

void tst_qmlgraphicstext::width()
{
    // uses Font metrics to find the width for standard and document to find the width for rich
    {
        QmlComponent textComponent(&engine, "import Qt 4.6\nText { text: \"\" }", QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), 0.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetrics fm(f);
        int metricWidth = fm.size(Qt::TextExpandTabs && Qt::TextShowMnemonic, standard.at(i)).width();

        QString componentStr = "import Qt 4.6\nText { text: \"" + standard.at(i) + "\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), qreal(metricWidth));
        QVERIFY(textObject->textFormat() == QmlGraphicsText::PlainText);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QTextDocument document;
        document.setHtml(richText.at(i));
        document.setDocumentMargin(0);

        int documentWidth = document.idealWidth();

        QString componentStr = "import Qt 4.6\nText { text: \"" + richText.at(i) + "\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), qreal(documentWidth));
        QVERIFY(textObject->textFormat() == QmlGraphicsText::RichText);
    }
}

void tst_qmlgraphicstext::wrap()
{
    int textHeight = 0;
    // for specified width and wrap set true
    {
        QmlComponent textComponent(&engine, "import Qt 4.6\nText { text: \"Hello\"; wrap: true; width: 300 }", QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());
        textHeight = textObject->height();

        QVERIFY(textObject != 0);
        QVERIFY(textObject->wrap() == true);
        QCOMPARE(textObject->width(), 300.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nText { wrap: true; width: 30; text: \"" + standard.at(i) + "\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), 30.);
        QVERIFY(textObject->height() > textHeight);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nText { wrap: true; width: 30; text: \"" + richText.at(i) + "\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), 30.);
        QVERIFY(textObject->height() > textHeight);
    }

}

void tst_qmlgraphicstext::elide()
{
    for (Qt::TextElideMode m = Qt::ElideLeft; m<=Qt::ElideNone; m=Qt::TextElideMode(int(m)+1)) {
        const char* elidename[]={"ElideLeft", "ElideRight", "ElideMiddle", "ElideNone"};
        QString elide = "elide: Text." + QString(elidename[int(m)]) + ";";

        // XXX Poor coverage.

        {
            QmlComponent textComponent(&engine, ("import Qt 4.6\nText { text: \"\"; "+elide+" width: 100 }").toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

            QCOMPARE(textObject->width(), 100.);
        }

        for (int i = 0; i < standard.size(); i++)
        {
            QString componentStr = "import Qt 4.6\nText { "+elide+" width: 100; text: \"" + standard.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

            QCOMPARE(textObject->width(), 100.);
        }

        // richtext - does nothing
        for (int i = 0; i < richText.size(); i++)
        {
            QString componentStr = "import Qt 4.6\nText { "+elide+" width: 100; text: \"" + richText.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

            QCOMPARE(textObject->width(), 100.);
        }
    }
}

void tst_qmlgraphicstext::textFormat()
{
    {
        QmlComponent textComponent(&engine, "import Qt 4.6\nText { text: \"Hello\"; textFormat: Text.RichText }", QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QmlGraphicsText::RichText);
    }
    {
        QmlComponent textComponent(&engine, "import Qt 4.6\nText { text: \"<b>Hello</b>\"; textFormat: Text.PlainText }", QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QmlGraphicsText::PlainText);
    }
}

//the alignment tests may be trivial o.oa
void tst_qmlgraphicstext::horizontalAlignment()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < horizontalAlignmentmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nText { horizontalAlignment: \"" + horizontalAlignmentmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

            QCOMPARE((int)textObject->hAlign(), (int)horizontalAlignmentments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < horizontalAlignmentmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nText { horizontalAlignment: \"" + horizontalAlignmentmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

            QCOMPARE((int)textObject->hAlign(), (int)horizontalAlignmentments.at(j));
        }
    }

}

void tst_qmlgraphicstext::verticalAlignment()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < verticalAlignmentmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nText { verticalAlignment: \"" + verticalAlignmentmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());
            QCOMPARE((int)textObject->vAlign(), (int)verticalAlignmentments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < verticalAlignmentmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nText { verticalAlignment: \"" + verticalAlignmentmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

            QCOMPARE((int)textObject->vAlign(), (int)verticalAlignmentments.at(j));
        }
    }

}

void tst_qmlgraphicstext::font()
{
    //test size, then bold, then italic, then family
    {
        QString componentStr = "import Qt 4.6\nText { font.pointSize: 40; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE(textObject->font().pointSize(), 40);
        QCOMPARE(textObject->font().bold(), false);
        QCOMPARE(textObject->font().italic(), false);
    }

    {
        QString componentStr = "import Qt 4.6\nText { font.pixelSize: 40; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE(textObject->font().pixelSize(), 40);
        QCOMPARE(textObject->font().bold(), false);
        QCOMPARE(textObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nText { font.bold: true; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE(textObject->font().bold(), true);
        QCOMPARE(textObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nText { font.italic: true; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE(textObject->font().italic(), true);
        QCOMPARE(textObject->font().bold(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nText { font.family: \"Helvetica\"; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE(textObject->font().family(), QString("Helvetica"));
        QCOMPARE(textObject->font().bold(), false);
        QCOMPARE(textObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nText { font.family: \"\"; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE(textObject->font().family(), QString(""));
    }
}

void tst_qmlgraphicstext::style()
{
    //test style
    for (int i = 0; i < styles.size(); i++)
    { 
        QString componentStr = "import Qt 4.6\nText { style: \"" + styleStrings.at(i) + "\"; styleColor: \"white\"; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE((int)textObject->style(), (int)styles.at(i));
        QCOMPARE(textObject->styleColor(), QColor("white"));
    }
}

void tst_qmlgraphicstext::color()
{
    //test style
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import Qt 4.6\nText { color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE(textObject->color(), QColor(colorStrings.at(i)));
        QCOMPARE(textObject->styleColor(), QColor());
    }

    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import Qt 4.6\nText { styleColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE(textObject->styleColor(), QColor(colorStrings.at(i)));
        // default color to black?
        QCOMPARE(textObject->color(), QColor("black"));
    }
    
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        for (int j = 0; j < colorStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nText { color: \"" + colorStrings.at(i) + "\"; styleColor: \"" + colorStrings.at(j) + "\"; text: \"Hello World\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

            QCOMPARE(textObject->color(), QColor(colorStrings.at(i)));
            QCOMPARE(textObject->styleColor(), QColor(colorStrings.at(j)));
        }
    }
    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import Qt 4.6\nText { color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
        QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());

        QCOMPARE(textObject->color(), testColor);
    }
}

void tst_qmlgraphicstext::smooth()
{
    for (int i = 0; i < standard.size(); i++)
    {
        {
            QString componentStr = "import Qt 4.6\nText { smooth: true; text: \"" + standard.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());
            QCOMPARE(textObject->smooth(), true);
        }
        {
            QString componentStr = "import Qt 4.6\nText { text: \"" + standard.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());
            QCOMPARE(textObject->smooth(), false);
        }
    }
    for (int i = 0; i < richText.size(); i++)
    {
        {
            QString componentStr = "import Qt 4.6\nText { smooth: true; text: \"" + richText.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());
            QCOMPARE(textObject->smooth(), true);
        }
        {
            QString componentStr = "import Qt 4.6\nText { text: \"" + richText.at(i) + "\" }";
            QmlComponent textComponent(&engine, componentStr.toLatin1(), QUrl("file://"));
            QmlGraphicsText *textObject = qobject_cast<QmlGraphicsText*>(textComponent.create());
            QCOMPARE(textObject->smooth(), false);
        }
    }
}

QTEST_MAIN(tst_qmlgraphicstext)

#include "tst_qmlgraphicstext.moc"

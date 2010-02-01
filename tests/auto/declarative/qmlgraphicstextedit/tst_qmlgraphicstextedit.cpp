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
#include "../../../shared/util.h"
#include "../shared/testhttpserver.h"
#include <math.h>
#include <QFile>
#include <QTextDocument>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlexpression.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlgraphicstextedit_p.h>
#include <QFontMetrics>
#include <QmlView>

class tst_qmlgraphicstextedit : public QObject

{
    Q_OBJECT
public:
    tst_qmlgraphicstextedit();

private slots:
    void text();
    void width();
    void wrap();
    void textFormat();

    // ### these tests may be trivial    
    void hAlign();
    void vAlign();
    void font();
    void color();
    void textMargin();
    void persistentSelection();
    void focusOnPress();
    void selection();

    void cursorDelegate();
    void delegateLoading();
    void navigation();
    void readOnly();

private:
    void simulateKey(QmlView *, int key);
    QmlView *createView(const QString &filename);

    QStringList standard;
    QStringList richText;

    QStringList hAlignmentStrings;
    QStringList vAlignmentStrings;

    QList<Qt::Alignment> vAlignments;
    QList<Qt::Alignment> hAlignments;

    QStringList colorStrings;

    QmlEngine engine;
};

tst_qmlgraphicstextedit::tst_qmlgraphicstextedit()
{
    standard << "the quick brown fox jumped over the lazy dog"
             << "the quick brown fox\n jumped over the lazy dog";

    richText << "<i>the <b>quick</b> brown <a href=\\\"http://www.google.com\\\">fox</a> jumped over the <b>lazy</b> dog</i>"
             << "<i>the <b>quick</b> brown <a href=\\\"http://www.google.com\\\">fox</a><br>jumped over the <b>lazy</b> dog</i>";

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

void tst_qmlgraphicstextedit::text()
{
    {
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData("import Qt 4.6\nTextEdit {  text: \"\"  }", QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), QString(""));
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit { text: \"" + standard.at(i) + "\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), standard.at(i));
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit { text: \"" + richText.at(i) + "\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QString actual = textEditObject->text();
        QString expected = richText.at(i);
        actual.replace(QRegExp(".*<body[^>]*>"),"");
        actual.replace(QRegExp("(<[^>]*>)+"),"<>");
        expected.replace(QRegExp("(<[^>]*>)+"),"<>");
        QCOMPARE(actual.simplified(),expected.simplified());
    }
}

void tst_qmlgraphicstextedit::width()
{
    // uses Font metrics to find the width for standard and document to find the width for rich
    {
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData("import Qt 4.6\nTextEdit {  text: \"\" }", QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 1.);//+1 for cursor
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetricsF fm(f);
        qreal metricWidth = fm.size(Qt::TextExpandTabs && Qt::TextShowMnemonic, standard.at(i)).width();
        metricWidth = floor(metricWidth);

        QString componentStr = "import Qt 4.6\nTextEdit { text: \"" + standard.at(i) + "\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), qreal(metricWidth + 1 + 3));//+3 is the current way of accounting for space between cursor and last character.
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QTextDocument document;
        document.setHtml(richText.at(i));
        document.setDocumentMargin(0);

        int documentWidth = document.idealWidth();

        QString componentStr = "import Qt 4.6\nTextEdit { text: \"" + richText.at(i) + "\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), qreal(documentWidth + 1 + 3));
    }
}

void tst_qmlgraphicstextedit::wrap()
{
    // for specified width and wrap set true
    {
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData("import Qt 4.6\nTextEdit {  text: \"\"; wrap: true; width: 300 }", QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit {  wrap: true; width: 300; text: \"" + standard.at(i) + "\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit {  wrap: true; width: 300; text: \"" + richText.at(i) + "\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

}

void tst_qmlgraphicstextedit::textFormat()
{
    {
        QmlComponent textComponent(&engine);
        textComponent.setData("import Qt 4.6\nTextEdit { text: \"Hello\"; textFormat: Text.RichText }", QUrl("file://"));
        QmlGraphicsTextEdit *textObject = qobject_cast<QmlGraphicsTextEdit*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QmlGraphicsTextEdit::RichText);
    }
    {
        QmlComponent textComponent(&engine);
        textComponent.setData("import Qt 4.6\nTextEdit { text: \"<b>Hello</b>\"; textFormat: Text.PlainText }", QUrl("file://"));
        QmlGraphicsTextEdit *textObject = qobject_cast<QmlGraphicsTextEdit*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QmlGraphicsTextEdit::PlainText);
    }
}

//the alignment tests may be trivial o.oa
void tst_qmlgraphicstextedit::hAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nTextEdit {  horizontalAlignment: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QmlComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->hAlign(), (int)hAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nTextEdit {  horizontalAlignment: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QmlComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->hAlign(), (int)hAlignments.at(j));
        }
    }

}

void tst_qmlgraphicstextedit::vAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nTextEdit {  verticalAlignment: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QmlComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->vAlign(), (int)vAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nTextEdit {  verticalAlignment: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QmlComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->vAlign(), (int)vAlignments.at(j));
        }
    }

}

void tst_qmlgraphicstextedit::font()
{
    //test size, then bold, then italic, then family
    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.pointSize: 40; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().pointSize(), 40);
        QCOMPARE(textEditObject->font().bold(), false);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.bold: true; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().bold(), true);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.italic: true; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().italic(), true);
        QCOMPARE(textEditObject->font().bold(), false);
    }
 
    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.family: \"Helvetica\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().family(), QString("Helvetica"));
        QCOMPARE(textEditObject->font().bold(), false);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.family: \"\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().family(), QString(""));
    }
}

void tst_qmlgraphicstextedit::color()
{
    //test normal
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());
        //qDebug() << "textEditObject: " << textEditObject->color() << "vs. " << QColor(colorStrings.at(i));
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), QColor(colorStrings.at(i)));
    }

    //test selection
    for (int i = 0; i < colorStrings.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit {  selectionColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->selectionColor(), QColor(colorStrings.at(i)));
    }

    //test selected text
    for (int i = 0; i < colorStrings.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit {  selectedTextColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->selectedTextColor(), QColor(colorStrings.at(i)));
    }

    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import Qt 4.6\nTextEdit {  color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), testColor);
    }
}

void tst_qmlgraphicstextedit::textMargin()
{
    for(qreal i=0; i<=10; i+=0.3){
        QString componentStr = "import Qt 4.6\nTextEdit {  textMargin: " + QString::number(i) + "; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->textMargin(), i);
    }
}

void tst_qmlgraphicstextedit::persistentSelection()
{
    {
        QString componentStr = "import Qt 4.6\nTextEdit {  persistentSelection: true; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->persistentSelection(), true);
    }

    {
        QString componentStr = "import Qt 4.6\nTextEdit {  persistentSelection: false; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->persistentSelection(), false);
    }
}

void tst_qmlgraphicstextedit::focusOnPress()
{
    {
        QString componentStr = "import Qt 4.6\nTextEdit {  focusOnPress: true; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->focusOnPress(), true);
    }

    {
        QString componentStr = "import Qt 4.6\nTextEdit {  focusOnPress: false; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->focusOnPress(), false);
    }
}

void tst_qmlgraphicstextedit::selection()
{
    QString testStr = standard[0];//TODO: What should happen for multiline/rich text?
    QString componentStr = "import Qt 4.6\nTextEdit {  text: \""+ testStr +"\"; }";
    QmlComponent texteditComponent(&engine);
    texteditComponent.setData(componentStr.toLatin1(), QUrl());
    QmlGraphicsTextEdit *textEditObject = qobject_cast<QmlGraphicsTextEdit*>(texteditComponent.create());
    QVERIFY(textEditObject != 0);


    //Test selection follows cursor
    for(int i=0; i<= testStr.size(); i++) {
        textEditObject->setCursorPosition(i);
        QCOMPARE(textEditObject->cursorPosition(), i);
        QCOMPARE(textEditObject->selectionStart(), i);
        QCOMPARE(textEditObject->selectionEnd(), i);
        QVERIFY(textEditObject->selectedText().isNull());
    }

    textEditObject->setCursorPosition(0);
    QVERIFY(textEditObject->cursorPosition() == 0);
    QVERIFY(textEditObject->selectionStart() == 0);
    QVERIFY(textEditObject->selectionEnd() == 0);
    QVERIFY(textEditObject->selectedText().isNull());

    //Test selection
    for(int i=0; i<= testStr.size(); i++) {
        textEditObject->setSelectionEnd(i);
        QCOMPARE(testStr.mid(0,i), textEditObject->selectedText());
    }
    for(int i=0; i<= testStr.size(); i++) {
        textEditObject->setSelectionStart(i);
        QCOMPARE(testStr.mid(i,testStr.size()-i), textEditObject->selectedText());
    }

    textEditObject->setCursorPosition(0);
    QVERIFY(textEditObject->cursorPosition() == 0);
    QVERIFY(textEditObject->selectionStart() == 0);
    QVERIFY(textEditObject->selectionEnd() == 0);
    QVERIFY(textEditObject->selectedText().isNull());

    for(int i=0; i< testStr.size(); i++) {
        textEditObject->setSelectionStart(i);
        QCOMPARE(textEditObject->selectionEnd(), i);
        QCOMPARE(testStr.mid(i,0), textEditObject->selectedText());
        textEditObject->setSelectionEnd(i+1);
        QCOMPARE(textEditObject->selectionStart(), i);
        QCOMPARE(testStr.mid(i,1), textEditObject->selectedText());
    }

    for(int i= testStr.size() - 1; i>0; i--) {
        textEditObject->setSelectionEnd(i);
        QCOMPARE(testStr.mid(i,0), textEditObject->selectedText());
        textEditObject->setSelectionStart(i-1);
        QCOMPARE(testStr.mid(i-1,1), textEditObject->selectedText());
    }

    //Test Error Ignoring behaviour
    textEditObject->setCursorPosition(0);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->setSelectionStart(-10);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->setSelectionStart(100);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->setSelectionEnd(-10);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->setSelectionEnd(100);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->setSelectionStart(0);
    textEditObject->setSelectionEnd(10);
    QVERIFY(textEditObject->selectedText().size() == 10);
    textEditObject->setSelectionStart(-10);
    QVERIFY(textEditObject->selectedText().size() == 10);
    textEditObject->setSelectionStart(100);
    QVERIFY(textEditObject->selectedText().size() == 10);
    textEditObject->setSelectionEnd(-10);
    QVERIFY(textEditObject->selectedText().size() == 10);
    textEditObject->setSelectionEnd(100);
    QVERIFY(textEditObject->selectedText().size() == 10);
}

void tst_qmlgraphicstextedit::cursorDelegate()
{
    QmlView* view = createView(SRCDIR "/data/cursorTest.qml");
    view->execute();
    view->show();
    view->setFocus();
    QmlGraphicsTextEdit *textEditObject = view->root()->findChild<QmlGraphicsTextEdit*>("textEditObject");
    QVERIFY(textEditObject != 0);
    QVERIFY(textEditObject->findChild<QmlGraphicsItem*>("cursorInstance"));
    //Test Delegate gets created
    textEditObject->setFocus(true);
    QmlGraphicsItem* delegateObject = textEditObject->findChild<QmlGraphicsItem*>("cursorInstance");
    QVERIFY(delegateObject);
    //Test Delegate gets moved
    for(int i=0; i<= textEditObject->text().length(); i++){
        textEditObject->setCursorPosition(i);
        QCOMPARE(textEditObject->cursorRect().x(), qRound(delegateObject->x()));
        QCOMPARE(textEditObject->cursorRect().y(), qRound(delegateObject->y()));
    }
    textEditObject->setCursorPosition(0);
    QCOMPARE(textEditObject->cursorRect().x(), qRound(delegateObject->x()));
    QCOMPARE(textEditObject->cursorRect().y(), qRound(delegateObject->y()));
    //Test Delegate gets deleted
    textEditObject->setCursorDelegate(0);
    QVERIFY(!textEditObject->findChild<QmlGraphicsItem*>("cursorInstance"));
}

void tst_qmlgraphicstextedit::delegateLoading()
{
    TestHTTPServer server(42332);
    server.serveDirectory("data/httpfail", TestHTTPServer::Disconnect);
    server.serveDirectory("data/httpslow", TestHTTPServer::Delay);
    server.serveDirectory("data/http");
    QmlView* view= new QmlView(0);
    view->setUrl(QUrl("http://localhost:42332/cursorHttpTestPass.qml"));
    view->execute();
    view->show();
    view->setFocus();
    QTest::qWait(500);
    QmlGraphicsTextEdit *textEditObject = view->root()->findChild<QmlGraphicsTextEdit*>("textEditObject");
    QEXPECT_FAIL("","QT-2498", Continue);
    QVERIFY(textEditObject != 0);
    //textEditObject->setFocus(true);
    QmlGraphicsItem *delegate;
    QEXPECT_FAIL("","QT-2498", Continue);
    delegate = view->root()->findChild<QmlGraphicsItem*>("delegateOkay");
    QVERIFY(delegate);
    QEXPECT_FAIL("","QT-2498", Continue);
    delegate = view->root()->findChild<QmlGraphicsItem*>("delegateSlow");
    QVERIFY(delegate);
    view->setUrl(QUrl("http://localhost:42332/cursorHttpTestFail1.qml"));
    view->execute();
    view->show();
    view->setFocus();
    delegate = view->root()->findChild<QmlGraphicsItem*>("delegateOkay");
    QVERIFY(!delegate);
    delegate = view->root()->findChild<QmlGraphicsItem*>("delegateFail");
    QVERIFY(!delegate);
    view->setUrl(QUrl("http://localhost:42332/cursorHttpTestFail2.qml"));
    view->execute();
    view->show();
    view->setFocus();
    delegate = view->root()->findChild<QmlGraphicsItem*>("delegateOkay");
    QVERIFY(!delegate);
    delegate = view->root()->findChild<QmlGraphicsItem*>("delegateErrorA");
    QVERIFY(!delegate);
    //ErrorB should get a component which is ready but component.create() returns null
    //Not sure how to accomplish this with QmlGraphicsTextEdits cursor delegate
    //delegate = view->root()->findChild<QmlGraphicsItem*>("delegateErrorB");
    //QVERIFY(!delegate);
}

/*
TextEdit element should only handle left/right keys until the cursor reaches
the extent of the text, then they should ignore the keys.
*/
void tst_qmlgraphicstextedit::navigation()
{
    QmlView *canvas = createView(SRCDIR "/data/navigation.qml");
    canvas->execute();
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->root() != 0);

    QmlGraphicsItem *input = qobject_cast<QmlGraphicsItem *>(qvariant_cast<QObject *>(canvas->root()->property("myInput")));

    QVERIFY(input != 0);
    QTRY_VERIFY(input->hasFocus() == true);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasFocus() == false);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasFocus() == true);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasFocus() == false);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasFocus() == true);
}

void tst_qmlgraphicstextedit::readOnly()
{
    QmlView *canvas = createView(SRCDIR "/data/readOnly.qml");
    canvas->execute();
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->root() != 0);

    QmlGraphicsTextEdit *edit = qobject_cast<QmlGraphicsTextEdit *>(qvariant_cast<QObject *>(canvas->root()->property("myInput")));

    QVERIFY(edit != 0);
    QTRY_VERIFY(edit->hasFocus() == true);
    QVERIFY(edit->isReadOnly() == true);
    QString initial = edit->text();
    for(int k=Qt::Key_0; k<=Qt::Key_Z; k++)
        simulateKey(canvas, k);
    simulateKey(canvas, Qt::Key_Return);
    simulateKey(canvas, Qt::Key_Space);
    simulateKey(canvas, Qt::Key_Escape);
    QCOMPARE(edit->text(), initial);
}

void tst_qmlgraphicstextedit::simulateKey(QmlView *view, int key)
{
    QKeyEvent press(QKeyEvent::KeyPress, key, 0);
    QKeyEvent release(QKeyEvent::KeyRelease, key, 0);

    QApplication::sendEvent(view, &press);
    QApplication::sendEvent(view, &release);
}

QmlView *tst_qmlgraphicstextedit::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString xml = file.readAll();
    canvas->setQml(xml, filename);

    return canvas;
}


QTEST_MAIN(tst_qmlgraphicstextedit)

#include "tst_qmlgraphicstextedit.moc"

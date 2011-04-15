/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtTest/QSignalSpy>
#include "../../../shared/util.h"
#include "../shared/testhttpserver.h"
#include <math.h>
#include <QFile>
#include <QTextDocument>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qsgtextedit_p.h>
#include <private/qsgtextedit_p_p.h>
#include <QFontMetrics>
#include <QSGView>
#include <QDir>
#include <QStyle>
#include <QInputContext>
#include <QClipboard>
#include <QMimeData>
#include <private/qapplication_p.h>
#include <private/qtextcontrol_p.h>

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

Q_DECLARE_METATYPE(QSGTextEdit::SelectionMode)

QString createExpectedFileIfNotFound(const QString& filebasename, const QImage& actual)
{
    // XXX This will be replaced by some clever persistent platform image store.
    QString persistent_dir = SRCDIR "/data";
    QString arch = "unknown-architecture"; // QTest needs to help with this.

    QString expectfile = persistent_dir + QDir::separator() + filebasename + "-" + arch + ".png";

    if (!QFile::exists(expectfile)) {
        actual.save(expectfile);
        qWarning() << "created" << expectfile;
    }

    return expectfile;
}


class tst_qsgtextedit : public QObject

{
    Q_OBJECT
public:
    tst_qsgtextedit();

private slots:
    void text();
    void width();
    void wrap();
    void textFormat();
    void alignments();
    void alignments_data();

    // ### these tests may be trivial    
    void hAlign();
    void hAlign_RightToLeft();
    void vAlign();
    void font();
    void color();
    void textMargin();
    void persistentSelection();
    void focusOnPress();
    void selection();
    void isRightToLeft_data();
    void isRightToLeft();
    void keySelection();
    void moveCursorSelection_data();
    void moveCursorSelection();
    void moveCursorSelectionSequence_data();
    void moveCursorSelectionSequence();
    void mouseSelection_data();
    void mouseSelection();
    void mouseSelectionMode_data();
    void mouseSelectionMode();
    void dragMouseSelection();
    void inputMethodHints();

    void positionAt();

    void cursorDelegate();
    void cursorVisible();
    void delegateLoading_data();
    void delegateLoading();
    void navigation();
    void readOnly();
    void copyAndPaste();
    void canPaste();
    void canPasteEmpty();
    void textInput();
    void openInputPanelOnClick();
    void openInputPanelOnFocus();
    void geometrySignals();
    void pastingRichText_QTBUG_14003();
    void implicitSize_data();
    void implicitSize();
    void testQtQuick11Attributes();
    void testQtQuick11Attributes_data();

    void preeditMicroFocus();
    void inputContextMouseHandler();
    void inputMethodComposing();
    void cursorRectangleSize();

private:
    void simulateKey(QSGView *, int key, Qt::KeyboardModifiers modifiers = 0);

    QStringList standard;
    QStringList richText;

    QStringList hAlignmentStrings;
    QStringList vAlignmentStrings;

    QList<Qt::Alignment> vAlignments;
    QList<Qt::Alignment> hAlignments;

    QStringList colorStrings;

    QDeclarativeEngine engine;
};

tst_qsgtextedit::tst_qsgtextedit()
{
    standard << "the quick brown fox jumped over the lazy dog"
             << "the quick brown fox\n jumped over the lazy dog"
             << "Hello, world!"
             << "!dlrow ,olleH";

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

void tst_qsgtextedit::text()
{
    {
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData("import QtQuick 2.0\nTextEdit {  text: \"\"  }", QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), QString(""));
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import QtQuick 2.0\nTextEdit { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), standard.at(i));
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import QtQuick 2.0\nTextEdit { text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QString actual = textEditObject->text();
        QString expected = richText.at(i);
        actual.replace(QRegExp(".*<body[^>]*>"),"");
        actual.replace(QRegExp("(<[^>]*>)+"),"<>");
        expected.replace(QRegExp("(<[^>]*>)+"),"<>");
        QCOMPARE(actual.simplified(),expected.simplified());
    }
}

void tst_qsgtextedit::width()
{
    // uses Font metrics to find the width for standard and document to find the width for rich
    {
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData("import QtQuick 2.0\nTextEdit {  text: \"\" }", QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 0.0);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetricsF fm(f);
        qreal metricWidth = fm.size(Qt::TextExpandTabs && Qt::TextShowMnemonic, standard.at(i)).width();
        metricWidth = ceil(metricWidth);

        QString componentStr = "import QtQuick 2.0\nTextEdit { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), qreal(metricWidth));
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QTextDocument document;
        document.setHtml(richText.at(i));
        document.setDocumentMargin(0);

        int documentWidth = ceil(document.idealWidth());

        QString componentStr = "import QtQuick 2.0\nTextEdit { text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), qreal(documentWidth));
    }
}

void tst_qsgtextedit::wrap()
{
    // for specified width and wrap set true
    {
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData("import QtQuick 2.0\nTextEdit {  text: \"\"; wrapMode: TextEdit.WordWrap; width: 300 }", QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import QtQuick 2.0\nTextEdit {  wrapMode: TextEdit.WordWrap; width: 300; text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import QtQuick 2.0\nTextEdit {  wrapMode: TextEdit.WordWrap; width: 300; text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

}

void tst_qsgtextedit::textFormat()
{
    {
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData("import QtQuick 2.0\nTextEdit { text: \"Hello\"; textFormat: Text.RichText }", QUrl::fromLocalFile(""));
        QSGTextEdit *textObject = qobject_cast<QSGTextEdit*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QSGTextEdit::RichText);
    }
    {
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData("import QtQuick 2.0\nTextEdit { text: \"<b>Hello</b>\"; textFormat: Text.PlainText }", QUrl::fromLocalFile(""));
        QSGTextEdit *textObject = qobject_cast<QSGTextEdit*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QSGTextEdit::PlainText);
    }
}

void tst_qsgtextedit::alignments_data()
{
    QTest::addColumn<int>("hAlign");
    QTest::addColumn<int>("vAlign");
    QTest::addColumn<QString>("expectfile");

    QTest::newRow("LT") << int(Qt::AlignLeft) << int(Qt::AlignTop) << "alignments_lt";
    QTest::newRow("RT") << int(Qt::AlignRight) << int(Qt::AlignTop) << "alignments_rt";
    QTest::newRow("CT") << int(Qt::AlignHCenter) << int(Qt::AlignTop) << "alignments_ct";

    QTest::newRow("LB") << int(Qt::AlignLeft) << int(Qt::AlignBottom) << "alignments_lb";
    QTest::newRow("RB") << int(Qt::AlignRight) << int(Qt::AlignBottom) << "alignments_rb";
    QTest::newRow("CB") << int(Qt::AlignHCenter) << int(Qt::AlignBottom) << "alignments_cb";

    QTest::newRow("LC") << int(Qt::AlignLeft) << int(Qt::AlignVCenter) << "alignments_lc";
    QTest::newRow("RC") << int(Qt::AlignRight) << int(Qt::AlignVCenter) << "alignments_rc";
    QTest::newRow("CC") << int(Qt::AlignHCenter) << int(Qt::AlignVCenter) << "alignments_cc";
}


void tst_qsgtextedit::alignments()
{
    QFETCH(int, hAlign);
    QFETCH(int, vAlign);
    QFETCH(QString, expectfile);

    QSGView canvas(QUrl::fromLocalFile(SRCDIR "/data/alignments.qml"));

    canvas.show();
    QApplication::setActiveWindow(&canvas);
    QTest::qWaitForWindowShown(&canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&canvas));

    QObject *ob = canvas.rootObject();
    QVERIFY(ob != 0);
    ob->setProperty("horizontalAlignment",hAlign);
    ob->setProperty("verticalAlignment",vAlign);
    QTRY_COMPARE(ob->property("running").toBool(),false);
    QImage actual(canvas.width(), canvas.height(), QImage::Format_RGB32);
    actual.fill(qRgb(255,255,255));
    QPainter p(&actual);
    canvas.render(&p);

    expectfile = createExpectedFileIfNotFound(expectfile, actual);

    QImage expect(expectfile);

    QCOMPARE(actual,expect);
}


//the alignment tests may be trivial o.oa
void tst_qsgtextedit::hAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 2.0\nTextEdit {  horizontalAlignment: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QDeclarativeComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->hAlign(), (int)hAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 2.0\nTextEdit {  horizontalAlignment: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QDeclarativeComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->hAlign(), (int)hAlignments.at(j));
        }
    }

}

void tst_qsgtextedit::hAlign_RightToLeft()
{
    QSGView canvas(QUrl::fromLocalFile(SRCDIR "/data/horizontalAlignment_RightToLeft.qml"));
    QSGTextEdit *textEdit = canvas.rootObject()->findChild<QSGTextEdit*>("text");
    QVERIFY(textEdit != 0);
    canvas.show();

    // implicit alignment should follow the reading direction of text
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignRight);
    QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);

    // explicitly left aligned
    textEdit->setHAlign(QSGTextEdit::AlignLeft);
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignLeft);
    QVERIFY(textEdit->positionToRectangle(0).x() < canvas.width()/2);

    // explicitly right aligned
    textEdit->setHAlign(QSGTextEdit::AlignRight);
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignRight);
    QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);

    QString textString = textEdit->text();
    textEdit->setText(QString("<i>") + textString + QString("</i>"));
    textEdit->resetHAlign();

    // implicitly aligned rich text should follow the reading direction of RTL text
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignRight);
    QCOMPARE(textEdit->effectiveHAlign(), textEdit->hAlign());
    QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);

    // explicitly left aligned rich text
    textEdit->setHAlign(QSGTextEdit::AlignLeft);
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignLeft);
    QCOMPARE(textEdit->effectiveHAlign(), textEdit->hAlign());
    QVERIFY(textEdit->positionToRectangle(0).x() < canvas.width()/2);

    // explicitly right aligned rich text
    textEdit->setHAlign(QSGTextEdit::AlignRight);
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignRight);
    QCOMPARE(textEdit->effectiveHAlign(), textEdit->hAlign());
    QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);

    textEdit->setText(textString);

    // explicitly center aligned
    textEdit->setHAlign(QSGTextEdit::AlignHCenter);
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignHCenter);
    QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);

    // reseted alignment should go back to following the text reading direction
    textEdit->resetHAlign();
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignRight);
    QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);

    // mirror the text item
    QSGItemPrivate::get(textEdit)->setLayoutMirror(true);

    // mirrored implicit alignment should continue to follow the reading direction of the text
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignRight);
    QCOMPARE(textEdit->effectiveHAlign(), QSGTextEdit::AlignRight);
    QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);

    // mirrored explicitly right aligned behaves as left aligned
    textEdit->setHAlign(QSGTextEdit::AlignRight);
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignRight);
    QCOMPARE(textEdit->effectiveHAlign(), QSGTextEdit::AlignLeft);
    QVERIFY(textEdit->positionToRectangle(0).x() < canvas.width()/2);

    // mirrored explicitly left aligned behaves as right aligned
    textEdit->setHAlign(QSGTextEdit::AlignLeft);
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignLeft);
    QCOMPARE(textEdit->effectiveHAlign(), QSGTextEdit::AlignRight);
    QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);

    // disable mirroring
    QSGItemPrivate::get(textEdit)->setLayoutMirror(false);
    textEdit->resetHAlign();

    // English text should be implicitly left aligned
    textEdit->setText("Hello world!");
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignLeft);
    QVERIFY(textEdit->positionToRectangle(0).x() < canvas.width()/2);

#ifndef Q_OS_MAC    // QTBUG-18040
    // empty text with implicit alignment follows the system locale-based
    // keyboard input direction from QApplication::keyboardInputDirection
    textEdit->setText("");
    QCOMPARE(textEdit->hAlign(), QApplication::keyboardInputDirection() == Qt::LeftToRight ?
                                  QSGTextEdit::AlignLeft : QSGTextEdit::AlignRight);
    if (QApplication::keyboardInputDirection() == Qt::LeftToRight)
        QVERIFY(textEdit->positionToRectangle(0).x() < canvas.width()/2);
    else
        QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);
    textEdit->setHAlign(QSGTextEdit::AlignRight);
    QCOMPARE(textEdit->hAlign(), QSGTextEdit::AlignRight);
    QVERIFY(textEdit->positionToRectangle(0).x() > canvas.width()/2);
#endif

#ifndef Q_OS_MAC    // QTBUG-18040
    // alignment of TextEdit with no text set to it
    QString componentStr = "import QtQuick 2.0\nTextEdit {}";
    QDeclarativeComponent textComponent(&engine);
    textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QSGTextEdit *textObject = qobject_cast<QSGTextEdit*>(textComponent.create());
    QCOMPARE(textObject->hAlign(), QApplication::keyboardInputDirection() == Qt::LeftToRight ?
                                  QSGTextEdit::AlignLeft : QSGTextEdit::AlignRight);
    delete textObject;
#endif
}

void tst_qsgtextedit::vAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 2.0\nTextEdit {  verticalAlignment: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QDeclarativeComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->vAlign(), (int)vAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 2.0\nTextEdit {  verticalAlignment: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QDeclarativeComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->vAlign(), (int)vAlignments.at(j));
        }
    }

}

void tst_qsgtextedit::font()
{
    //test size, then bold, then italic, then family
    { 
        QString componentStr = "import QtQuick 2.0\nTextEdit {  font.pointSize: 40; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().pointSize(), 40);
        QCOMPARE(textEditObject->font().bold(), false);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import QtQuick 2.0\nTextEdit {  font.bold: true; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().bold(), true);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import QtQuick 2.0\nTextEdit {  font.italic: true; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().italic(), true);
        QCOMPARE(textEditObject->font().bold(), false);
    }
 
    { 
        QString componentStr = "import QtQuick 2.0\nTextEdit {  font.family: \"Helvetica\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().family(), QString("Helvetica"));
        QCOMPARE(textEditObject->font().bold(), false);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import QtQuick 2.0\nTextEdit {  font.family: \"\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().family(), QString(""));
    }
}

void tst_qsgtextedit::color()
{
    //test initial color
    {
        QString componentStr = "import QtQuick 2.0\nTextEdit { text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QSGTextEditPrivate *textEditPrivate = static_cast<QSGTextEditPrivate*>(QSGItemPrivate::get(textEditObject));

        QVERIFY(textEditObject);
        QVERIFY(textEditPrivate);
        QVERIFY(textEditPrivate->control);

        QPalette pal = textEditPrivate->control->palette();
        QCOMPARE(textEditPrivate->color, QColor("black"));
        QCOMPARE(textEditPrivate->color, pal.color(QPalette::Text));
    }
    //test normal
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import QtQuick 2.0\nTextEdit {  color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
        //qDebug() << "textEditObject: " << textEditObject->color() << "vs. " << QColor(colorStrings.at(i));
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), QColor(colorStrings.at(i)));
    }

    //test selection
    for (int i = 0; i < colorStrings.size(); i++)
    {
        QString componentStr = "import QtQuick 2.0\nTextEdit {  selectionColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->selectionColor(), QColor(colorStrings.at(i)));
    }

    //test selected text
    for (int i = 0; i < colorStrings.size(); i++)
    {
        QString componentStr = "import QtQuick 2.0\nTextEdit {  selectedTextColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->selectedTextColor(), QColor(colorStrings.at(i)));
    }

    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import QtQuick 2.0\nTextEdit {  color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), testColor);
    }
}

void tst_qsgtextedit::textMargin()
{
    for(qreal i=0; i<=10; i+=0.3){
        QString componentStr = "import QtQuick 2.0\nTextEdit {  textMargin: " + QString::number(i) + "; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->textMargin(), i);
    }
}

void tst_qsgtextedit::persistentSelection()
{
    {
        QString componentStr = "import QtQuick 2.0\nTextEdit {  persistentSelection: true; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->persistentSelection(), true);
    }

    {
        QString componentStr = "import QtQuick 2.0\nTextEdit {  persistentSelection: false; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->persistentSelection(), false);
    }
}

void tst_qsgtextedit::focusOnPress()
{
    {
        QString componentStr = "import QtQuick 2.0\nTextEdit {  activeFocusOnPress: true; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->focusOnPress(), true);
    }

    {
        QString componentStr = "import QtQuick 2.0\nTextEdit {  activeFocusOnPress: false; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->focusOnPress(), false);
    }
}

void tst_qsgtextedit::selection()
{
    QString testStr = standard[0];//TODO: What should happen for multiline/rich text?
    QString componentStr = "import QtQuick 2.0\nTextEdit {  text: \""+ testStr +"\"; }";
    QDeclarativeComponent texteditComponent(&engine);
    texteditComponent.setData(componentStr.toLatin1(), QUrl());
    QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
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

    // Verify invalid positions are ignored.
    textEditObject->setCursorPosition(-1);
    QVERIFY(textEditObject->cursorPosition() == 0);
    QVERIFY(textEditObject->selectionStart() == 0);
    QVERIFY(textEditObject->selectionEnd() == 0);
    QVERIFY(textEditObject->selectedText().isNull());

    textEditObject->setCursorPosition(textEditObject->text().count()+1);
    QVERIFY(textEditObject->cursorPosition() == 0);
    QVERIFY(textEditObject->selectionStart() == 0);
    QVERIFY(textEditObject->selectionEnd() == 0);
    QVERIFY(textEditObject->selectedText().isNull());

    //Test selection
    for(int i=0; i<= testStr.size(); i++) {
        textEditObject->select(0,i);
        QCOMPARE(testStr.mid(0,i), textEditObject->selectedText());
    }
    for(int i=0; i<= testStr.size(); i++) {
        textEditObject->select(i,testStr.size());
        QCOMPARE(testStr.mid(i,testStr.size()-i), textEditObject->selectedText());
    }

    textEditObject->setCursorPosition(0);
    QVERIFY(textEditObject->cursorPosition() == 0);
    QVERIFY(textEditObject->selectionStart() == 0);
    QVERIFY(textEditObject->selectionEnd() == 0);
    QVERIFY(textEditObject->selectedText().isNull());

    //Test Error Ignoring behaviour
    textEditObject->setCursorPosition(0);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->select(-10,0);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->select(100,101);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->select(0,-10);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->select(0,100);
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->select(0,10);
    QVERIFY(textEditObject->selectedText().size() == 10);
    textEditObject->select(-10,0);
    QVERIFY(textEditObject->selectedText().size() == 10);
    textEditObject->select(100,101);
    QVERIFY(textEditObject->selectedText().size() == 10);
    textEditObject->select(0,-10);
    QVERIFY(textEditObject->selectedText().size() == 10);
    textEditObject->select(0,100);
    QVERIFY(textEditObject->selectedText().size() == 10);

    textEditObject->deselect();
    QVERIFY(textEditObject->selectedText().isNull());
    textEditObject->select(0,10);
    QVERIFY(textEditObject->selectedText().size() == 10);
    textEditObject->deselect();
    QVERIFY(textEditObject->selectedText().isNull());
}

void tst_qsgtextedit::isRightToLeft_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<bool>("emptyString");
    QTest::addColumn<bool>("firstCharacter");
    QTest::addColumn<bool>("lastCharacter");
    QTest::addColumn<bool>("middleCharacter");
    QTest::addColumn<bool>("startString");
    QTest::addColumn<bool>("midString");
    QTest::addColumn<bool>("endString");

    const quint16 arabic_str[] = { 0x0638, 0x0643, 0x00646, 0x0647, 0x0633, 0x0638, 0x0643, 0x00646, 0x0647, 0x0633, 0x0647};
    QTest::newRow("Empty") << "" << false << false << false << false << false << false << false;
    QTest::newRow("Neutral") << "23244242" << false << false << false << false << false << false << false;
    QTest::newRow("LTR") << "Hello world" << false << false << false << false << false << false << false;
    QTest::newRow("RTL") << QString::fromUtf16(arabic_str, 11) << false << true << true << true << true << true << true;
    QTest::newRow("Bidi RTL + LTR + RTL") << QString::fromUtf16(arabic_str, 11) + QString("Hello world") + QString::fromUtf16(arabic_str, 11) << false << true << true << false << true << true << true;
    QTest::newRow("Bidi LTR + RTL + LTR") << QString("Hello world") + QString::fromUtf16(arabic_str, 11) + QString("Hello world") << false << false << false << true << false << false << false;
}

void tst_qsgtextedit::isRightToLeft()
{
    QFETCH(QString, text);
    QFETCH(bool, emptyString);
    QFETCH(bool, firstCharacter);
    QFETCH(bool, lastCharacter);
    QFETCH(bool, middleCharacter);
    QFETCH(bool, startString);
    QFETCH(bool, midString);
    QFETCH(bool, endString);

    QSGTextEdit textEdit;
    textEdit.setText(text);

    // first test that the right string is delivered to the QString::isRightToLeft()
    QCOMPARE(textEdit.isRightToLeft(0,0), text.mid(0,0).isRightToLeft());
    QCOMPARE(textEdit.isRightToLeft(0,1), text.mid(0,1).isRightToLeft());
    QCOMPARE(textEdit.isRightToLeft(text.count()-2, text.count()-1), text.mid(text.count()-2, text.count()-1).isRightToLeft());
    QCOMPARE(textEdit.isRightToLeft(text.count()/2, text.count()/2 + 1), text.mid(text.count()/2, text.count()/2 + 1).isRightToLeft());
    QCOMPARE(textEdit.isRightToLeft(0,text.count()/4), text.mid(0,text.count()/4).isRightToLeft());
    QCOMPARE(textEdit.isRightToLeft(text.count()/4,3*text.count()/4), text.mid(text.count()/4,3*text.count()/4).isRightToLeft());
    if (text.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML TextEdit: isRightToLeft(start, end) called with the end property being smaller than the start.");
    QCOMPARE(textEdit.isRightToLeft(3*text.count()/4,text.count()-1), text.mid(3*text.count()/4,text.count()-1).isRightToLeft());

    // then test that the feature actually works
    QCOMPARE(textEdit.isRightToLeft(0,0), emptyString);
    QCOMPARE(textEdit.isRightToLeft(0,1), firstCharacter);
    QCOMPARE(textEdit.isRightToLeft(text.count()-2, text.count()-1), lastCharacter);
    QCOMPARE(textEdit.isRightToLeft(text.count()/2, text.count()/2 + 1), middleCharacter);
    QCOMPARE(textEdit.isRightToLeft(0,text.count()/4), startString);
    QCOMPARE(textEdit.isRightToLeft(text.count()/4,3*text.count()/4), midString);
    if (text.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML TextEdit: isRightToLeft(start, end) called with the end property being smaller than the start.");
    QCOMPARE(textEdit.isRightToLeft(3*text.count()/4,text.count()-1), endString);
}

void tst_qsgtextedit::keySelection()
{
    QSGView canvas(QUrl::fromLocalFile(SRCDIR "/data/navigation.qml"));
    canvas.show();
    QApplication::setActiveWindow(&canvas);
    QTest::qWaitForWindowShown(&canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&canvas));
    canvas.setFocus();

    QVERIFY(canvas.rootObject() != 0);

    QSGTextEdit *input = qobject_cast<QSGTextEdit *>(qvariant_cast<QObject *>(canvas.rootObject()->property("myInput")));

    QVERIFY(input != 0);
    QTRY_VERIFY(input->hasActiveFocus() == true);

    QSignalSpy spy(input, SIGNAL(selectionChanged()));

    simulateKey(&canvas, Qt::Key_Right, Qt::ShiftModifier);
    QVERIFY(input->hasActiveFocus() == true);
    QCOMPARE(input->selectedText(), QString("a"));
    QCOMPARE(spy.count(), 1);
    simulateKey(&canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == true);
    QCOMPARE(input->selectedText(), QString());
    QCOMPARE(spy.count(), 2);
    simulateKey(&canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == false);
    QCOMPARE(input->selectedText(), QString());
    QCOMPARE(spy.count(), 2);

    simulateKey(&canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == true);
    QCOMPARE(spy.count(), 2);
    simulateKey(&canvas, Qt::Key_Left, Qt::ShiftModifier);
    QVERIFY(input->hasActiveFocus() == true);
    QCOMPARE(input->selectedText(), QString("a"));
    QCOMPARE(spy.count(), 3);
    simulateKey(&canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == true);
    QCOMPARE(input->selectedText(), QString());
    QCOMPARE(spy.count(), 4);
    simulateKey(&canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == false);
    QCOMPARE(input->selectedText(), QString());
    QCOMPARE(spy.count(), 4);
}

void tst_qsgtextedit::moveCursorSelection_data()
{
    QTest::addColumn<QString>("testStr");
    QTest::addColumn<int>("cursorPosition");
    QTest::addColumn<int>("movePosition");
    QTest::addColumn<QSGTextEdit::SelectionMode>("mode");
    QTest::addColumn<int>("selectionStart");
    QTest::addColumn<int>("selectionEnd");
    QTest::addColumn<bool>("reversible");

    QTest::newRow("(t)he|characters")
            << standard[0] << 0 << 1 << QSGTextEdit::SelectCharacters << 0 << 1 << true;
    QTest::newRow("do(g)|characters")
            << standard[0] << 43 << 44 << QSGTextEdit::SelectCharacters << 43 << 44 << true;
    QTest::newRow("jum(p)ed|characters")
            << standard[0] << 23 << 24 << QSGTextEdit::SelectCharacters << 23 << 24 << true;
    QTest::newRow("jumped( )over|characters")
            << standard[0] << 26 << 27 << QSGTextEdit::SelectCharacters << 26 << 27 << true;
    QTest::newRow("(the )|characters")
            << standard[0] << 0 << 4 << QSGTextEdit::SelectCharacters << 0 << 4 << true;
    QTest::newRow("( dog)|characters")
            << standard[0] << 40 << 44 << QSGTextEdit::SelectCharacters << 40 << 44 << true;
    QTest::newRow("( jumped )|characters")
            << standard[0] << 19 << 27 << QSGTextEdit::SelectCharacters << 19 << 27 << true;
    QTest::newRow("th(e qu)ick|characters")
            << standard[0] << 2 << 6 << QSGTextEdit::SelectCharacters << 2 << 6 << true;
    QTest::newRow("la(zy d)og|characters")
            << standard[0] << 38 << 42 << QSGTextEdit::SelectCharacters << 38 << 42 << true;
    QTest::newRow("jum(ped ov)er|characters")
            << standard[0] << 23 << 29 << QSGTextEdit::SelectCharacters << 23 << 29 << true;
    QTest::newRow("()the|characters")
            << standard[0] << 0 << 0 << QSGTextEdit::SelectCharacters << 0 << 0 << true;
    QTest::newRow("dog()|characters")
            << standard[0] << 44 << 44 << QSGTextEdit::SelectCharacters << 44 << 44 << true;
    QTest::newRow("jum()ped|characters")
            << standard[0] << 23 << 23 << QSGTextEdit::SelectCharacters << 23 << 23 << true;

    QTest::newRow("<(t)he>|words")
            << standard[0] << 0 << 1 << QSGTextEdit::SelectWords << 0 << 3 << true;
    QTest::newRow("<do(g)>|words")
            << standard[0] << 43 << 44 << QSGTextEdit::SelectWords << 41 << 44 << true;
    QTest::newRow("<jum(p)ed>|words")
            << standard[0] << 23 << 24 << QSGTextEdit::SelectWords << 20 << 26 << true;
    QTest::newRow("<jumped( )>over|words")
            << standard[0] << 26 << 27 << QSGTextEdit::SelectWords << 20 << 27 << false;
    QTest::newRow("jumped<( )over>|words,reversed")
            << standard[0] << 27 << 26 << QSGTextEdit::SelectWords << 26 << 31 << false;
    QTest::newRow("<(the )>quick|words")
            << standard[0] << 0 << 4 << QSGTextEdit::SelectWords << 0 << 4 << false;
    QTest::newRow("<(the )quick>|words,reversed")
            << standard[0] << 4 << 0 << QSGTextEdit::SelectWords << 0 << 9 << false;
    QTest::newRow("<lazy( dog)>|words")
            << standard[0] << 40 << 44 << QSGTextEdit::SelectWords << 36 << 44 << false;
    QTest::newRow("lazy<( dog)>|words,reversed")
            << standard[0] << 44 << 40 << QSGTextEdit::SelectWords << 40 << 44 << false;
    QTest::newRow("<fox( jumped )>over|words")
            << standard[0] << 19 << 27 << QSGTextEdit::SelectWords << 16 << 27 << false;
    QTest::newRow("fox<( jumped )over>|words,reversed")
            << standard[0] << 27 << 19 << QSGTextEdit::SelectWords << 19 << 31 << false;
    QTest::newRow("<th(e qu)ick>|words")
            << standard[0] << 2 << 6 << QSGTextEdit::SelectWords << 0 << 9 << true;
    QTest::newRow("<la(zy d)og|words>")
            << standard[0] << 38 << 42 << QSGTextEdit::SelectWords << 36 << 44 << true;
    QTest::newRow("<jum(ped ov)er>|words")
            << standard[0] << 23 << 29 << QSGTextEdit::SelectWords << 20 << 31 << true;
    QTest::newRow("<()>the|words")
            << standard[0] << 0 << 0 << QSGTextEdit::SelectWords << 0 << 0 << true;
    QTest::newRow("dog<()>|words")
            << standard[0] << 44 << 44 << QSGTextEdit::SelectWords << 44 << 44 << true;
    QTest::newRow("jum<()>ped|words")
            << standard[0] << 23 << 23 << QSGTextEdit::SelectWords << 23 << 23 << true;

    QTest::newRow("Hello<(,)> |words")
            << standard[2] << 5 << 6 << QSGTextEdit::SelectWords << 5 << 6 << true;
    QTest::newRow("Hello<(, )>world|words")
            << standard[2] << 5 << 7 << QSGTextEdit::SelectWords << 5 << 7 << false;
    QTest::newRow("Hello<(, )world>|words,reversed")
            << standard[2] << 7 << 5 << QSGTextEdit::SelectWords << 5 << 12 << false;
    QTest::newRow("<Hel(lo, )>world|words")
            << standard[2] << 3 << 7 << QSGTextEdit::SelectWords << 0 << 7 << false;
    QTest::newRow("<Hel(lo, )world>|words,reversed")
            << standard[2] << 7 << 3 << QSGTextEdit::SelectWords << 0 << 12 << false;
    QTest::newRow("<Hel(lo)>,|words")
            << standard[2] << 3 << 5 << QSGTextEdit::SelectWords << 0 << 5 << true;
    QTest::newRow("Hello<()>,|words")
            << standard[2] << 5 << 5 << QSGTextEdit::SelectWords << 5 << 5 << true;
    QTest::newRow("Hello,<()>|words")
            << standard[2] << 6 << 6 << QSGTextEdit::SelectWords << 6 << 6 << true;
    QTest::newRow("Hello<,( )>world|words")
            << standard[2] << 6 << 7 << QSGTextEdit::SelectWords << 5 << 7 << false;
    QTest::newRow("Hello,<( )world>|words,reversed")
            << standard[2] << 7 << 6 << QSGTextEdit::SelectWords << 6 << 12 << false;
    QTest::newRow("Hello<,( world)>|words")
            << standard[2] << 6 << 12 << QSGTextEdit::SelectWords << 5 << 12 << false;
    QTest::newRow("Hello,<( world)>|words,reversed")
            << standard[2] << 12 << 6 << QSGTextEdit::SelectWords << 6 << 12 << false;
    QTest::newRow("Hello<,( world!)>|words")
            << standard[2] << 6 << 13 << QSGTextEdit::SelectWords << 5 << 13 << false;
    QTest::newRow("Hello,<( world!)>|words,reversed")
            << standard[2] << 13 << 6 << QSGTextEdit::SelectWords << 6 << 13 << false;
    QTest::newRow("Hello<(, world!)>|words")
            << standard[2] << 5 << 13 << QSGTextEdit::SelectWords << 5 << 13 << true;
    QTest::newRow("world<(!)>|words")
            << standard[2] << 12 << 13 << QSGTextEdit::SelectWords << 12 << 13 << true;
    QTest::newRow("world!<()>)|words")
            << standard[2] << 13 << 13 << QSGTextEdit::SelectWords << 13 << 13 << true;
    QTest::newRow("world<()>!)|words")
            << standard[2] << 12 << 12 << QSGTextEdit::SelectWords << 12 << 12 << true;

    QTest::newRow("<(,)>olleH |words")
            << standard[3] << 7 << 8 << QSGTextEdit::SelectWords << 7 << 8 << true;
    QTest::newRow("<dlrow( ,)>olleH|words")
            << standard[3] << 6 << 8 << QSGTextEdit::SelectWords << 1 << 8 << false;
    QTest::newRow("dlrow<( ,)>olleH|words,reversed")
            << standard[3] << 8 << 6 << QSGTextEdit::SelectWords << 6 << 8 << false;
    QTest::newRow("<dlrow( ,ol)leH>|words")
            << standard[3] << 6 << 10 << QSGTextEdit::SelectWords << 1 << 13 << false;
    QTest::newRow("dlrow<( ,ol)leH>|words,reversed")
            << standard[3] << 10 << 6 << QSGTextEdit::SelectWords << 6 << 13 << false;
    QTest::newRow(",<(ol)leH>,|words")
            << standard[3] << 8 << 10 << QSGTextEdit::SelectWords << 8 << 13 << true;
    QTest::newRow(",<()>olleH|words")
            << standard[3] << 8 << 8 << QSGTextEdit::SelectWords << 8 << 8 << true;
    QTest::newRow("<()>,olleH|words")
            << standard[3] << 7 << 7 << QSGTextEdit::SelectWords << 7 << 7 << true;
    QTest::newRow("<dlrow( )>,olleH|words")
            << standard[3] << 6 << 7 << QSGTextEdit::SelectWords << 1 << 7 << false;
    QTest::newRow("dlrow<( ),>olleH|words,reversed")
            << standard[3] << 7 << 6 << QSGTextEdit::SelectWords << 6 << 8 << false;
    QTest::newRow("<(dlrow )>,olleH|words")
            << standard[3] << 1 << 7 << QSGTextEdit::SelectWords << 1 << 7 << false;
    QTest::newRow("<(dlrow ),>olleH|words,reversed")
            << standard[3] << 7 << 1 << QSGTextEdit::SelectWords << 1 << 8 << false;
    QTest::newRow("<(!dlrow )>,olleH|words")
            << standard[3] << 0 << 7 << QSGTextEdit::SelectWords << 0 << 7 << false;
    QTest::newRow("<(!dlrow ),>olleH|words,reversed")
            << standard[3] << 7 << 0 << QSGTextEdit::SelectWords << 0 << 8 << false;
    QTest::newRow("(!dlrow ,)olleH|words")
            << standard[3] << 0 << 8 << QSGTextEdit::SelectWords << 0 << 8 << true;
    QTest::newRow("<(!)>dlrow|words")
            << standard[3] << 0 << 1 << QSGTextEdit::SelectWords << 0 << 1 << true;
    QTest::newRow("<()>!dlrow|words")
            << standard[3] << 0 << 0 << QSGTextEdit::SelectWords << 0 << 0 << true;
    QTest::newRow("!<()>dlrow|words")
            << standard[3] << 1 << 1 << QSGTextEdit::SelectWords << 1 << 1 << true;
}

void tst_qsgtextedit::moveCursorSelection()
{
    QFETCH(QString, testStr);
    QFETCH(int, cursorPosition);
    QFETCH(int, movePosition);
    QFETCH(QSGTextEdit::SelectionMode, mode);
    QFETCH(int, selectionStart);
    QFETCH(int, selectionEnd);
    QFETCH(bool, reversible);

    QString componentStr = "import QtQuick 2.0\nTextEdit {  text: \""+ testStr +"\"; }";
    QDeclarativeComponent textinputComponent(&engine);
    textinputComponent.setData(componentStr.toLatin1(), QUrl());
    QSGTextEdit *texteditObject = qobject_cast<QSGTextEdit*>(textinputComponent.create());
    QVERIFY(texteditObject != 0);

    texteditObject->setCursorPosition(cursorPosition);
    texteditObject->moveCursorSelection(movePosition, mode);

    QCOMPARE(texteditObject->selectedText(), testStr.mid(selectionStart, selectionEnd - selectionStart));
    QCOMPARE(texteditObject->selectionStart(), selectionStart);
    QCOMPARE(texteditObject->selectionEnd(), selectionEnd);

    if (reversible) {
        texteditObject->setCursorPosition(movePosition);
        texteditObject->moveCursorSelection(cursorPosition, mode);

        QCOMPARE(texteditObject->selectedText(), testStr.mid(selectionStart, selectionEnd - selectionStart));
        QCOMPARE(texteditObject->selectionStart(), selectionStart);
        QCOMPARE(texteditObject->selectionEnd(), selectionEnd);
    }
}

void tst_qsgtextedit::moveCursorSelectionSequence_data()
{
    QTest::addColumn<QString>("testStr");
    QTest::addColumn<int>("cursorPosition");
    QTest::addColumn<int>("movePosition1");
    QTest::addColumn<int>("movePosition2");
    QTest::addColumn<int>("selection1Start");
    QTest::addColumn<int>("selection1End");
    QTest::addColumn<int>("selection2Start");
    QTest::addColumn<int>("selection2End");

    QTest::newRow("the {<quick( bro)wn> f^ox} jumped|ltr")
            << standard[0]
            << 9 << 13 << 17
            << 4 << 15
            << 4 << 19;
    QTest::newRow("the quick<( {bro)wn> f^ox} jumped|rtl")
            << standard[0]
            << 13 << 9 << 17
            << 9 << 15
            << 10 << 19;
    QTest::newRow("the {<quick( bro)wn> ^}fox jumped|ltr")
            << standard[0]
            << 9 << 13 << 16
            << 4 << 15
            << 4 << 16;
    QTest::newRow("the quick<( {bro)wn> ^}fox jumped|rtl")
            << standard[0]
            << 13 << 9 << 16
            << 9 << 15
            << 10 << 16;
    QTest::newRow("the {<quick( bro)wn^>} fox jumped|ltr")
            << standard[0]
            << 9 << 13 << 15
            << 4 << 15
            << 4 << 15;
    QTest::newRow("the quick<( {bro)wn^>} f^ox jumped|rtl")
            << standard[0]
            << 13 << 9 << 15
            << 9 << 15
            << 10 << 15;
    QTest::newRow("the {<quick() ^}bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 10
            << 4 << 15
            << 4 << 10;
    QTest::newRow("the quick<(^ {^bro)wn>} fox|rtl")
            << standard[0]
            << 13 << 9 << 10
            << 9 << 15
            << 10 << 15;
    QTest::newRow("the {<quick^}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 9
            << 4 << 15
            << 4 << 9;
    QTest::newRow("the quick{<(^ bro)wn>} fox|rtl")
            << standard[0]
            << 13 << 9 << 9
            << 9 << 15
            << 9 << 15;
    QTest::newRow("the {<qui^ck}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 7
            << 4 << 15
            << 4 << 9;
    QTest::newRow("the {<qui^ck}( bro)wn> fox|rtl")
            << standard[0]
            << 13 << 9 << 7
            << 9 << 15
            << 4 << 15;
    QTest::newRow("the {<^quick}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 4
            << 4 << 15
            << 4 << 9;
    QTest::newRow("the {<^quick}( bro)wn> fox|rtl")
            << standard[0]
            << 13 << 9 << 4
            << 9 << 15
            << 4 << 15;
    QTest::newRow("the{^ <quick}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 3
            << 4 << 15
            << 3 << 9;
    QTest::newRow("the{^ <quick}( bro)wn> fox|rtl")
            << standard[0]
            << 13 << 9 << 3
            << 9 << 15
            << 3 << 15;
    QTest::newRow("{t^he <quick}( bro)wn> fox|ltr")
            << standard[0]
            << 9 << 13 << 1
            << 4 << 15
            << 0 << 9;
    QTest::newRow("{t^he <quick}( bro)wn> fox|rtl")
            << standard[0]
            << 13 << 9 << 1
            << 9 << 15
            << 0 << 15;

    QTest::newRow("{<He(ll)o>, w^orld}!|ltr")
            << standard[2]
            << 2 << 4 << 8
            << 0 << 5
            << 0 << 12;
    QTest::newRow("{<He(ll)o>, w^orld}!|rtl")
            << standard[2]
            << 4 << 2 << 8
            << 0 << 5
            << 0 << 12;

    QTest::newRow("!{dlro^w ,<o(ll)eH>}|ltr")
            << standard[3]
            << 9 << 11 << 5
            << 8 << 13
            << 1 << 13;
    QTest::newRow("!{dlro^w ,<o(ll)eH>}|rtl")
            << standard[3]
            << 11 << 9 << 5
            << 8 << 13
            << 1 << 13;
}

void tst_qsgtextedit::moveCursorSelectionSequence()
{
    QFETCH(QString, testStr);
    QFETCH(int, cursorPosition);
    QFETCH(int, movePosition1);
    QFETCH(int, movePosition2);
    QFETCH(int, selection1Start);
    QFETCH(int, selection1End);
    QFETCH(int, selection2Start);
    QFETCH(int, selection2End);

    QString componentStr = "import QtQuick 2.0\nTextEdit {  text: \""+ testStr +"\"; }";
    QDeclarativeComponent texteditComponent(&engine);
    texteditComponent.setData(componentStr.toLatin1(), QUrl());
    QSGTextEdit *texteditObject = qobject_cast<QSGTextEdit*>(texteditComponent.create());
    QVERIFY(texteditObject != 0);

    texteditObject->setCursorPosition(cursorPosition);

    texteditObject->moveCursorSelection(movePosition1, QSGTextEdit::SelectWords);
    QCOMPARE(texteditObject->selectedText(), testStr.mid(selection1Start, selection1End - selection1Start));
    QCOMPARE(texteditObject->selectionStart(), selection1Start);
    QCOMPARE(texteditObject->selectionEnd(), selection1End);

    texteditObject->moveCursorSelection(movePosition2, QSGTextEdit::SelectWords);
    QCOMPARE(texteditObject->selectedText(), testStr.mid(selection2Start, selection2End - selection2Start));
    QCOMPARE(texteditObject->selectionStart(), selection2Start);
    QCOMPARE(texteditObject->selectionEnd(), selection2End);
}


void tst_qsgtextedit::mouseSelection_data()
{
    QTest::addColumn<QString>("qmlfile");
    QTest::addColumn<bool>("expectSelection");

    // import installed
    QTest::newRow("on") << SRCDIR "/data/mouseselection_true.qml" << true;
    QTest::newRow("off") << SRCDIR "/data/mouseselection_false.qml" << false;
    QTest::newRow("default") << SRCDIR "/data/mouseselection_default.qml" << false;
    QTest::newRow("on word selection") << SRCDIR "/data/mouseselection_true_words.qml" << true;
    QTest::newRow("off word selection") << SRCDIR "/data/mouseselection_false_words.qml" << false;
}

void tst_qsgtextedit::mouseSelection()
{
    QFETCH(QString, qmlfile);
    QFETCH(bool, expectSelection);

    QSGView canvas(QUrl::fromLocalFile(qmlfile));

    canvas.show();
    QApplication::setActiveWindow(&canvas);
    QTest::qWaitForWindowShown(&canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&canvas));

    QVERIFY(canvas.rootObject() != 0);
    QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit *>(canvas.rootObject());
    QVERIFY(textEditObject != 0);

    // press-and-drag-and-release from x1 to x2
    int x1 = 10;
    int x2 = 70;
    int y = textEditObject->height()/2;
    QTest::mousePress(&canvas, Qt::LeftButton, 0, QPoint(x1,y));
    //QTest::mouseMove(canvas, QPoint(x2,y)); // doesn't work
    QMouseEvent mv(QEvent::MouseMove, QPoint(x2,y), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
    QApplication::sendEvent(&canvas, &mv);
    QTest::mouseRelease(&canvas, Qt::LeftButton, 0, QPoint(x2,y));
    QString str = textEditObject->selectedText();
    if (expectSelection)
        QVERIFY(str.length() > 3); // don't reallly care *what* was selected (and it's too sensitive to platform)
    else
        QVERIFY(str.isEmpty());
}

void tst_qsgtextedit::dragMouseSelection()
{
    QString qmlfile = SRCDIR "/data/mouseselection_true.qml";

    QSGView canvas(QUrl::fromLocalFile(qmlfile));

    canvas.show();
    QApplication::setActiveWindow(&canvas);
    QTest::qWaitForWindowShown(&canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&canvas));

    QVERIFY(canvas.rootObject() != 0);
    QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit *>(canvas.rootObject());
    QVERIFY(textEditObject != 0);

    // press-and-drag-and-release from x1 to x2
    int x1 = 10;
    int x2 = 70;
    int y = textEditObject->height()/2;
    QTest::mousePress(&canvas, Qt::LeftButton, 0, QPoint(x1,y));
    {
        QMouseEvent mv(QEvent::MouseMove, QPoint(x2,y), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(&canvas, &mv);
    }
    QTest::mouseRelease(&canvas, Qt::LeftButton, 0, QPoint(x2,y));
    QString str1 = textEditObject->selectedText();
    QVERIFY(str1.length() > 3);

    // press and drag the current selection.
    x1 = 40;
    x2 = 100;
    QTest::mousePress(&canvas, Qt::LeftButton, 0, QPoint(x1,y));
    {
        QMouseEvent mv(QEvent::MouseMove, QPoint(x2,y), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(&canvas, &mv);
    }
    QTest::mouseRelease(&canvas, Qt::LeftButton, 0, QPoint(x2,y));
    QString str2 = textEditObject->selectedText();
    QVERIFY(str2.length() > 3);

    QVERIFY(str1 != str2); // Verify the second press and drag is a new selection and not the first moved.
}

void tst_qsgtextedit::mouseSelectionMode_data()
{
    QTest::addColumn<QString>("qmlfile");
    QTest::addColumn<bool>("selectWords");

    // import installed
    QTest::newRow("SelectWords") << SRCDIR "/data/mouseselectionmode_words.qml" << true;
    QTest::newRow("SelectCharacters") << SRCDIR "/data/mouseselectionmode_characters.qml" << false;
    QTest::newRow("default") << SRCDIR "/data/mouseselectionmode_default.qml" << false;
}

void tst_qsgtextedit::mouseSelectionMode()
{
    QFETCH(QString, qmlfile);
    QFETCH(bool, selectWords);

    QString text = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    QSGView canvas(QUrl::fromLocalFile(qmlfile));

    canvas.show();
    QApplication::setActiveWindow(&canvas);
    QTest::qWaitForWindowShown(&canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&canvas));

    QVERIFY(canvas.rootObject() != 0);
    QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit *>(canvas.rootObject());
    QVERIFY(textEditObject != 0);

    // press-and-drag-and-release from x1 to x2
    int x1 = 10;
    int x2 = 70;
    int y = textEditObject->height()/2;
    QTest::mousePress(&canvas, Qt::LeftButton, 0, QPoint(x1,y));
    //QTest::mouseMove(canvas, QPoint(x2,y)); // doesn't work
    QMouseEvent mv(QEvent::MouseMove, QPoint(x2,y), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
    QApplication::sendEvent(&canvas, &mv);
    QTest::mouseRelease(&canvas, Qt::LeftButton, 0, QPoint(x2,y));
    QString str = textEditObject->selectedText();
    if (selectWords) {
        QCOMPARE(str, text);
    } else {
        QVERIFY(str.length() > 3);
        QVERIFY(str != text);
    }
}

void tst_qsgtextedit::inputMethodHints()
{
    QSGView canvas(QUrl::fromLocalFile(SRCDIR "/data/inputmethodhints.qml"));
    canvas.show();
    canvas.setFocus();

    QVERIFY(canvas.rootObject() != 0);
    QSGTextEdit *textEditObject = qobject_cast<QSGTextEdit *>(canvas.rootObject());
    QVERIFY(textEditObject != 0);
    QVERIFY(textEditObject->inputMethodHints() & Qt::ImhNoPredictiveText);
    textEditObject->setInputMethodHints(Qt::ImhUppercaseOnly);
    QVERIFY(textEditObject->inputMethodHints() & Qt::ImhUppercaseOnly);
}

void tst_qsgtextedit::positionAt()
{
    QSGView canvas(QUrl::fromLocalFile(SRCDIR "/data/positionAt.qml"));
    QVERIFY(canvas.rootObject() != 0);
    canvas.show();
    canvas.setFocus();
    QApplication::setActiveWindow(&canvas);
    QTest::qWaitForWindowShown(&canvas);

    QSGTextEdit *texteditObject = qobject_cast<QSGTextEdit *>(canvas.rootObject());
    QVERIFY(texteditObject != 0);

    QFontMetrics fm(texteditObject->font());
    const int y0 = fm.height() / 2;
    const int y1 = fm.height() * 3 / 2;

    int pos = texteditObject->positionAt(texteditObject->width()/2, y0);
    int diff = abs(int(fm.width(texteditObject->text().left(pos))-texteditObject->width()/2));

    // some tollerance for different fonts.
#ifdef Q_OS_LINUX
    QVERIFY(diff < 2);
#else
    QVERIFY(diff < 5);
#endif

    const qreal x0 = texteditObject->positionToRectangle(pos).x();
    const qreal x1 = texteditObject->positionToRectangle(pos + 1).x();

    QString preeditText = texteditObject->text().mid(0, pos);
    texteditObject->setText(texteditObject->text().mid(pos));
    texteditObject->setCursorPosition(0);

    QInputMethodEvent inputEvent(preeditText, QList<QInputMethodEvent::Attribute>());
    QApplication::sendEvent(&canvas, &inputEvent);

    // Check all points within the preedit text return the same position.
    QCOMPARE(texteditObject->positionAt(0, y0), 0);
    QCOMPARE(texteditObject->positionAt(x0 / 2, y0), 0);
    QCOMPARE(texteditObject->positionAt(x0, y0), 0);

    // Verify positioning returns to normal after the preedit text.
    QCOMPARE(texteditObject->positionAt(x1, y0), 1);
    QCOMPARE(texteditObject->positionToRectangle(1).x(), x1);

    QVERIFY(texteditObject->positionAt(x0 / 2, y1) > 0);
}

void tst_qsgtextedit::cursorDelegate()
{
    QSGView view(QUrl::fromLocalFile(SRCDIR "/data/cursorTest.qml"));
    view.show();
    view.setFocus();
    QSGTextEdit *textEditObject = view.rootObject()->findChild<QSGTextEdit*>("textEditObject");
    QVERIFY(textEditObject != 0);
    QVERIFY(textEditObject->findChild<QSGItem*>("cursorInstance"));
    //Test Delegate gets created
    textEditObject->setFocus(true);
    QSGItem* delegateObject = textEditObject->findChild<QSGItem*>("cursorInstance");
    QVERIFY(delegateObject);
    //Test Delegate gets moved
    for(int i=0; i<= textEditObject->text().length(); i++){
        textEditObject->setCursorPosition(i);
        QCOMPARE(textEditObject->cursorRectangle().x(), qRound(delegateObject->x()));
        QCOMPARE(textEditObject->cursorRectangle().y(), qRound(delegateObject->y()));
    }
    textEditObject->setCursorPosition(0);
    QCOMPARE(textEditObject->cursorRectangle().x(), qRound(delegateObject->x()));
    QCOMPARE(textEditObject->cursorRectangle().y(), qRound(delegateObject->y()));
    //Test Delegate gets deleted
    textEditObject->setCursorDelegate(0);
    QVERIFY(!textEditObject->findChild<QSGItem*>("cursorInstance"));
}

void tst_qsgtextedit::cursorVisible()
{
    QSGView view(QUrl::fromLocalFile(SRCDIR "/data/cursorVisible.qml"));
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    view.setFocus();

    QSGTextEdit edit;
    QSignalSpy spy(&edit, SIGNAL(cursorVisibleChanged(bool)));

    QCOMPARE(edit.isCursorVisible(), false);

    edit.setCursorVisible(true);
    QCOMPARE(edit.isCursorVisible(), true);
    QCOMPARE(spy.count(), 1);

    edit.setCursorVisible(false);
    QCOMPARE(edit.isCursorVisible(), false);
    QCOMPARE(spy.count(), 2);

    edit.setFocus(true);
    QCOMPARE(edit.isCursorVisible(), false);
    QCOMPARE(spy.count(), 2);

    edit.setParentItem(view.rootObject());
    QCOMPARE(edit.isCursorVisible(), true);
    QCOMPARE(spy.count(), 3);

    edit.setFocus(false);
    QCOMPARE(edit.isCursorVisible(), false);
    QCOMPARE(spy.count(), 4);

    edit.setFocus(true);
    QCOMPARE(edit.isCursorVisible(), true);
    QCOMPARE(spy.count(), 5);

    view.clearFocus();
    QCOMPARE(edit.isCursorVisible(), false);
    QCOMPARE(spy.count(), 6);

    view.setFocus();
    QCOMPARE(edit.isCursorVisible(), true);
    QCOMPARE(spy.count(), 7);

    // on mac, setActiveWindow(0) on mac does not deactivate the current application
    // (you have to switch to a different app or hide the current app to trigger this)
#if !defined(Q_WS_MAC)
    QApplication::setActiveWindow(0);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(0));
    QCOMPARE(edit.isCursorVisible(), false);
    QCOMPARE(spy.count(), 8);

    QApplication::setActiveWindow(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    QCOMPARE(edit.isCursorVisible(), true);
    QCOMPARE(spy.count(), 9);
#endif
}

void tst_qsgtextedit::delegateLoading_data()
{
    QTest::addColumn<QString>("qmlfile");
    QTest::addColumn<QString>("error");

    // import installed
    QTest::newRow("pass") << "cursorHttpTestPass.qml" << "";
    QTest::newRow("fail1") << "cursorHttpTestFail1.qml" << "http://localhost:42332/FailItem.qml: Remote host closed the connection ";
    QTest::newRow("fail2") << "cursorHttpTestFail2.qml" << "http://localhost:42332/ErrItem.qml:4:5: Fungus is not a type ";
}

void tst_qsgtextedit::delegateLoading()
{
    QFETCH(QString, qmlfile);
    QFETCH(QString, error);

    TestHTTPServer server(42332);
    server.serveDirectory(SRCDIR "/data/httpfail", TestHTTPServer::Disconnect);
    server.serveDirectory(SRCDIR "/data/httpslow", TestHTTPServer::Delay);
    server.serveDirectory(SRCDIR "/data/http");

    QSGView view(QUrl(QLatin1String("http://localhost:42332/") + qmlfile));
    view.show();
    view.setFocus();

    if (!error.isEmpty()) {
        QTest::ignoreMessage(QtWarningMsg, error.toUtf8());
        QTRY_VERIFY(view.status()==QSGView::Error);
        QTRY_VERIFY(!view.rootObject()); // there is fail item inside this test
    } else {
        QTRY_VERIFY(view.rootObject());//Wait for loading to finish.
        QSGTextEdit *textEditObject = view.rootObject()->findChild<QSGTextEdit*>("textEditObject");
        //    view.rootObject()->dumpObjectTree();
        QVERIFY(textEditObject != 0);
        textEditObject->setFocus(true);
        QSGItem *delegate;
        delegate = view.rootObject()->findChild<QSGItem*>("delegateOkay");
        QVERIFY(delegate);
        delegate = view.rootObject()->findChild<QSGItem*>("delegateSlow");
        QVERIFY(delegate);

        delete delegate;
    }


    //A test should be added here with a component which is ready but component.create() returns null
    //Not sure how to accomplish this with QSGTextEdits cursor delegate
    //###This was only needed for code coverage, and could be a case of overzealous defensive programming
    //delegate = view.rootObject()->findChild<QSGItem*>("delegateErrorB");
    //QVERIFY(!delegate);
}

/*
TextEdit element should only handle left/right keys until the cursor reaches
the extent of the text, then they should ignore the keys.
*/
void tst_qsgtextedit::navigation()
{
    QSGView canvas(QUrl::fromLocalFile(SRCDIR "/data/navigation.qml"));
    canvas.show();
    canvas.setFocus();

    QVERIFY(canvas.rootObject() != 0);

    QSGItem *input = qobject_cast<QSGItem *>(qvariant_cast<QObject *>(canvas.rootObject()->property("myInput")));

    QVERIFY(input != 0);
    QTRY_VERIFY(input->hasActiveFocus() == true);
    simulateKey(&canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == false);
    simulateKey(&canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == true);
    simulateKey(&canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == true);
    simulateKey(&canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == false);
    simulateKey(&canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == true);
}

void tst_qsgtextedit::copyAndPaste() {
#ifndef QT_NO_CLIPBOARD

#ifdef Q_WS_MAC
    {
        PasteboardRef pasteboard;
        OSStatus status = PasteboardCreate(0, &pasteboard);
        if (status == noErr)
            CFRelease(pasteboard);
        else
            QSKIP("This machine doesn't support the clipboard", SkipAll);
    }
#endif

    QString componentStr = "import QtQuick 2.0\nTextEdit { text: \"Hello world!\" }";
    QDeclarativeComponent textEditComponent(&engine);
    textEditComponent.setData(componentStr.toLatin1(), QUrl());
    QSGTextEdit *textEdit = qobject_cast<QSGTextEdit*>(textEditComponent.create());
    QVERIFY(textEdit != 0);

    // copy and paste
    QCOMPARE(textEdit->text().length(), 12);
    textEdit->select(0, textEdit->text().length());;
    textEdit->copy();
    QCOMPARE(textEdit->selectedText(), QString("Hello world!"));
    QCOMPARE(textEdit->selectedText().length(), 12);
    textEdit->setCursorPosition(0);
    QVERIFY(textEdit->canPaste());
    textEdit->paste();
    QCOMPARE(textEdit->text(), QString("Hello world!Hello world!"));
    QCOMPARE(textEdit->text().length(), 24);

    // canPaste
    QVERIFY(textEdit->canPaste());
    textEdit->setReadOnly(true);
    QVERIFY(!textEdit->canPaste());
    textEdit->setReadOnly(false);
    QVERIFY(textEdit->canPaste());

    // QTBUG-12339
    // test that document and internal text attribute are in sync
    QSGItemPrivate* pri = QSGItemPrivate::get(textEdit);
    QSGTextEditPrivate *editPrivate = static_cast<QSGTextEditPrivate*>(pri);
    QCOMPARE(textEdit->text(), editPrivate->text);

    // select word
    textEdit->setCursorPosition(0);
    textEdit->selectWord();
    QCOMPARE(textEdit->selectedText(), QString("Hello"));

    // select all and cut
    textEdit->selectAll();
    textEdit->cut();
    QCOMPARE(textEdit->text().length(), 0);
    textEdit->paste();
    QCOMPARE(textEdit->text(), QString("Hello world!Hello world!"));
    QCOMPARE(textEdit->text().length(), 24);
#endif
}

void tst_qsgtextedit::canPaste() {
#ifndef QT_NO_CLIPBOARD

    QApplication::clipboard()->setText("Some text");

    QString componentStr = "import QtQuick 2.0\nTextEdit { text: \"Hello world!\" }";
    QDeclarativeComponent textEditComponent(&engine);
    textEditComponent.setData(componentStr.toLatin1(), QUrl());
    QSGTextEdit *textEdit = qobject_cast<QSGTextEdit*>(textEditComponent.create());
    QVERIFY(textEdit != 0);

    // check initial value - QTBUG-17765
    QTextControl tc;
    QCOMPARE(textEdit->canPaste(), tc.canPaste());

#endif
}

void tst_qsgtextedit::canPasteEmpty() {
#ifndef QT_NO_CLIPBOARD

    QApplication::clipboard()->clear();

    QString componentStr = "import QtQuick 2.0\nTextEdit { text: \"Hello world!\" }";
    QDeclarativeComponent textEditComponent(&engine);
    textEditComponent.setData(componentStr.toLatin1(), QUrl());
    QSGTextEdit *textEdit = qobject_cast<QSGTextEdit*>(textEditComponent.create());
    QVERIFY(textEdit != 0);

    // check initial value - QTBUG-17765
    QTextControl tc;
    QCOMPARE(textEdit->canPaste(), tc.canPaste());

#endif
}

void tst_qsgtextedit::readOnly()
{
    QSGView canvas(QUrl::fromLocalFile(SRCDIR "/data/readOnly.qml"));
    canvas.show();
    canvas.setFocus();

    QVERIFY(canvas.rootObject() != 0);

    QSGTextEdit *edit = qobject_cast<QSGTextEdit *>(qvariant_cast<QObject *>(canvas.rootObject()->property("myInput")));

    QVERIFY(edit != 0);
    QTRY_VERIFY(edit->hasActiveFocus() == true);
    QVERIFY(edit->isReadOnly() == true);
    QString initial = edit->text();
    for(int k=Qt::Key_0; k<=Qt::Key_Z; k++)
        simulateKey(&canvas, k);
    simulateKey(&canvas, Qt::Key_Return);
    simulateKey(&canvas, Qt::Key_Space);
    simulateKey(&canvas, Qt::Key_Escape);
    QCOMPARE(edit->text(), initial);
}

void tst_qsgtextedit::simulateKey(QSGView *view, int key, Qt::KeyboardModifiers modifiers)
{
    QKeyEvent press(QKeyEvent::KeyPress, key, modifiers);
    QKeyEvent release(QKeyEvent::KeyRelease, key, modifiers);

    QApplication::sendEvent(view, &press);
    QApplication::sendEvent(view, &release);
}

class MyInputContext : public QInputContext
{
public:
    MyInputContext() : openInputPanelReceived(false), closeInputPanelReceived(false), updateReceived(false), eventType(QEvent::None) {}
    ~MyInputContext() {}

    QString identifierName() { return QString(); }
    QString language() { return QString(); }

    void reset() {}

    bool isComposing() const { return false; }

    bool filterEvent( const QEvent *event )
    {
        if (event->type() == QEvent::RequestSoftwareInputPanel)
            openInputPanelReceived = true;
        if (event->type() == QEvent::CloseSoftwareInputPanel)
            closeInputPanelReceived = true;
        return QInputContext::filterEvent(event);
    }

    void update() { updateReceived = true; }

    void sendPreeditText(const QString &text, int cursor)
    {
        QList<QInputMethodEvent::Attribute> attributes;
        attributes.append(QInputMethodEvent::Attribute(
                QInputMethodEvent::Cursor, cursor, text.length(), QVariant()));

        QInputMethodEvent event(text, attributes);
        sendEvent(event);
    }

    void mouseHandler(int x, QMouseEvent *event)
    {
        cursor = x;
        eventType = event->type();
        eventPosition = event->pos();
        eventGlobalPosition = event->globalPos();
        eventButton = event->button();
        eventButtons = event->buttons();
        eventModifiers = event->modifiers();
    }

    bool openInputPanelReceived;
    bool closeInputPanelReceived;
    bool updateReceived;
    int cursor;
    QEvent::Type eventType;
    QPoint eventPosition;
    QPoint eventGlobalPosition;
    Qt::MouseButton eventButton;
    Qt::MouseButtons eventButtons;
    Qt::KeyboardModifiers eventModifiers;
};

void tst_qsgtextedit::textInput()
{
    QSGView view(QUrl::fromLocalFile(SRCDIR "/data/inputMethodEvent.qml"));
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    QSGTextEdit *edit = qobject_cast<QSGTextEdit *>(view.rootObject());
    QVERIFY(edit);
    QVERIFY(edit->hasActiveFocus() == true);

    // test that input method event is committed
    QInputMethodEvent event;
    event.setCommitString( "Hello world!", 0, 0);
    QApplication::sendEvent(&view, &event);
    QCOMPARE(edit->text(), QString("Hello world!"));

    // QTBUG-12339
    // test that document and internal text attribute are in sync
    QSGTextEditPrivate *editPrivate = static_cast<QSGTextEditPrivate*>(QSGItemPrivate::get(edit));
    QCOMPARE(editPrivate->text, QString("Hello world!"));
}

void tst_qsgtextedit::openInputPanelOnClick()
{
    QSGView view(QUrl::fromLocalFile(SRCDIR "/data/openInputPanel.qml"));
    MyInputContext ic;
    // QSGCanvas won't set the Qt::WA_InputMethodEnabled flag unless a suitable item has focus
    // and QWidget won't allow an input context to be set when the flag is not set.
    view.setAttribute(Qt::WA_InputMethodEnabled, true);
    view.setInputContext(&ic);
    view.setAttribute(Qt::WA_InputMethodEnabled, false);
    view.show();

    qApp->setAutoSipEnabled(true);
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QSGTextEdit *edit = qobject_cast<QSGTextEdit *>(view.rootObject());
    QVERIFY(edit);
    QSignalSpy focusOnPressSpy(edit, SIGNAL(activeFocusOnPressChanged(bool)));

    QSGItemPrivate* pri = QSGItemPrivate::get(edit);
    QSGTextEditPrivate *editPrivate = static_cast<QSGTextEditPrivate*>(pri);

    // input panel on click
    editPrivate->showInputPanelOnFocus = false;

    QStyle::RequestSoftwareInputPanel behavior = QStyle::RequestSoftwareInputPanel(
            view.style()->styleHint(QStyle::SH_RequestSoftwareInputPanel));
    QTest::mouseClick(&view, Qt::LeftButton, 0, edit->mapToScene(QPointF(0,0)).toPoint());
    QApplication::processEvents();
    if (behavior == QStyle::RSIP_OnMouseClickAndAlreadyFocused) {
        QCOMPARE(ic.openInputPanelReceived, false);
        QTest::mouseClick(&view, Qt::LeftButton, 0, edit->mapToScene(QPointF(0,0)).toPoint());
        QApplication::processEvents();
        QCOMPARE(ic.openInputPanelReceived, true);
    } else if (behavior == QStyle::RSIP_OnMouseClick) {
        QCOMPARE(ic.openInputPanelReceived, true);
    }
    ic.openInputPanelReceived = false;

    // focus should not cause input panels to open or close
    edit->setFocus(false);
    edit->setFocus(true);
    edit->setFocus(false);
    edit->setFocus(true);
    edit->setFocus(false);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);
}

void tst_qsgtextedit::openInputPanelOnFocus()
{
    QSGView view(QUrl::fromLocalFile(SRCDIR "/data/openInputPanel.qml"));
    MyInputContext ic;
    // QSGCanvas won't set the Qt::WA_InputMethodEnabled flag unless a suitable item has focus
    // and QWidget won't allow an input context to be set when the flag is not set.
    view.setAttribute(Qt::WA_InputMethodEnabled, true);
    view.setInputContext(&ic);
    view.setAttribute(Qt::WA_InputMethodEnabled, false);
    view.show();

    qApp->setAutoSipEnabled(true);
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QSGTextEdit *edit = qobject_cast<QSGTextEdit *>(view.rootObject());
    QVERIFY(edit);
    QSignalSpy focusOnPressSpy(edit, SIGNAL(activeFocusOnPressChanged(bool)));

    QSGItemPrivate* pri = QSGItemPrivate::get(edit);
    QSGTextEditPrivate *editPrivate = static_cast<QSGTextEditPrivate*>(pri);
    editPrivate->showInputPanelOnFocus = true;

    // test default values
    QVERIFY(edit->focusOnPress());
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);

    // focus on press, input panel on focus
    QTest::mousePress(&view, Qt::LeftButton, 0, edit->mapToScene(QPointF(0,0)).toPoint());
    QApplication::processEvents();
    QVERIFY(edit->hasActiveFocus());
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;

    // no events on release
    QTest::mouseRelease(&view, Qt::LeftButton, 0, edit->mapToScene(QPointF(0,0)).toPoint());
    QCOMPARE(ic.openInputPanelReceived, false);
    ic.openInputPanelReceived = false;

    // if already focused, input panel can be opened on press
    QVERIFY(edit->hasActiveFocus());
    QTest::mousePress(&view, Qt::LeftButton, 0, edit->mapToScene(QPointF(0,0)).toPoint());
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;

    // input method should stay enabled if focus
    // is lost to an item that also accepts inputs
    QSGTextEdit anotherEdit;
    anotherEdit.setParentItem(view.rootObject());
    anotherEdit.setFocus(true);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;
    QCOMPARE(view.inputContext(), (QInputContext*)&ic);
    QVERIFY(view.testAttribute(Qt::WA_InputMethodEnabled));

    // input method should be disabled if focus
    // is lost to an item that doesn't accept inputs
    QSGItem item;
    item.setParentItem(view.rootObject());
    item.setFocus(true);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QVERIFY(view.inputContext() == 0);
    QVERIFY(!view.testAttribute(Qt::WA_InputMethodEnabled));

    // no automatic input panel events should
    // be sent if activeFocusOnPress is false
    edit->setFocusOnPress(false);
    QCOMPARE(focusOnPressSpy.count(),1);
    edit->setFocusOnPress(false);
    QCOMPARE(focusOnPressSpy.count(),1);
    edit->setFocus(false);
    edit->setFocus(true);
    QTest::mousePress(&view, Qt::LeftButton, 0, edit->mapToScene(QPointF(0,0)).toPoint());
    QTest::mouseRelease(&view, Qt::LeftButton, 0, edit->mapToScene(QPointF(0,0)).toPoint());
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);

    // one show input panel event should
    // be set when openSoftwareInputPanel is called
    edit->openSoftwareInputPanel();
    QCOMPARE(ic.openInputPanelReceived, true);
    QCOMPARE(ic.closeInputPanelReceived, false);
    ic.openInputPanelReceived = false;

    // one close input panel event should
    // be sent when closeSoftwareInputPanel is called
    edit->closeSoftwareInputPanel();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, true);
    ic.closeInputPanelReceived = false;

    // set activeFocusOnPress back to true
    edit->setFocusOnPress(true);
    QCOMPARE(focusOnPressSpy.count(),2);
    edit->setFocusOnPress(true);
    QCOMPARE(focusOnPressSpy.count(),2);
    edit->setFocus(false);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);
    ic.closeInputPanelReceived = false;

    // input panel should not re-open
    // if focus has already been set
    edit->setFocus(true);
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;
    edit->setFocus(true);
    QCOMPARE(ic.openInputPanelReceived, false);

    // input method should be disabled
    // if TextEdit loses focus
    edit->setFocus(false);
    QApplication::processEvents();
    QVERIFY(view.inputContext() == 0);
    QVERIFY(!view.testAttribute(Qt::WA_InputMethodEnabled));

    // input method should not be enabled
    // if TextEdit is read only.
    edit->setReadOnly(true);
    ic.openInputPanelReceived = false;
    edit->setFocus(true);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QVERIFY(view.inputContext() == 0);
    QVERIFY(!view.testAttribute(Qt::WA_InputMethodEnabled));
}

void tst_qsgtextedit::geometrySignals()
{
    QDeclarativeComponent component(&engine, SRCDIR "/data/geometrySignals.qml");
    QObject *o = component.create();
    QVERIFY(o);
    QCOMPARE(o->property("bindingWidth").toInt(), 400);
    QCOMPARE(o->property("bindingHeight").toInt(), 500);
    delete o;
}

void tst_qsgtextedit::pastingRichText_QTBUG_14003()
{
#ifndef QT_NO_CLIPBOARD
    QString componentStr = "import QtQuick 2.0\nTextEdit { textFormat: TextEdit.PlainText }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QSGTextEdit *obj = qobject_cast<QSGTextEdit*>(component.create());

    QTRY_VERIFY(obj != 0);
    QTRY_VERIFY(obj->textFormat() == QSGTextEdit::PlainText);

    QMimeData *mData = new QMimeData;
    mData->setHtml("<font color=\"red\">Hello</font>");
    QApplication::clipboard()->setMimeData(mData);

    obj->paste();
    QTRY_VERIFY(obj->text() == "");
    QTRY_VERIFY(obj->textFormat() == QSGTextEdit::PlainText);
#endif
}

void tst_qsgtextedit::implicitSize_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("wrap");
    QTest::newRow("plain") << "The quick red fox jumped over the lazy brown dog" << "TextEdit.NoWrap";
    QTest::newRow("richtext") << "<b>The quick red fox jumped over the lazy brown dog</b>" << "TextEdit.NoWrap";
    QTest::newRow("plain_wrap") << "The quick red fox jumped over the lazy brown dog" << "TextEdit.Wrap";
    QTest::newRow("richtext_wrap") << "<b>The quick red fox jumped over the lazy brown dog</b>" << "TextEdit.Wrap";
}

void tst_qsgtextedit::implicitSize()
{
    QFETCH(QString, text);
    QFETCH(QString, wrap);
    QString componentStr = "import QtQuick 2.0\nTextEdit { text: \"" + text + "\"; width: 50; wrapMode: " + wrap + " }";
    QDeclarativeComponent textComponent(&engine);
    textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QSGTextEdit *textObject = qobject_cast<QSGTextEdit*>(textComponent.create());

    QVERIFY(textObject->width() < textObject->implicitWidth());
    QVERIFY(textObject->height() == textObject->implicitHeight());

    textObject->resetWidth();
    QVERIFY(textObject->width() == textObject->implicitWidth());
    QVERIFY(textObject->height() == textObject->implicitHeight());
}

void tst_qsgtextedit::testQtQuick11Attributes()
{
    QFETCH(QString, code);
    QFETCH(QString, warning);
    QFETCH(QString, error);

    QDeclarativeEngine engine;
    QObject *obj;

    QDeclarativeComponent valid(&engine);
    valid.setData("import QtQuick 2.0; TextEdit { " + code.toUtf8() + " }", QUrl(""));
    obj = valid.create();
    QVERIFY(obj);
    QVERIFY(valid.errorString().isEmpty());
    delete obj;

    QDeclarativeComponent invalid(&engine);
    invalid.setData("import QtQuick 1.0; TextEdit { " + code.toUtf8() + " }", QUrl(""));
    QTest::ignoreMessage(QtWarningMsg, warning.toUtf8());
    obj = invalid.create();
    QCOMPARE(invalid.errorString(), error);
    delete obj;
}

void tst_qsgtextedit::testQtQuick11Attributes_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<QString>("warning");
    QTest::addColumn<QString>("error");

    QTest::newRow("canPaste") << "property bool foo: canPaste"
        << "<Unknown File>:1: ReferenceError: Can't find variable: canPaste"
        << "";

    QTest::newRow("lineCount") << "property int foo: lineCount"
        << "<Unknown File>:1: ReferenceError: Can't find variable: lineCount"
        << "";

    QTest::newRow("moveCursorSelection") << "Component.onCompleted: moveCursorSelection(0, TextEdit.SelectCharacters)"
        << "<Unknown File>:1: ReferenceError: Can't find variable: moveCursorSelection"
        << "";

    QTest::newRow("deselect") << "Component.onCompleted: deselect()"
        << "<Unknown File>:1: ReferenceError: Can't find variable: deselect"
        << "";

    QTest::newRow("onLinkActivated") << "onLinkActivated: {}"
        << "QDeclarativeComponent: Component is not ready"
        << ":1 \"TextEdit.onLinkActivated\" is not available in QtQuick 1.0.\n";
}

void tst_qsgtextedit::preeditMicroFocus()
{
    QString preeditText = "super";

    QSGView view(QUrl::fromLocalFile(SRCDIR "/data/inputMethodEvent.qml"));
    MyInputContext ic;
    view.setInputContext(&ic);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    QSGTextEdit *edit = qobject_cast<QSGTextEdit *>(view.rootObject());
    QVERIFY(edit);

    QRect currentRect;
    QRect previousRect = edit->inputMethodQuery(Qt::ImMicroFocus).toRect();

    // Verify that the micro focus rect is positioned the same for position 0 as
    // it would be if there was no preedit text.
    ic.updateReceived = false;
    ic.sendPreeditText(preeditText, 0);
    currentRect = edit->inputMethodQuery(Qt::ImMicroFocus).toRect();
    QCOMPARE(currentRect, previousRect);
#if defined(Q_WS_X11) || defined(Q_WS_QWS) || defined(Q_OS_SYMBIAN)
    QCOMPARE(ic.updateReceived, true);
#endif

    // Verify that the micro focus rect moves to the left as the cursor position
    // is incremented.
    for (int i = 1; i <= 5; ++i) {
        ic.updateReceived = false;
        ic.sendPreeditText(preeditText, i);
        currentRect = edit->inputMethodQuery(Qt::ImMicroFocus).toRect();
        QVERIFY(previousRect.left() < currentRect.left());
#if defined(Q_WS_X11) || defined(Q_WS_QWS) || defined(Q_OS_SYMBIAN)
        QCOMPARE(ic.updateReceived, true);
#endif
        previousRect = currentRect;
    }

    // Verify that if there is no preedit cursor then the micro focus rect is the
    // same as it would be if it were positioned at the end of the preedit text.
    ic.sendPreeditText(preeditText, 0);
    ic.updateReceived = false;
    ic.sendEvent(QInputMethodEvent(preeditText, QList<QInputMethodEvent::Attribute>()));
    currentRect = edit->inputMethodQuery(Qt::ImMicroFocus).toRect();
    QCOMPARE(currentRect, previousRect);
#if defined(Q_WS_X11) || defined(Q_WS_QWS) || defined(Q_OS_SYMBIAN)
    QCOMPARE(ic.updateReceived, true);
#endif
}

void tst_qsgtextedit::inputContextMouseHandler()
{
    QString text = "supercalifragisiticexpialidocious!";

    QSGView view(QUrl::fromLocalFile(SRCDIR "/data/inputContext.qml"));
    MyInputContext ic;
    view.setInputContext(&ic);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    QSGTextEdit *edit = qobject_cast<QSGTextEdit *>(view.rootObject());
    QVERIFY(edit);
    edit->setCursorPosition(12);

    QFontMetricsF fm(edit->font());
    const qreal y = fm.height() / 2;

    QPoint position2 = edit->mapToScene(QPointF(fm.width(text.mid(0, 2)), y)).toPoint();
    QPoint position8 = edit->mapToScene(QPointF(fm.width(text.mid(0, 8)), y)).toPoint();
    QPoint position20 = edit->mapToScene(QPointF(fm.width(text.mid(0, 20)), y)).toPoint();
    QPoint position27 = edit->mapToScene(QPointF(fm.width(text.mid(0, 27)), y)).toPoint();
    QPoint globalPosition2 = view.mapToGlobal(position2);
    QPoint globalposition8 = view.mapToGlobal(position8);
    QPoint globalposition20 = view.mapToGlobal(position20);
    QPoint globalposition27 = view.mapToGlobal(position27);

    ic.sendEvent(QInputMethodEvent(text.mid(12), QList<QInputMethodEvent::Attribute>()));

    QTest::mouseDClick(&view, Qt::LeftButton, Qt::NoModifier, position2);
    QCOMPARE(ic.eventType, QEvent::MouseButtonDblClick);
    QCOMPARE(ic.eventPosition, position2);
    QCOMPARE(ic.eventGlobalPosition, globalPosition2);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::NoModifier);
    QVERIFY(ic.cursor < 0);
    ic.eventType = QEvent::None;

    QTest::mousePress(&view, Qt::LeftButton, Qt::NoModifier, position2);
    QCOMPARE(ic.eventType, QEvent::MouseButtonPress);
    QCOMPARE(ic.eventPosition, position2);
    QCOMPARE(ic.eventGlobalPosition, globalPosition2);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::NoModifier);
    QVERIFY(ic.cursor < 0);
    ic.eventType = QEvent::None;

    {   QMouseEvent mv(QEvent::MouseMove, position8, globalposition8, Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(&view, &mv); }
    QCOMPARE(ic.eventType, QEvent::None);

    {   QMouseEvent mv(QEvent::MouseMove, position27, globalposition27, Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(&view, &mv); }
    QCOMPARE(ic.eventType, QEvent::MouseMove);
    QCOMPARE(ic.eventPosition, position27);
        QCOMPARE(ic.eventGlobalPosition, globalposition27);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::NoModifier);
    QVERIFY(ic.cursor >= 14 && ic.cursor <= 16);    // 15 is expected but some platforms may be off by one.
    ic.eventType = QEvent::None;

    QTest::mouseRelease(&view, Qt::LeftButton, Qt::NoModifier, position27);
    QCOMPARE(ic.eventType, QEvent::MouseButtonRelease);
    QCOMPARE(ic.eventPosition, position27);
    QCOMPARE(ic.eventGlobalPosition, globalposition27);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::NoModifier);
    QVERIFY(ic.cursor >= 14 && ic.cursor <= 16);
    ic.eventType = QEvent::None;

    // And in the other direction.
    QTest::mouseDClick(&view, Qt::LeftButton, Qt::ControlModifier, position27);
    QCOMPARE(ic.eventType, QEvent::MouseButtonDblClick);
    QCOMPARE(ic.eventPosition, position27);
    QCOMPARE(ic.eventGlobalPosition, globalposition27);
    QCOMPARE(ic.eventButton, Qt::LeftButton);
    QCOMPARE(ic.eventModifiers, Qt::ControlModifier);
    QVERIFY(ic.cursor >= 14 && ic.cursor <= 16);
    ic.eventType = QEvent::None;

    QTest::mousePress(&view, Qt::RightButton, Qt::ControlModifier, position27);
    QCOMPARE(ic.eventType, QEvent::MouseButtonPress);
    QCOMPARE(ic.eventPosition, position27);
    QCOMPARE(ic.eventGlobalPosition, globalposition27);
    QCOMPARE(ic.eventButton, Qt::RightButton);
    QCOMPARE(ic.eventModifiers, Qt::ControlModifier);
    QVERIFY(ic.cursor >= 14 && ic.cursor <= 16);
    ic.eventType = QEvent::None;

    {   QMouseEvent mv(QEvent::MouseMove, position20, globalposition20, Qt::RightButton, Qt::RightButton,Qt::ControlModifier);
        QApplication::sendEvent(&view, &mv); }
    QCOMPARE(ic.eventType, QEvent::MouseMove);
    QCOMPARE(ic.eventPosition, position20);
    QCOMPARE(ic.eventGlobalPosition, globalposition20);
    QCOMPARE(ic.eventButton, Qt::RightButton);
    QCOMPARE(ic.eventModifiers, Qt::ControlModifier);
    QVERIFY(ic.cursor >= 7 && ic.cursor <= 9);
    ic.eventType = QEvent::None;

    {   QMouseEvent mv(QEvent::MouseMove, position2, globalPosition2, Qt::RightButton, Qt::RightButton,Qt::ControlModifier);
        QApplication::sendEvent(&view, &mv); }
    QCOMPARE(ic.eventType, QEvent::None);

    QTest::mouseRelease(&view, Qt::RightButton, Qt::ControlModifier, position2);
    QCOMPARE(ic.eventType, QEvent::MouseButtonRelease);
    QCOMPARE(ic.eventPosition, position2);
    QCOMPARE(ic.eventGlobalPosition, globalPosition2);
    QCOMPARE(ic.eventButton, Qt::RightButton);
    QCOMPARE(ic.eventModifiers, Qt::ControlModifier);
    QVERIFY(ic.cursor < 0);
    ic.eventType = QEvent::None;
}

void tst_qsgtextedit::inputMethodComposing()
{
    QString text = "supercalifragisiticexpialidocious!";

    QSGView view(QUrl::fromLocalFile(SRCDIR "/data/inputContext.qml"));
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    QSGTextEdit *edit = qobject_cast<QSGTextEdit *>(view.rootObject());
    QVERIFY(edit);
    QSignalSpy spy(edit, SIGNAL(inputMethodComposingChanged()));
    edit->setCursorPosition(12);

    QCOMPARE(edit->isInputMethodComposing(), false);

    {
        QInputMethodEvent event(text.mid(3), QList<QInputMethodEvent::Attribute>());
        QApplication::sendEvent(&view, &event);
    }
    QCOMPARE(edit->isInputMethodComposing(), true);
    QCOMPARE(spy.count(), 1);

    {
        QInputMethodEvent event(text.mid(12), QList<QInputMethodEvent::Attribute>());
        QApplication::sendEvent(&view, &event);
    }
    QCOMPARE(spy.count(), 1);

    {
        QInputMethodEvent event;
        QApplication::sendEvent(&view, &event);
    }
    QCOMPARE(edit->isInputMethodComposing(), false);
    QCOMPARE(spy.count(), 2);
}

void tst_qsgtextedit::cursorRectangleSize()
{
    QSGView *canvas = new QSGView(QUrl::fromLocalFile(SRCDIR "/data/CursorRect.qml"));
    QVERIFY(canvas->rootObject() != 0);
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);

    QSGTextEdit *textEdit = qobject_cast<QSGTextEdit *>(canvas->rootObject());
    QVERIFY(textEdit != 0);
    textEdit->setFocus(Qt::OtherFocusReason);
    QRectF cursorRect = textEdit->positionToRectangle(textEdit->cursorPosition());
    QRectF microFocusFromScene = canvas->inputMethodQuery(Qt::ImMicroFocus).toRectF();
    QRectF microFocusFromApp= QApplication::focusWidget()->inputMethodQuery(Qt::ImMicroFocus).toRectF();

    QCOMPARE(microFocusFromScene.size(), cursorRect.size());
    QCOMPARE(microFocusFromApp.size(), cursorRect.size());

    delete canvas;
}

QTEST_MAIN(tst_qsgtextedit)

#include "tst_qsgtextedit.moc"

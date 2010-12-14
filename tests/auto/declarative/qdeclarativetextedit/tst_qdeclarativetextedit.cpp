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
#include <private/qdeclarativetextedit_p.h>
#include <private/qdeclarativetextedit_p_p.h>
#include <QFontMetrics>
#include <QDeclarativeView>
#include <QDir>
#include <QStyle>
#include <QInputContext>
#include <QClipboard>
#include <QMimeData>
#include <private/qapplication_p.h>
#include <private/qtextcontrol_p.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

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


class tst_qdeclarativetextedit : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativetextedit();

private slots:
    void text();
    void width();
    void wrap();
    void textFormat();
    void alignments();
    void alignments_data();

    // ### these tests may be trivial    
    void hAlign();
    void vAlign();
    void font();
    void color();
    void textMargin();
    void persistentSelection();
    void focusOnPress();
    void selection();
    void mouseSelection_data();
    void mouseSelection();
    void inputMethodHints();

    void cursorDelegate();
    void delegateLoading_data();
    void delegateLoading();
    void navigation();
    void readOnly();
    void copyAndPaste();
    void textInput();
    void openInputPanelOnClick();
    void openInputPanelOnFocus();
    void geometrySignals();
    void pastingRichText_QTBUG_14003();

private:
    void simulateKey(QDeclarativeView *, int key);
    QDeclarativeView *createView(const QString &filename);

    QStringList standard;
    QStringList richText;

    QStringList hAlignmentStrings;
    QStringList vAlignmentStrings;

    QList<Qt::Alignment> vAlignments;
    QList<Qt::Alignment> hAlignments;

    QStringList colorStrings;

    QDeclarativeEngine engine;
};

tst_qdeclarativetextedit::tst_qdeclarativetextedit()
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

void tst_qdeclarativetextedit::text()
{
    {
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData("import QtQuick 1.0\nTextEdit {  text: \"\"  }", QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), QString(""));
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nTextEdit { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), standard.at(i));
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nTextEdit { text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QString actual = textEditObject->text();
        QString expected = richText.at(i);
        actual.replace(QRegExp(".*<body[^>]*>"),"");
        actual.replace(QRegExp("(<[^>]*>)+"),"<>");
        expected.replace(QRegExp("(<[^>]*>)+"),"<>");
        QCOMPARE(actual.simplified(),expected.simplified());
    }
}

void tst_qdeclarativetextedit::width()
{
    // uses Font metrics to find the width for standard and document to find the width for rich
    {
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData("import QtQuick 1.0\nTextEdit {  text: \"\" }", QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 0.0);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetricsF fm(f);
        qreal metricWidth = fm.size(Qt::TextExpandTabs && Qt::TextShowMnemonic, standard.at(i)).width();
        metricWidth = ceil(metricWidth);

        QString componentStr = "import QtQuick 1.0\nTextEdit { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), qreal(metricWidth));
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QTextDocument document;
        document.setHtml(richText.at(i));
        document.setDocumentMargin(0);

        int documentWidth = ceil(document.idealWidth());

        QString componentStr = "import QtQuick 1.0\nTextEdit { text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), qreal(documentWidth));
    }
}

void tst_qdeclarativetextedit::wrap()
{
    // for specified width and wrap set true
    {
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData("import QtQuick 1.0\nTextEdit {  text: \"\"; wrapMode: TextEdit.WordWrap; width: 300 }", QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nTextEdit {  wrapMode: TextEdit.WordWrap; width: 300; text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nTextEdit {  wrapMode: TextEdit.WordWrap; width: 300; text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

}

void tst_qdeclarativetextedit::textFormat()
{
    {
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData("import QtQuick 1.0\nTextEdit { text: \"Hello\"; textFormat: Text.RichText }", QUrl::fromLocalFile(""));
        QDeclarativeTextEdit *textObject = qobject_cast<QDeclarativeTextEdit*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QDeclarativeTextEdit::RichText);
    }
    {
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData("import QtQuick 1.0\nTextEdit { text: \"<b>Hello</b>\"; textFormat: Text.PlainText }", QUrl::fromLocalFile(""));
        QDeclarativeTextEdit *textObject = qobject_cast<QDeclarativeTextEdit*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QDeclarativeTextEdit::PlainText);
    }
}

void tst_qdeclarativetextedit::alignments_data()
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


void tst_qdeclarativetextedit::alignments()
{
    QFETCH(int, hAlign);
    QFETCH(int, vAlign);
    QFETCH(QString, expectfile);

    QDeclarativeView *canvas = createView(SRCDIR "/data/alignments.qml");

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QObject *ob = canvas->rootObject();
    QVERIFY(ob != 0);
    ob->setProperty("horizontalAlignment",hAlign);
    ob->setProperty("verticalAlignment",vAlign);
    QTRY_COMPARE(ob->property("running").toBool(),false);
    QImage actual(canvas->width(), canvas->height(), QImage::Format_RGB32);
    actual.fill(qRgb(255,255,255));
    QPainter p(&actual);
    canvas->render(&p);

    expectfile = createExpectedFileIfNotFound(expectfile, actual);

    QImage expect(expectfile);

    QCOMPARE(actual,expect);
}


//the alignment tests may be trivial o.oa
void tst_qdeclarativetextedit::hAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 1.0\nTextEdit {  horizontalAlignment: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QDeclarativeComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->hAlign(), (int)hAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 1.0\nTextEdit {  horizontalAlignment: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QDeclarativeComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->hAlign(), (int)hAlignments.at(j));
        }
    }

}

void tst_qdeclarativetextedit::vAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 1.0\nTextEdit {  verticalAlignment: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QDeclarativeComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->vAlign(), (int)vAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 1.0\nTextEdit {  verticalAlignment: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QDeclarativeComponent texteditComponent(&engine);
            texteditComponent.setData(componentStr.toLatin1(), QUrl());
            QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->vAlign(), (int)vAlignments.at(j));
        }
    }

}

void tst_qdeclarativetextedit::font()
{
    //test size, then bold, then italic, then family
    { 
        QString componentStr = "import QtQuick 1.0\nTextEdit {  font.pointSize: 40; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().pointSize(), 40);
        QCOMPARE(textEditObject->font().bold(), false);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import QtQuick 1.0\nTextEdit {  font.bold: true; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().bold(), true);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import QtQuick 1.0\nTextEdit {  font.italic: true; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().italic(), true);
        QCOMPARE(textEditObject->font().bold(), false);
    }
 
    { 
        QString componentStr = "import QtQuick 1.0\nTextEdit {  font.family: \"Helvetica\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().family(), QString("Helvetica"));
        QCOMPARE(textEditObject->font().bold(), false);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import QtQuick 1.0\nTextEdit {  font.family: \"\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().family(), QString(""));
    }
}

void tst_qdeclarativetextedit::color()
{
    //test initial color
    {
        QString componentStr = "import QtQuick 1.0\nTextEdit { text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QDeclarativeTextEditPrivate *textEditPrivate = static_cast<QDeclarativeTextEditPrivate*>(QDeclarativeItemPrivate::get(textEditObject));

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
        QString componentStr = "import QtQuick 1.0\nTextEdit {  color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());
        //qDebug() << "textEditObject: " << textEditObject->color() << "vs. " << QColor(colorStrings.at(i));
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), QColor(colorStrings.at(i)));
    }

    //test selection
    for (int i = 0; i < colorStrings.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nTextEdit {  selectionColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->selectionColor(), QColor(colorStrings.at(i)));
    }

    //test selected text
    for (int i = 0; i < colorStrings.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nTextEdit {  selectedTextColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->selectedTextColor(), QColor(colorStrings.at(i)));
    }

    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import QtQuick 1.0\nTextEdit {  color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), testColor);
    }
}

void tst_qdeclarativetextedit::textMargin()
{
    for(qreal i=0; i<=10; i+=0.3){
        QString componentStr = "import QtQuick 1.0\nTextEdit {  textMargin: " + QString::number(i) + "; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->textMargin(), i);
    }
}

void tst_qdeclarativetextedit::persistentSelection()
{
    {
        QString componentStr = "import QtQuick 1.0\nTextEdit {  persistentSelection: true; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->persistentSelection(), true);
    }

    {
        QString componentStr = "import QtQuick 1.0\nTextEdit {  persistentSelection: false; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->persistentSelection(), false);
    }
}

void tst_qdeclarativetextedit::focusOnPress()
{
    {
        QString componentStr = "import QtQuick 1.0\nTextEdit {  activeFocusOnPress: true; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->focusOnPress(), true);
    }

    {
        QString componentStr = "import QtQuick 1.0\nTextEdit {  activeFocusOnPress: false; text: \"Hello World\" }";
        QDeclarativeComponent texteditComponent(&engine);
        texteditComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->focusOnPress(), false);
    }
}

void tst_qdeclarativetextedit::selection()
{
    QString testStr = standard[0];//TODO: What should happen for multiline/rich text?
    QString componentStr = "import QtQuick 1.0\nTextEdit {  text: \""+ testStr +"\"; }";
    QDeclarativeComponent texteditComponent(&engine);
    texteditComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit*>(texteditComponent.create());
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
}

void tst_qdeclarativetextedit::mouseSelection_data()
{
    QTest::addColumn<QString>("qmlfile");
    QTest::addColumn<bool>("expectSelection");

    // import installed
    QTest::newRow("on") << SRCDIR "/data/mouseselection_true.qml" << true;
    QTest::newRow("off") << SRCDIR "/data/mouseselection_false.qml" << false;
    QTest::newRow("default") << SRCDIR "/data/mouseselection_default.qml" << false;
}

void tst_qdeclarativetextedit::mouseSelection()
{
    QFETCH(QString, qmlfile);
    QFETCH(bool, expectSelection);

    QDeclarativeView *canvas = createView(qmlfile);

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit *>(canvas->rootObject());
    QVERIFY(textEditObject != 0);

    // press-and-drag-and-release from x1 to x2
    int x1 = 10;
    int x2 = 70;
    int y = textEditObject->height()/2;
    QTest::mousePress(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x1,y)));
    //QTest::mouseMove(canvas->viewport(), canvas->mapFromScene(QPoint(x2,y))); // doesn't work
    QMouseEvent mv(QEvent::MouseMove, canvas->mapFromScene(QPoint(x2,y)), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
    QApplication::sendEvent(canvas->viewport(), &mv);
    QTest::mouseRelease(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(x2,y)));
    QString str = textEditObject->selectedText();
    if (expectSelection)
        QVERIFY(str.length() > 3); // don't reallly care *what* was selected (and it's too sensitive to platform)
    else
        QVERIFY(str.isEmpty());
}

void tst_qdeclarativetextedit::inputMethodHints()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/inputmethodhints.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextEdit *textEditObject = qobject_cast<QDeclarativeTextEdit *>(canvas->rootObject());
    QVERIFY(textEditObject != 0);
    QVERIFY(textEditObject->inputMethodHints() & Qt::ImhNoPredictiveText);
    textEditObject->setInputMethodHints(Qt::ImhUppercaseOnly);
    QVERIFY(textEditObject->inputMethodHints() & Qt::ImhUppercaseOnly);
}

void tst_qdeclarativetextedit::cursorDelegate()
{
    QDeclarativeView* view = createView(SRCDIR "/data/cursorTest.qml");
    view->show();
    view->setFocus();
    QDeclarativeTextEdit *textEditObject = view->rootObject()->findChild<QDeclarativeTextEdit*>("textEditObject");
    QVERIFY(textEditObject != 0);
    QVERIFY(textEditObject->findChild<QDeclarativeItem*>("cursorInstance"));
    //Test Delegate gets created
    textEditObject->setFocus(true);
    QDeclarativeItem* delegateObject = textEditObject->findChild<QDeclarativeItem*>("cursorInstance");
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
    QVERIFY(!textEditObject->findChild<QDeclarativeItem*>("cursorInstance"));
}

void tst_qdeclarativetextedit::delegateLoading_data()
{
    QTest::addColumn<QString>("qmlfile");
    QTest::addColumn<QString>("error");

    // import installed
    QTest::newRow("pass") << "cursorHttpTestPass.qml" << "";
    QTest::newRow("fail1") << "cursorHttpTestFail1.qml" << "http://localhost:42332/FailItem.qml: Remote host closed the connection ";
    QTest::newRow("fail2") << "cursorHttpTestFail2.qml" << "http://localhost:42332/ErrItem.qml:4:5: Fungus is not a type ";
}

void tst_qdeclarativetextedit::delegateLoading()
{
    QFETCH(QString, qmlfile);
    QFETCH(QString, error);

    TestHTTPServer server(42332);
    server.serveDirectory(SRCDIR "/data/httpfail", TestHTTPServer::Disconnect);
    server.serveDirectory(SRCDIR "/data/httpslow", TestHTTPServer::Delay);
    server.serveDirectory(SRCDIR "/data/http");

    QDeclarativeView* view = new QDeclarativeView(0);

    view->setSource(QUrl(QLatin1String("http://localhost:42332/") + qmlfile));
    view->show();
    view->setFocus();

    if (!error.isEmpty()) {
        QTest::ignoreMessage(QtWarningMsg, error.toUtf8());
        QTRY_VERIFY(view->status()==QDeclarativeView::Error);
        QTRY_VERIFY(!view->rootObject()); // there is fail item inside this test
    } else {
        QTRY_VERIFY(view->rootObject());//Wait for loading to finish.
        QDeclarativeTextEdit *textEditObject = view->rootObject()->findChild<QDeclarativeTextEdit*>("textEditObject");
        //    view->rootObject()->dumpObjectTree();
        QVERIFY(textEditObject != 0);
        textEditObject->setFocus(true);
        QDeclarativeItem *delegate;
        delegate = view->rootObject()->findChild<QDeclarativeItem*>("delegateOkay");
        QVERIFY(delegate);
        delegate = view->rootObject()->findChild<QDeclarativeItem*>("delegateSlow");
        QVERIFY(delegate);

        delete delegate;
    }


    //A test should be added here with a component which is ready but component.create() returns null
    //Not sure how to accomplish this with QDeclarativeTextEdits cursor delegate
    //###This was only needed for code coverage, and could be a case of overzealous defensive programming
    //delegate = view->rootObject()->findChild<QDeclarativeItem*>("delegateErrorB");
    //QVERIFY(!delegate);

    delete view;
}

/*
TextEdit element should only handle left/right keys until the cursor reaches
the extent of the text, then they should ignore the keys.
*/
void tst_qdeclarativetextedit::navigation()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/navigation.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeItem *input = qobject_cast<QDeclarativeItem *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    QTRY_VERIFY(input->hasActiveFocus() == true);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == false);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == true);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == false);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == true);
}

void tst_qdeclarativetextedit::copyAndPaste() {
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

    QString componentStr = "import QtQuick 1.0\nTextEdit { text: \"Hello world!\" }";
    QDeclarativeComponent textEditComponent(&engine);
    textEditComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextEdit *textEdit = qobject_cast<QDeclarativeTextEdit*>(textEditComponent.create());
    QVERIFY(textEdit != 0);

    // copy and paste
    QCOMPARE(textEdit->text().length(), 12);
    textEdit->select(0, textEdit->text().length());;
    textEdit->copy();
    QCOMPARE(textEdit->selectedText(), QString("Hello world!"));
    QCOMPARE(textEdit->selectedText().length(), 12);
    textEdit->setCursorPosition(0);
    textEdit->paste();
    QCOMPARE(textEdit->text(), QString("Hello world!Hello world!"));
    QCOMPARE(textEdit->text().length(), 24);

    // QTBUG-12339
    // test that document and internal text attribute are in sync
    QDeclarativeItemPrivate* pri = QDeclarativeItemPrivate::get(textEdit);
    QDeclarativeTextEditPrivate *editPrivate = static_cast<QDeclarativeTextEditPrivate*>(pri);
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

void tst_qdeclarativetextedit::readOnly()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/readOnly.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextEdit *edit = qobject_cast<QDeclarativeTextEdit *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(edit != 0);
    QTRY_VERIFY(edit->hasActiveFocus() == true);
    QVERIFY(edit->isReadOnly() == true);
    QString initial = edit->text();
    for(int k=Qt::Key_0; k<=Qt::Key_Z; k++)
        simulateKey(canvas, k);
    simulateKey(canvas, Qt::Key_Return);
    simulateKey(canvas, Qt::Key_Space);
    simulateKey(canvas, Qt::Key_Escape);
    QCOMPARE(edit->text(), initial);
}

void tst_qdeclarativetextedit::simulateKey(QDeclarativeView *view, int key)
{
    QKeyEvent press(QKeyEvent::KeyPress, key, 0);
    QKeyEvent release(QKeyEvent::KeyRelease, key, 0);

    QApplication::sendEvent(view, &press);
    QApplication::sendEvent(view, &release);
}

QDeclarativeView *tst_qdeclarativetextedit::createView(const QString &filename)
{
    QDeclarativeView *canvas = new QDeclarativeView(0);

    canvas->setSource(QUrl::fromLocalFile(filename));
    return canvas;
}

class MyInputContext : public QInputContext
{
public:
    MyInputContext() : openInputPanelReceived(false), closeInputPanelReceived(false) {}
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
    bool openInputPanelReceived;
    bool closeInputPanelReceived;
};

void tst_qdeclarativetextedit::textInput()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QDeclarativeTextEdit edit;
    QDeclarativeItemPrivate* pri = QDeclarativeItemPrivate::get(&edit);
    QDeclarativeTextEditPrivate *editPrivate = static_cast<QDeclarativeTextEditPrivate*>(pri);
    edit.setPos(0, 0);
    scene.addItem(&edit);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    edit.setFocus(true);
    QVERIFY(edit.hasActiveFocus() == true);

    // test that input method event is committed
    QInputMethodEvent event;
    event.setCommitString( "Hello world!", 0, 0);
    QApplication::sendEvent(&view, &event);
    QCOMPARE(edit.text(), QString("Hello world!"));

    // QTBUG-12339
    // test that document and internal text attribute are in sync
    QCOMPARE(editPrivate->text, QString("Hello world!"));
}

void tst_qdeclarativetextedit::openInputPanelOnClick()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextEdit edit;
    QSignalSpy focusOnPressSpy(&edit, SIGNAL(activeFocusOnPressChanged(bool)));
    edit.setText("Hello world");
    edit.setPos(0, 0);
    scene.addItem(&edit);
    view.show();
    qApp->setAutoSipEnabled(true);
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QDeclarativeItemPrivate* pri = QDeclarativeItemPrivate::get(&edit);
    QDeclarativeTextEditPrivate *editPrivate = static_cast<QDeclarativeTextEditPrivate*>(pri);

    // input panel on click
    editPrivate->showInputPanelOnFocus = false;

    QStyle::RequestSoftwareInputPanel behavior = QStyle::RequestSoftwareInputPanel(
            view.style()->styleHint(QStyle::SH_RequestSoftwareInputPanel));
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(edit.scenePos()));
    QApplication::processEvents();
    if (behavior == QStyle::RSIP_OnMouseClickAndAlreadyFocused) {
        QCOMPARE(ic.openInputPanelReceived, false);
        QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(edit.scenePos()));
        QApplication::processEvents();
        QCOMPARE(ic.openInputPanelReceived, true);
    } else if (behavior == QStyle::RSIP_OnMouseClick) {
        QCOMPARE(ic.openInputPanelReceived, true);
    }
    ic.openInputPanelReceived = false;

    // focus should not cause input panels to open or close
    edit.setFocus(false);
    edit.setFocus(true);
    edit.setFocus(false);
    edit.setFocus(true);
    edit.setFocus(false);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);
}

void tst_qdeclarativetextedit::openInputPanelOnFocus()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextEdit edit;
    QSignalSpy focusOnPressSpy(&edit, SIGNAL(activeFocusOnPressChanged(bool)));
    edit.setText("Hello world");
    edit.setPos(0, 0);
    scene.addItem(&edit);
    view.show();
    qApp->setAutoSipEnabled(true);
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QDeclarativeItemPrivate* pri = QDeclarativeItemPrivate::get(&edit);
    QDeclarativeTextEditPrivate *editPrivate = static_cast<QDeclarativeTextEditPrivate*>(pri);
    editPrivate->showInputPanelOnFocus = true;

    // test default values
    QVERIFY(edit.focusOnPress());
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);

    // focus on press, input panel on focus
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(edit.scenePos()));
    QApplication::processEvents();
    QVERIFY(edit.hasActiveFocus());
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;

    // no events on release
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(edit.scenePos()));
    QCOMPARE(ic.openInputPanelReceived, false);
    ic.openInputPanelReceived = false;

    // if already focused, input panel can be opened on press
    QVERIFY(edit.hasActiveFocus());
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(edit.scenePos()));
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;

    // input method should stay enabled if focus
    // is lost to an item that also accepts inputs
    QDeclarativeTextEdit anotherEdit;
    scene.addItem(&anotherEdit);
    anotherEdit.setFocus(true);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;
    QCOMPARE(view.inputContext(), (QInputContext*)&ic);
    QVERIFY(view.testAttribute(Qt::WA_InputMethodEnabled));

    // input method should be disabled if focus
    // is lost to an item that doesn't accept inputs
    QDeclarativeItem item;
    scene.addItem(&item);
    item.setFocus(true);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QVERIFY(view.inputContext() == 0);
    QVERIFY(!view.testAttribute(Qt::WA_InputMethodEnabled));

    // no automatic input panel events should
    // be sent if activeFocusOnPress is false
    edit.setFocusOnPress(false);
    QCOMPARE(focusOnPressSpy.count(),1);
    edit.setFocusOnPress(false);
    QCOMPARE(focusOnPressSpy.count(),1);
    edit.setFocus(false);
    edit.setFocus(true);
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(edit.scenePos()));
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(edit.scenePos()));
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);

    // one show input panel event should
    // be set when openSoftwareInputPanel is called
    edit.openSoftwareInputPanel();
    QCOMPARE(ic.openInputPanelReceived, true);
    QCOMPARE(ic.closeInputPanelReceived, false);
    ic.openInputPanelReceived = false;

    // one close input panel event should
    // be sent when closeSoftwareInputPanel is called
    edit.closeSoftwareInputPanel();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, true);
    ic.closeInputPanelReceived = false;

    // set activeFocusOnPress back to true
    edit.setFocusOnPress(true);
    QCOMPARE(focusOnPressSpy.count(),2);
    edit.setFocusOnPress(true);
    QCOMPARE(focusOnPressSpy.count(),2);
    edit.setFocus(false);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);
    ic.closeInputPanelReceived = false;

    // input panel should not re-open
    // if focus has already been set
    edit.setFocus(true);
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;
    edit.setFocus(true);
    QCOMPARE(ic.openInputPanelReceived, false);

    // input method should be disabled
    // if TextEdit loses focus
    edit.setFocus(false);
    QApplication::processEvents();
    QVERIFY(view.inputContext() == 0);
    QVERIFY(!view.testAttribute(Qt::WA_InputMethodEnabled));
}

void tst_qdeclarativetextedit::geometrySignals()
{
    QDeclarativeComponent component(&engine, SRCDIR "/data/geometrySignals.qml");
    QObject *o = component.create();
    QVERIFY(o);
    QCOMPARE(o->property("bindingWidth").toInt(), 400);
    QCOMPARE(o->property("bindingHeight").toInt(), 500);
    delete o;
}

void tst_qdeclarativetextedit::pastingRichText_QTBUG_14003()
{
#ifndef QT_NO_CLIPBOARD
    QString componentStr = "import QtQuick 1.0\nTextEdit { textFormat: TextEdit.PlainText }";
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeTextEdit *obj = qobject_cast<QDeclarativeTextEdit*>(component.create());

    QTRY_VERIFY(obj != 0);
    QTRY_VERIFY(obj->textFormat() == QDeclarativeTextEdit::PlainText);

    QMimeData *mData = new QMimeData;
    mData->setHtml("<font color=\"red\">Hello</font>");
    QApplication::clipboard()->setMimeData(mData);

    obj->paste();
    QTRY_VERIFY(obj->text() == "");
    QTRY_VERIFY(obj->textFormat() == QDeclarativeTextEdit::PlainText);
#endif
}

QTEST_MAIN(tst_qdeclarativetextedit)

#include "tst_qdeclarativetextedit.moc"

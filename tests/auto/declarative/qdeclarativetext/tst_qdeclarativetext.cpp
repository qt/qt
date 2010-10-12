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
#include <QTextDocument>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativetext_p.h>
#include <private/qdeclarativevaluetype_p.h>
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <qmath.h>
#include <QDeclarativeView>
#include <private/qapplication_p.h>

#include "../../../shared/util.h"
#include "testhttpserver.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativetext : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativetext();

private slots:
    void text();
    void width();
    void wrap();
    void elide();
    void textFormat();

    void alignments_data();
    void alignments();

    void embeddedImages_data();
    void embeddedImages();

    // ### these tests may be trivial    
    void horizontalAlignment();
    void verticalAlignment();
    void font();
    void style();
    void color();
    void smooth();

    // QDeclarativeFontValueType
    void weight();
    void underline();
    void overline();
    void strikeout();
    void capitalization();
    void letterSpacing();
    void wordSpacing();

    void clickLink();

    void QTBUG_12291();

private:
    QStringList standard;
    QStringList richText;

    QStringList horizontalAlignmentmentStrings;
    QStringList verticalAlignmentmentStrings;

    QList<Qt::Alignment> verticalAlignmentments;
    QList<Qt::Alignment> horizontalAlignmentments;

    QStringList styleStrings;
    QList<QDeclarativeText::TextStyle> styles;

    QStringList colorStrings;

    QDeclarativeEngine engine;

    QDeclarativeView *createView(const QString &filename);
};

tst_qdeclarativetext::tst_qdeclarativetext()
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

    styles << QDeclarativeText::Normal
            << QDeclarativeText::Outline
            << QDeclarativeText::Raised
            << QDeclarativeText::Sunken;

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

QDeclarativeView *tst_qdeclarativetext::createView(const QString &filename)
{
    QDeclarativeView *canvas = new QDeclarativeView(0);

    canvas->setSource(QUrl::fromLocalFile(filename));
    return canvas;
}

void tst_qdeclarativetext::text()
{
    {
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData("import QtQuick 1.0\nText { text: \"\" }", QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->text(), QString(""));
        QVERIFY(textObject->width() == 0);

        delete textObject;
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));

        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->text(), standard.at(i));
        QVERIFY(textObject->width() > 0);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QString expected = richText.at(i);
        QCOMPARE(textObject->text(), expected.replace("\\\"", "\""));
        QVERIFY(textObject->width() > 0);
    }
}

void tst_qdeclarativetext::width()
{
    // uses Font metrics to find the width for standard and document to find the width for rich
    {
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData("import QtQuick 1.0\nText { text: \"\" }", QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), 0.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QVERIFY(!Qt::mightBeRichText(standard.at(i))); // self-test

        QFont f;
        QFontMetricsF fm(f);
        qreal metricWidth = fm.size(Qt::TextExpandTabs && Qt::TextShowMnemonic, standard.at(i)).width();
        metricWidth = qCeil(metricWidth);

        QString componentStr = "import QtQuick 1.0\nText { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->boundingRect().width() > 0);
        QCOMPARE(textObject->width(), qreal(metricWidth));
        QVERIFY(textObject->textFormat() == QDeclarativeText::AutoText); // setting text doesn't change format
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QVERIFY(Qt::mightBeRichText(richText.at(i))); // self-test

        QTextDocument document;
        document.setHtml(richText.at(i));
        document.setDocumentMargin(0);

        int documentWidth = document.idealWidth();

        QString componentStr = "import QtQuick 1.0\nText { text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), qreal(documentWidth));
        QVERIFY(textObject->textFormat() == QDeclarativeText::AutoText); // setting text doesn't change format
    }
}

void tst_qdeclarativetext::wrap()
{
    int textHeight = 0;
    // for specified width and wrap set true
    {
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData("import QtQuick 1.0\nText { text: \"Hello\"; wrapMode: Text.WordWrap; width: 300 }", QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());
        textHeight = textObject->height();

        QVERIFY(textObject != 0);
        QVERIFY(textObject->wrapMode() == QDeclarativeText::WordWrap);
        QCOMPARE(textObject->width(), 300.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nText { wrapMode: Text.WordWrap; width: 30; text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), 30.);
        QVERIFY(textObject->height() > textHeight);

        int oldHeight = textObject->height();
        textObject->setWidth(100);
        QVERIFY(textObject->height() < oldHeight);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nText { wrapMode: Text.WordWrap; width: 30; text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), 30.);
        QVERIFY(textObject->height() > textHeight);

        qreal oldHeight = textObject->height();
        textObject->setWidth(100);
        QVERIFY(textObject->height() < oldHeight);
    }

    // richtext again with a fixed height
    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import QtQuick 1.0\nText { wrapMode: Text.WordWrap; width: 30; height: 50; text: \"" + richText.at(i) + "\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->width(), 30.);
        QVERIFY(textObject->implicitHeight() > textHeight);

        qreal oldHeight = textObject->implicitHeight();
        textObject->setWidth(100);
        QVERIFY(textObject->implicitHeight() < oldHeight);
    }
}

void tst_qdeclarativetext::elide()
{
    for (QDeclarativeText::TextElideMode m = QDeclarativeText::ElideLeft; m<=QDeclarativeText::ElideNone; m=QDeclarativeText::TextElideMode(int(m)+1)) {
        const char* elidename[]={"ElideLeft", "ElideRight", "ElideMiddle", "ElideNone"};
        QString elide = "elide: Text." + QString(elidename[int(m)]) + ";";

        // XXX Poor coverage.

        {
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(("import QtQuick 1.0\nText { text: \"\"; "+elide+" width: 100 }").toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

            QCOMPARE(textObject->elideMode(), m);
            QCOMPARE(textObject->width(), 100.);
        }

        for (int i = 0; i < standard.size(); i++)
        {
            QString componentStr = "import QtQuick 1.0\nText { "+elide+" width: 100; text: \"" + standard.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

            QCOMPARE(textObject->elideMode(), m);
            QCOMPARE(textObject->width(), 100.);
        }

        // richtext - does nothing
        for (int i = 0; i < richText.size(); i++)
        {
            QString componentStr = "import QtQuick 1.0\nText { "+elide+" width: 100; text: \"" + richText.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

            QCOMPARE(textObject->elideMode(), m);
            QCOMPARE(textObject->width(), 100.);
        }
    }
}

void tst_qdeclarativetext::textFormat()
{
    {
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData("import QtQuick 1.0\nText { text: \"Hello\"; textFormat: Text.RichText }", QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QDeclarativeText::RichText);
    }
    {
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData("import QtQuick 1.0\nText { text: \"<b>Hello</b>\"; textFormat: Text.PlainText }", QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QVERIFY(textObject->textFormat() == QDeclarativeText::PlainText);
    }
}


void tst_qdeclarativetext::alignments_data()
{
    QTest::addColumn<int>("hAlign");
    QTest::addColumn<int>("vAlign");
    QTest::addColumn<QString>("expectfile");

    QTest::newRow("LT") << int(Qt::AlignLeft) << int(Qt::AlignTop) << SRCDIR "/data/alignments_lt.png";
    QTest::newRow("RT") << int(Qt::AlignRight) << int(Qt::AlignTop) << SRCDIR "/data/alignments_rt.png";
    QTest::newRow("CT") << int(Qt::AlignHCenter) << int(Qt::AlignTop) << SRCDIR "/data/alignments_ct.png";

    QTest::newRow("LB") << int(Qt::AlignLeft) << int(Qt::AlignBottom) << SRCDIR "/data/alignments_lb.png";
    QTest::newRow("RB") << int(Qt::AlignRight) << int(Qt::AlignBottom) << SRCDIR "/data/alignments_rb.png";
    QTest::newRow("CB") << int(Qt::AlignHCenter) << int(Qt::AlignBottom) << SRCDIR "/data/alignments_cb.png";

    QTest::newRow("LC") << int(Qt::AlignLeft) << int(Qt::AlignVCenter) << SRCDIR "/data/alignments_lc.png";
    QTest::newRow("RC") << int(Qt::AlignRight) << int(Qt::AlignVCenter) << SRCDIR "/data/alignments_rc.png";
    QTest::newRow("CC") << int(Qt::AlignHCenter) << int(Qt::AlignVCenter) << SRCDIR "/data/alignments_cc.png";
}


void tst_qdeclarativetext::alignments()
{
    QFETCH(int, hAlign);
    QFETCH(int, vAlign);
    QFETCH(QString, expectfile);

#ifdef Q_WS_X11
    // Font-specific, but not likely platform-specific, so only test on one platform
    QFont fn;
    fn.setRawName("-misc-fixed-medium-r-*-*-8-*-*-*-*-*-*-*");
    QApplication::setFont(fn);
#endif

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

    QImage expect(expectfile);

#ifdef Q_WS_X11
    // Font-specific, but not likely platform-specific, so only test on one platform
    if (QApplicationPrivate::graphics_system_name == "raster" || QApplicationPrivate::graphics_system_name == "") {
        QCOMPARE(actual,expect);
    }
#endif
}

//the alignment tests may be trivial o.oa
void tst_qdeclarativetext::horizontalAlignment()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < horizontalAlignmentmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 1.0\nText { horizontalAlignment: \"" + horizontalAlignmentmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

            QCOMPARE((int)textObject->hAlign(), (int)horizontalAlignmentments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < horizontalAlignmentmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 1.0\nText { horizontalAlignment: \"" + horizontalAlignmentmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

            QCOMPARE((int)textObject->hAlign(), (int)horizontalAlignmentments.at(j));
        }
    }

}

void tst_qdeclarativetext::verticalAlignment()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < verticalAlignmentmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 1.0\nText { verticalAlignment: \"" + verticalAlignmentmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

            QVERIFY(textObject != 0);
            QCOMPARE((int)textObject->vAlign(), (int)verticalAlignmentments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < verticalAlignmentmentStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 1.0\nText { verticalAlignment: \"" + verticalAlignmentmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

            QVERIFY(textObject != 0);
            QCOMPARE((int)textObject->vAlign(), (int)verticalAlignmentments.at(j));
        }
    }

}

void tst_qdeclarativetext::font()
{
    //test size, then bold, then italic, then family
    {
        QString componentStr = "import QtQuick 1.0\nText { font.pointSize: 40; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE(textObject->font().pointSize(), 40);
        QCOMPARE(textObject->font().bold(), false);
        QCOMPARE(textObject->font().italic(), false);
    }

    {
        QString componentStr = "import QtQuick 1.0\nText { font.pixelSize: 40; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE(textObject->font().pixelSize(), 40);
        QCOMPARE(textObject->font().bold(), false);
        QCOMPARE(textObject->font().italic(), false);
    }

    { 
        QString componentStr = "import QtQuick 1.0\nText { font.bold: true; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE(textObject->font().bold(), true);
        QCOMPARE(textObject->font().italic(), false);
    }

    { 
        QString componentStr = "import QtQuick 1.0\nText { font.italic: true; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE(textObject->font().italic(), true);
        QCOMPARE(textObject->font().bold(), false);
    }

    { 
        QString componentStr = "import QtQuick 1.0\nText { font.family: \"Helvetica\"; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE(textObject->font().family(), QString("Helvetica"));
        QCOMPARE(textObject->font().bold(), false);
        QCOMPARE(textObject->font().italic(), false);
    }

    { 
        QString componentStr = "import QtQuick 1.0\nText { font.family: \"\"; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE(textObject->font().family(), QString(""));
    }
}

void tst_qdeclarativetext::style()
{
    //test style
    for (int i = 0; i < styles.size(); i++)
    { 
        QString componentStr = "import QtQuick 1.0\nText { style: \"" + styleStrings.at(i) + "\"; styleColor: \"white\"; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE((int)textObject->style(), (int)styles.at(i));
        QCOMPARE(textObject->styleColor(), QColor("white"));
    }
    QString componentStr = "import QtQuick 1.0\nText { text: \"Hello World\" }";
    QDeclarativeComponent textComponent(&engine);
    textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
    QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

    QRectF brPre = textObject->boundingRect();
    textObject->setStyle(QDeclarativeText::Outline);
    QRectF brPost = textObject->boundingRect();

    QVERIFY(brPre.width() < brPost.width());
    QVERIFY(brPre.height() < brPost.height());
}

void tst_qdeclarativetext::color()
{
    //test style
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import QtQuick 1.0\nText { color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE(textObject->color(), QColor(colorStrings.at(i)));
        QCOMPARE(textObject->styleColor(), QColor());
    }

    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import QtQuick 1.0\nText { styleColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE(textObject->styleColor(), QColor(colorStrings.at(i)));
        // default color to black?
        QCOMPARE(textObject->color(), QColor("black"));
    }
    
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        for (int j = 0; j < colorStrings.size(); j++)
        {
            QString componentStr = "import QtQuick 1.0\nText { color: \"" + colorStrings.at(i) + "\"; styleColor: \"" + colorStrings.at(j) + "\"; text: \"Hello World\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

            QCOMPARE(textObject->color(), QColor(colorStrings.at(i)));
            QCOMPARE(textObject->styleColor(), QColor(colorStrings.at(j)));
        }
    }
    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import QtQuick 1.0\nText { color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QCOMPARE(textObject->color(), testColor);
    }
}

void tst_qdeclarativetext::smooth()
{
    for (int i = 0; i < standard.size(); i++)
    {
        {
            QString componentStr = "import QtQuick 1.0\nText { smooth: true; text: \"" + standard.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());
            QCOMPARE(textObject->smooth(), true);
        }
        {
            QString componentStr = "import QtQuick 1.0\nText { text: \"" + standard.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());
            QCOMPARE(textObject->smooth(), false);
        }
    }
    for (int i = 0; i < richText.size(); i++)
    {
        {
            QString componentStr = "import QtQuick 1.0\nText { smooth: true; text: \"" + richText.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());
            QCOMPARE(textObject->smooth(), true);
        }
        {
            QString componentStr = "import QtQuick 1.0\nText { text: \"" + richText.at(i) + "\" }";
            QDeclarativeComponent textComponent(&engine);
            textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
            QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());
            QCOMPARE(textObject->smooth(), false);
        }
    }
}

void tst_qdeclarativetext::weight()
{
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE((int)textObject->font().weight(), (int)QDeclarativeFontValueType::Normal);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { font.weight: \"Bold\"; text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE((int)textObject->font().weight(), (int)QDeclarativeFontValueType::Bold);
    }
}

void tst_qdeclarativetext::underline()
{
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().underline(), false);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { font.underline: true; text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().underline(), true);
    }
}

void tst_qdeclarativetext::overline()
{
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().overline(), false);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { font.overline: true; text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().overline(), true);
    }
}

void tst_qdeclarativetext::strikeout()
{
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().strikeOut(), false);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { font.strikeout: true; text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().strikeOut(), true);
    }
}

void tst_qdeclarativetext::capitalization()
{
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE((int)textObject->font().capitalization(), (int)QDeclarativeFontValueType::MixedCase);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\"; font.capitalization: \"AllUppercase\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE((int)textObject->font().capitalization(), (int)QDeclarativeFontValueType::AllUppercase);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\"; font.capitalization: \"AllLowercase\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE((int)textObject->font().capitalization(), (int)QDeclarativeFontValueType::AllLowercase);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\"; font.capitalization: \"SmallCaps\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE((int)textObject->font().capitalization(), (int)QDeclarativeFontValueType::SmallCaps);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\"; font.capitalization: \"Capitalize\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE((int)textObject->font().capitalization(), (int)QDeclarativeFontValueType::Capitalize);
    }
}

void tst_qdeclarativetext::letterSpacing()
{
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().letterSpacing(), 0.0);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\"; font.letterSpacing: -2 }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().letterSpacing(), -2.);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\"; font.letterSpacing: 3 }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().letterSpacing(), 3.);
    }
}

void tst_qdeclarativetext::wordSpacing()
{
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().wordSpacing(), 0.0);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\"; font.wordSpacing: -50 }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().wordSpacing(), -50.);
    }
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"Hello world!\"; font.wordSpacing: 200 }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);
        QCOMPARE(textObject->font().wordSpacing(), 200.);
    }
}

void tst_qdeclarativetext::QTBUG_12291()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/rotated.qml");

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QObject *ob = canvas->rootObject();
    QVERIFY(ob != 0);

    QDeclarativeText *text = ob->findChild<QDeclarativeText*>("text");
    QVERIFY(text);
    QVERIFY(text->boundingRect().isValid());
}

class EventSender : public QGraphicsItem
{
public:
    void sendEvent(QEvent *event) { sceneEvent(event); }
};

class LinkTest : public QObject
{
    Q_OBJECT
public:
    LinkTest() {}

    QString link;

public slots:
    void linkClicked(QString l) { link = l; }
};

void tst_qdeclarativetext::clickLink()
{
    {
        QString componentStr = "import QtQuick 1.0\nText { text: \"<a href=\\\"http://qt.nokia.com\\\">Hello world!</a>\" }";
        QDeclarativeComponent textComponent(&engine);
        textComponent.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));
        QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

        QVERIFY(textObject != 0);

        LinkTest test;
        QObject::connect(textObject, SIGNAL(linkActivated(QString)), &test, SLOT(linkClicked(QString)));

        {
            QGraphicsSceneMouseEvent me(QEvent::GraphicsSceneMousePress);
            me.setPos(QPointF(textObject->x()/2, textObject->y()/2));
            me.setButton(Qt::LeftButton);
            static_cast<EventSender*>(static_cast<QGraphicsItem*>(textObject))->sendEvent(&me);
        }

        {
            QGraphicsSceneMouseEvent me(QEvent::GraphicsSceneMouseRelease);
            me.setPos(QPointF(textObject->x()/2, textObject->y()/2));
            me.setButton(Qt::LeftButton);
            static_cast<EventSender*>(static_cast<QGraphicsItem*>(textObject))->sendEvent(&me);
        }

        QCOMPARE(test.link, QLatin1String("http://qt.nokia.com"));
    }
}

void tst_qdeclarativetext::embeddedImages_data()
{
    QTest::addColumn<QUrl>("qmlfile");
    QTest::addColumn<QString>("error");
    QTest::newRow("local") << QUrl::fromLocalFile(SRCDIR "/data/embeddedImagesLocal.qml") << "";
    QTest::newRow("local-error") << QUrl::fromLocalFile(SRCDIR "/data/embeddedImagesLocalError.qml")
        << QUrl::fromLocalFile(SRCDIR "/data/embeddedImagesLocalError.qml").toString()+":3:1: QML Text: Cannot open: " + QUrl::fromLocalFile(SRCDIR "/data/http/notexists.png").toString();
    QTest::newRow("remote") << QUrl::fromLocalFile(SRCDIR "/data/embeddedImagesRemote.qml") << "";
    QTest::newRow("remote-error") << QUrl::fromLocalFile(SRCDIR "/data/embeddedImagesRemoteError.qml")
        << QUrl::fromLocalFile(SRCDIR "/data/embeddedImagesRemoteError.qml").toString()+":3:1: QML Text: Error downloading http://127.0.0.1:14453/notexists.png - server replied: Not found";
}

void tst_qdeclarativetext::embeddedImages()
{
    // Tests QTBUG-9900

    QFETCH(QUrl, qmlfile);
    QFETCH(QString, error);

    TestHTTPServer server(14453);
    server.serveDirectory(SRCDIR "/data/http");

    if (!error.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, error.toLatin1());
    
    QDeclarativeComponent textComponent(&engine, qmlfile);
    QDeclarativeText *textObject = qobject_cast<QDeclarativeText*>(textComponent.create());

    QVERIFY(textObject != 0);

    QTRY_COMPARE(textObject->resourcesLoading(), 0);

    QPixmap pm(SRCDIR "/data/http/exists.png");
    if (error.isEmpty()) {
        QCOMPARE(textObject->width(), double(pm.width()));
        QCOMPARE(textObject->height(), double(pm.height()));
    } else {
        QVERIFY(16 != pm.width()); // check test is effective
        QCOMPARE(textObject->width(), 16.0); // default size of QTextDocument broken image icon
        QCOMPARE(textObject->height(), 16.0);
    }
}

QTEST_MAIN(tst_qdeclarativetext)

#include "tst_qdeclarativetext.moc"

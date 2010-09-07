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
#include <qtest.h>
#include <QtTest/QSignalSpy>
#include "../../../shared/util.h"
#include <QtDeclarative/qdeclarativeengine.h>
#include <QFile>
#include <QtDeclarative/qdeclarativeview.h>
#include <private/qdeclarativetextinput_p.h>
#include <private/qdeclarativetextinput_p_p.h>
#include <QDebug>
#include <QDir>
#include <QStyle>
#include <QInputContext>
#include <private/qapplication_p.h>

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

class tst_qdeclarativetextinput : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativetextinput();

private slots:

    void text();
    void width();
    void font();
    void color();
    void selection();

    void horizontalAlignment_data();
    void horizontalAlignment();

    void positionAt();

    void maxLength();
    void masks();
    void validators();
    void inputMethods();

    void cursorDelegate();
    void navigation();
    void copyAndPaste();
    void readOnly();

    void openInputPanelOnClick();
    void openInputPanelOnFocus();
    void setHAlignClearCache();
    void focusOutClearSelection();

    void echoMode();
    void geometrySignals();
private:
    void simulateKey(QDeclarativeView *, int key);
    QDeclarativeView *createView(const QString &filename);

    QDeclarativeEngine engine;
    QStringList standard;
    QStringList colorStrings;
};

tst_qdeclarativetextinput::tst_qdeclarativetextinput()
{
    standard << "the quick brown fox jumped over the lazy dog"
        << "It's supercalifragisiticexpialidocious!"
        << "Hello, world!";

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
}

void tst_qdeclarativetextinput::text()
{
    {
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData("import Qt 4.7\nTextInput {  text: \"\"  }", QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->text(), QString(""));

        delete textinputObject;
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import Qt 4.7\nTextInput { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->text(), standard.at(i));

        delete textinputObject;
    }

}

void tst_qdeclarativetextinput::width()
{
    // uses Font metrics to find the width for standard
    {
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData("import Qt 4.7\nTextInput {  text: \"\" }", QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->width(), 0.0);

        delete textinputObject;
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetricsF fm(f);
        qreal metricWidth = fm.width(standard.at(i));

        QString componentStr = "import Qt 4.7\nTextInput { text: \"" + standard.at(i) + "\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        int delta = abs(int(textinputObject->width()) - metricWidth);
        QVERIFY(delta <= 3.0); // As best as we can hope for cross-platform.

        delete textinputObject;
    }
}

void tst_qdeclarativetextinput::font()
{
    //test size, then bold, then italic, then family
    { 
        QString componentStr = "import Qt 4.7\nTextInput {  font.pointSize: 40; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().pointSize(), 40);
        QCOMPARE(textinputObject->font().bold(), false);
        QCOMPARE(textinputObject->font().italic(), false);

        delete textinputObject;
    }

    { 
        QString componentStr = "import Qt 4.7\nTextInput {  font.bold: true; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().bold(), true);
        QCOMPARE(textinputObject->font().italic(), false);

        delete textinputObject;
    }

    { 
        QString componentStr = "import Qt 4.7\nTextInput {  font.italic: true; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().italic(), true);
        QCOMPARE(textinputObject->font().bold(), false);

        delete textinputObject;
    }
 
    { 
        QString componentStr = "import Qt 4.7\nTextInput {  font.family: \"Helvetica\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().family(), QString("Helvetica"));
        QCOMPARE(textinputObject->font().bold(), false);
        QCOMPARE(textinputObject->font().italic(), false);

        delete textinputObject;
    }

    { 
        QString componentStr = "import Qt 4.7\nTextInput {  font.family: \"\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().family(), QString(""));

        delete textinputObject;
    }
}

void tst_qdeclarativetextinput::color()
{
    //test color
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import Qt 4.7\nTextInput {  color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->color(), QColor(colorStrings.at(i)));

        delete textinputObject;
    }

    //test selection color
    for (int i = 0; i < colorStrings.size(); i++)
    {
        QString componentStr = "import Qt 4.7\nTextInput {  selectionColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->selectionColor(), QColor(colorStrings.at(i)));

        delete textinputObject;
    }

    //test selected text color
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import Qt 4.7\nTextInput {  selectedTextColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->selectedTextColor(), QColor(colorStrings.at(i)));

        delete textinputObject;
    }

    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import Qt 4.7\nTextInput {  color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QDeclarativeComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->color(), testColor);

        delete textinputObject;
    }
}

void tst_qdeclarativetextinput::selection()
{
    QString testStr = standard[0];
    QString componentStr = "import Qt 4.7\nTextInput {  text: \""+ testStr +"\"; }";
    QDeclarativeComponent textinputComponent(&engine);
    textinputComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput*>(textinputComponent.create());
    QVERIFY(textinputObject != 0);


    //Test selection follows cursor
    for(int i=0; i<= testStr.size(); i++) {
        textinputObject->setCursorPosition(i);
        QCOMPARE(textinputObject->cursorPosition(), i);
        QCOMPARE(textinputObject->selectionStart(), i);
        QCOMPARE(textinputObject->selectionEnd(), i);
        QVERIFY(textinputObject->selectedText().isNull());
    }

    textinputObject->setCursorPosition(0);
    QVERIFY(textinputObject->cursorPosition() == 0);
    QVERIFY(textinputObject->selectionStart() == 0);
    QVERIFY(textinputObject->selectionEnd() == 0);
    QVERIFY(textinputObject->selectedText().isNull());

    //Test selection
    for(int i=0; i<= testStr.size(); i++) {
        textinputObject->select(0,i);
        QCOMPARE(testStr.mid(0,i), textinputObject->selectedText());
    }
    for(int i=0; i<= testStr.size(); i++) {
        textinputObject->select(i,testStr.size());
        QCOMPARE(testStr.mid(i,testStr.size()-i), textinputObject->selectedText());
    }

    textinputObject->setCursorPosition(0);
    QVERIFY(textinputObject->cursorPosition() == 0);
    QVERIFY(textinputObject->selectionStart() == 0);
    QVERIFY(textinputObject->selectionEnd() == 0);
    QVERIFY(textinputObject->selectedText().isNull());

    //Test Error Ignoring behaviour
    textinputObject->setCursorPosition(0);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(-10,0);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(100,110);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(0,-10);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(0,100);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->select(0,10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->select(-10,10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->select(100,101);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->select(0,-10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->select(0,100);
    QVERIFY(textinputObject->selectedText().size() == 10);

    delete textinputObject;
}

void tst_qdeclarativetextinput::horizontalAlignment_data()
{
    QTest::addColumn<int>("hAlign");
    QTest::addColumn<QString>("expectfile");

    QTest::newRow("L") << int(Qt::AlignLeft) << "halign_left";
    QTest::newRow("R") << int(Qt::AlignRight) << "halign_right";
    QTest::newRow("C") << int(Qt::AlignHCenter) << "halign_center";
}

void tst_qdeclarativetextinput::horizontalAlignment()
{
    QFETCH(int, hAlign);
    QFETCH(QString, expectfile);

    QDeclarativeView *canvas = createView(SRCDIR "/data/horizontalAlignment.qml");

    canvas->show();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));
    QObject *ob = canvas->rootObject();
    QVERIFY(ob != 0);
    ob->setProperty("horizontalAlignment",hAlign);
    QImage actual(canvas->width(), canvas->height(), QImage::Format_RGB32);
    actual.fill(qRgb(255,255,255));
    {
        QPainter p(&actual);
        canvas->render(&p);
    }

    expectfile = createExpectedFileIfNotFound(expectfile, actual);

    QImage expect(expectfile);

    QCOMPARE(actual,expect);

    delete canvas;
}

void tst_qdeclarativetextinput::positionAt()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/positionAt.qml");
    QVERIFY(canvas->rootObject() != 0);
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);

    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textinputObject != 0);

    // Check autoscrolled...
    QFontMetrics fm(textinputObject->font());

    int pos = textinputObject->positionAt(textinputObject->width()/2);
    int diff = abs(fm.width(textinputObject->text()) - (fm.width(textinputObject->text().left(pos))+textinputObject->width()/2));

    // some tollerance for different fonts.
#ifdef Q_OS_LINUX
    QVERIFY(diff < 2);
#else
    QVERIFY(diff < 5);
#endif

    // Check without autoscroll...
    textinputObject->setAutoScroll(false);
    pos = textinputObject->positionAt(textinputObject->width()/2);
    diff = abs(fm.width(textinputObject->text().left(pos))-textinputObject->width()/2);

    // some tollerance for different fonts.
#ifdef Q_OS_LINUX
    QVERIFY(diff < 2);
#else
    QVERIFY(diff < 5);
#endif

    delete canvas;
}

void tst_qdeclarativetextinput::maxLength()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/maxLength.qml");
    QVERIFY(canvas->rootObject() != 0);
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);

    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textinputObject != 0);
    QVERIFY(textinputObject->text().isEmpty());
    QVERIFY(textinputObject->maxLength() == 10);
    foreach(const QString &str, standard){
        QVERIFY(textinputObject->text().length() <= 10);
        textinputObject->setText(str);
        QVERIFY(textinputObject->text().length() <= 10);
    }

    textinputObject->setText("");
    QTRY_VERIFY(textinputObject->hasActiveFocus() == true);
    for(int i=0; i<20; i++){
        QCOMPARE(textinputObject->text().length(), qMin(i,10));
        //simulateKey(canvas, Qt::Key_A);
        QTest::keyPress(canvas, Qt::Key_A);
        QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    }

    delete canvas;
}

void tst_qdeclarativetextinput::masks()
{
    //Not a comprehensive test of the possible masks, that's done elsewhere (QLineEdit)
    //QString componentStr = "import Qt 4.7\nTextInput {  inputMask: 'HHHHhhhh'; }";
    QDeclarativeView *canvas = createView(SRCDIR "/data/masks.qml");
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextInput *textinputObject = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(textinputObject != 0);
    QTRY_VERIFY(textinputObject->hasActiveFocus() == true);
    QVERIFY(textinputObject->text().length() == 0);
    QCOMPARE(textinputObject->inputMask(), QString("HHHHhhhh; "));
    for(int i=0; i<10; i++){
        QCOMPARE(qMin(i,8), textinputObject->text().length());
        QCOMPARE(i>=4, textinputObject->hasAcceptableInput());
        //simulateKey(canvas, Qt::Key_A);
        QTest::keyPress(canvas, Qt::Key_A);
        QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    }

    delete canvas;
}

void tst_qdeclarativetextinput::validators()
{
    // Note that this test assumes that the validators are working properly
    // so you may need to run their tests first. All validators are checked
    // here to ensure that their exposure to QML is working.

    QDeclarativeView *canvas = createView(SRCDIR "/data/validators.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *intInput = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("intInput")));
    QVERIFY(intInput);
    intInput->setFocus(true);
    QTRY_VERIFY(intInput->hasActiveFocus());
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(intInput->text(), QLatin1String("1"));
    QCOMPARE(intInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_2);
    QTest::keyRelease(canvas, Qt::Key_2, Qt::NoModifier ,10);
    QCOMPARE(intInput->text(), QLatin1String("1"));
    QCOMPARE(intInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(intInput->text(), QLatin1String("11"));
    QCOMPARE(intInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_0);
    QTest::keyRelease(canvas, Qt::Key_0, Qt::NoModifier ,10);
    QCOMPARE(intInput->text(), QLatin1String("11"));
    QCOMPARE(intInput->hasAcceptableInput(), true);

    QDeclarativeTextInput *dblInput = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("dblInput")));
    QTRY_VERIFY(dblInput);
    dblInput->setFocus(true);
    QVERIFY(dblInput->hasActiveFocus() == true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("1"));
    QCOMPARE(dblInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_2);
    QTest::keyRelease(canvas, Qt::Key_2, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12"));
    QCOMPARE(dblInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_Period);
    QTest::keyRelease(canvas, Qt::Key_Period, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12."));
    QCOMPARE(dblInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12.1"));
    QCOMPARE(dblInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12.11"));
    QCOMPARE(dblInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(dblInput->text(), QLatin1String("12.11"));
    QCOMPARE(dblInput->hasAcceptableInput(), true);

    QDeclarativeTextInput *strInput = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("strInput")));
    QTRY_VERIFY(strInput);
    strInput->setFocus(true);
    QVERIFY(strInput->hasActiveFocus() == true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String(""));
    QCOMPARE(strInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("a"));
    QCOMPARE(strInput->hasAcceptableInput(), false);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("aa"));
    QCOMPARE(strInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("aaa"));
    QCOMPARE(strInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("aaaa"));
    QCOMPARE(strInput->hasAcceptableInput(), true);
    QTest::keyPress(canvas, Qt::Key_A);
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(strInput->text(), QLatin1String("aaaa"));
    QCOMPARE(strInput->hasAcceptableInput(), true);

    delete canvas;
}

void tst_qdeclarativetextinput::inputMethods()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/inputmethods.qml");
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);

    // test input method hints
    QVERIFY(canvas->rootObject() != 0);
    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(canvas->rootObject());
    QVERIFY(input != 0);
    QVERIFY(input->inputMethodHints() & Qt::ImhNoPredictiveText);
    input->setInputMethodHints(Qt::ImhUppercaseOnly);
    QVERIFY(input->inputMethodHints() & Qt::ImhUppercaseOnly);

    QVERIFY(canvas->rootObject() != 0);

    input->setFocus(true);
    QVERIFY(input->hasActiveFocus() == true);
    // test that input method event is committed
    QInputMethodEvent event;
    event.setCommitString( "My ", -12, 0);
    QApplication::sendEvent(canvas, &event);
    QCOMPARE(input->text(), QString("My Hello world!"));

    delete canvas;
}

/*
TextInput element should only handle left/right keys until the cursor reaches
the extent of the text, then they should ignore the keys.

*/
void tst_qdeclarativetextinput::navigation()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/navigation.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    input->setCursorPosition(0);
    QTRY_VERIFY(input->hasActiveFocus() == true);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == false);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == true);
    //QT-2944: If text is selected, ensure we deselect upon cursor motion
    input->setCursorPosition(input->text().length());
    input->select(0,input->text().length());
    QVERIFY(input->selectionStart() != input->selectionEnd());
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->selectionStart() == input->selectionEnd());
    QVERIFY(input->selectionStart() == input->text().length());
    QVERIFY(input->hasActiveFocus() == true);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasActiveFocus() == false);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasActiveFocus() == true);

    // Up and Down should NOT do Home/End, even on Mac OS X (QTBUG-10438).
    input->setCursorPosition(2);
    QCOMPARE(input->cursorPosition(),2);
    simulateKey(canvas, Qt::Key_Up);
    QCOMPARE(input->cursorPosition(),2);
    simulateKey(canvas, Qt::Key_Down);
    QCOMPARE(input->cursorPosition(),2);

    delete canvas;
}

void tst_qdeclarativetextinput::copyAndPaste() {
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

    QString componentStr = "import Qt 4.7\nTextInput { text: \"Hello world!\" }";
    QDeclarativeComponent textInputComponent(&engine);
    textInputComponent.setData(componentStr.toLatin1(), QUrl());
    QDeclarativeTextInput *textInput = qobject_cast<QDeclarativeTextInput*>(textInputComponent.create());
    QVERIFY(textInput != 0);

    // copy and paste
    QCOMPARE(textInput->text().length(), 12);
    textInput->select(0, textInput->text().length());;
    textInput->copy();
    QCOMPARE(textInput->selectedText(), QString("Hello world!"));
    QCOMPARE(textInput->selectedText().length(), 12);
    textInput->setCursorPosition(0);
    textInput->paste();
    QCOMPARE(textInput->text(), QString("Hello world!Hello world!"));
    QCOMPARE(textInput->text().length(), 24);

    // select word
    textInput->setCursorPosition(0);
    textInput->selectWord();
    QCOMPARE(textInput->selectedText(), QString("Hello"));

    // select all and cut
    textInput->selectAll();
    textInput->cut();
    QCOMPARE(textInput->text().length(), 0);
    textInput->paste();
    QCOMPARE(textInput->text(), QString("Hello world!Hello world!"));
    QCOMPARE(textInput->text().length(), 24);
#endif
}

void tst_qdeclarativetextinput::cursorDelegate()
{
    QDeclarativeView* view = createView(SRCDIR "/data/cursorTest.qml");
    view->show();
    view->setFocus();
    QDeclarativeTextInput *textInputObject = view->rootObject()->findChild<QDeclarativeTextInput*>("textInputObject");
    QVERIFY(textInputObject != 0);
    QVERIFY(textInputObject->findChild<QDeclarativeItem*>("cursorInstance"));
    //Test Delegate gets created
    textInputObject->setFocus(true);
    QDeclarativeItem* delegateObject = textInputObject->findChild<QDeclarativeItem*>("cursorInstance");
    QVERIFY(delegateObject);
    //Test Delegate gets moved
    for(int i=0; i<= textInputObject->text().length(); i++){
        textInputObject->setCursorPosition(i);
        //+5 is because the TextInput cursorRectangle is just a 10xHeight area centered on cursor position
        QCOMPARE(textInputObject->cursorRectangle().x() + 5, qRound(delegateObject->x()));
        QCOMPARE(textInputObject->cursorRectangle().y(), qRound(delegateObject->y()));
    }
    textInputObject->setCursorPosition(0);
    QCOMPARE(textInputObject->cursorRectangle().x()+5, qRound(delegateObject->x()));
    QCOMPARE(textInputObject->cursorRectangle().y(), qRound(delegateObject->y()));
    //Test Delegate gets deleted
    textInputObject->setCursorDelegate(0);
    QVERIFY(!textInputObject->findChild<QDeclarativeItem*>("cursorInstance"));

    delete view;
}

void tst_qdeclarativetextinput::readOnly()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/readOnly.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    QTRY_VERIFY(input->hasActiveFocus() == true);
    QVERIFY(input->isReadOnly() == true);
    QString initial = input->text();
    for(int k=Qt::Key_0; k<=Qt::Key_Z; k++)
        simulateKey(canvas, k);
    simulateKey(canvas, Qt::Key_Return);
    simulateKey(canvas, Qt::Key_Space);
    simulateKey(canvas, Qt::Key_Escape);
    QCOMPARE(input->text(), initial);

    delete canvas;
}

void tst_qdeclarativetextinput::echoMode()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/echoMode.qml");
    canvas->show();
    canvas->setFocus();
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeTextInput *input = qobject_cast<QDeclarativeTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    QTRY_VERIFY(input->hasActiveFocus() == true);
    QString initial = input->text();
    Qt::InputMethodHints ref;
    QCOMPARE(initial, QLatin1String("ABCDefgh"));
    QCOMPARE(input->echoMode(), QDeclarativeTextInput::Normal);
    QCOMPARE(input->displayText(), input->text());
    //Normal
    ref &= ~Qt::ImhHiddenText;
    ref &= ~(Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    input->setEchoMode(QDeclarativeTextInput::NoEcho);
    QCOMPARE(input->text(), initial);
    QCOMPARE(input->displayText(), QLatin1String(""));
    QCOMPARE(input->passwordCharacter(), QLatin1String("*"));
    //NoEcho
    ref |= Qt::ImhHiddenText;
    ref |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    input->setEchoMode(QDeclarativeTextInput::Password);
    //Password
    ref |= Qt::ImhHiddenText;
    ref |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->text(), initial);
    QCOMPARE(input->displayText(), QLatin1String("********"));
    QCOMPARE(input->inputMethodHints(), ref);
    input->setPasswordCharacter(QChar('Q'));
    QCOMPARE(input->passwordCharacter(), QLatin1String("Q"));
    QCOMPARE(input->text(), initial);
    QCOMPARE(input->displayText(), QLatin1String("QQQQQQQQ"));
    input->setEchoMode(QDeclarativeTextInput::PasswordEchoOnEdit);
    //PasswordEchoOnEdit
    ref &= ~Qt::ImhHiddenText;
    ref |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    QCOMPARE(input->inputMethodHints(), ref);
    QCOMPARE(input->text(), initial);
    QCOMPARE(input->displayText(), QLatin1String("QQQQQQQQ"));
    QTest::keyPress(canvas, Qt::Key_A);//Clearing previous entry is part of PasswordEchoOnEdit
    QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    QCOMPARE(input->text(), QLatin1String("a"));
    QCOMPARE(input->displayText(), QLatin1String("a"));
    input->setFocus(false);
    QVERIFY(input->hasActiveFocus() == false);
    QCOMPARE(input->displayText(), QLatin1String("Q"));
}

void tst_qdeclarativetextinput::simulateKey(QDeclarativeView *view, int key)
{
    QKeyEvent press(QKeyEvent::KeyPress, key, 0);
    QKeyEvent release(QKeyEvent::KeyRelease, key, 0);

    QApplication::sendEvent(view, &press);
    QApplication::sendEvent(view, &release);
}

QDeclarativeView *tst_qdeclarativetextinput::createView(const QString &filename)
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

void tst_qdeclarativetextinput::openInputPanelOnClick()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextInput input;
    QSignalSpy focusOnPressSpy(&input, SIGNAL(activeFocusOnPressChanged(bool)));
    input.setText("Hello world");
    input.setPos(0, 0);
    scene.addItem(&input);
    view.show();
    qApp->setAutoSipEnabled(true);
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QDeclarativeItemPrivate* pri = QDeclarativeItemPrivate::get(&input);
    QDeclarativeTextInputPrivate *inputPrivate = static_cast<QDeclarativeTextInputPrivate*>(pri);

    // input panel on click
    inputPrivate->showInputPanelOnFocus = false;

    QStyle::RequestSoftwareInputPanel behavior = QStyle::RequestSoftwareInputPanel(
            view.style()->styleHint(QStyle::SH_RequestSoftwareInputPanel));
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QApplication::processEvents();
    if (behavior == QStyle::RSIP_OnMouseClickAndAlreadyFocused) {
        QCOMPARE(ic.openInputPanelReceived, false);
        QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
        QApplication::processEvents();
        QCOMPARE(ic.openInputPanelReceived, true);
    } else if (behavior == QStyle::RSIP_OnMouseClick) {
        QCOMPARE(ic.openInputPanelReceived, true);
    }
    ic.openInputPanelReceived = false;

    // focus should not cause input panels to open or close
    input.setFocus(false);
    input.setFocus(true);
    input.setFocus(false);
    input.setFocus(true);
    input.setFocus(false);
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);
}

void tst_qdeclarativetextinput::openInputPanelOnFocus()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.setInputContext(&ic);
    QDeclarativeTextInput input;
    QSignalSpy focusOnPressSpy(&input, SIGNAL(activeFocusOnPressChanged(bool)));
    input.setText("Hello world");
    input.setPos(0, 0);
    scene.addItem(&input);
    view.show();
    qApp->setAutoSipEnabled(true);
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));

    QDeclarativeItemPrivate* pri = QDeclarativeItemPrivate::get(&input);
    QDeclarativeTextInputPrivate *inputPrivate = static_cast<QDeclarativeTextInputPrivate*>(pri);
    inputPrivate->showInputPanelOnFocus = true;

    // test default values
    QVERIFY(input.focusOnPress());
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);

    // focus on press, input panel on focus
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QApplication::processEvents();
    QVERIFY(input.hasActiveFocus());
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;

    // no events on release
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QCOMPARE(ic.openInputPanelReceived, false);
    ic.openInputPanelReceived = false;

    // if already focused, input panel can be opened on press
    QVERIFY(input.hasActiveFocus());
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;

    // input method should stay enabled if focus
    // is lost to an item that also accepts inputs
    QDeclarativeTextInput anotherInput;
    scene.addItem(&anotherInput);
    anotherInput.setFocus(true);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;
    QCOMPARE(view.inputContext(), &ic);
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
    input.setFocusOnPress(false);
    QCOMPARE(focusOnPressSpy.count(),1);
    input.setFocusOnPress(false);
    QCOMPARE(focusOnPressSpy.count(),1);
    input.setFocus(false);
    input.setFocus(true);
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);

    // one show input panel event should
    // be set when openSoftwareInputPanel is called
    input.openSoftwareInputPanel();
    QCOMPARE(ic.openInputPanelReceived, true);
    QCOMPARE(ic.closeInputPanelReceived, false);
    ic.openInputPanelReceived = false;

    // one close input panel event should
    // be sent when closeSoftwareInputPanel is called
    input.closeSoftwareInputPanel();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, true);
    ic.closeInputPanelReceived = false;

    // set activeFocusOnPress back to true
    input.setFocusOnPress(true);
    QCOMPARE(focusOnPressSpy.count(),2);
    input.setFocusOnPress(true);
    QCOMPARE(focusOnPressSpy.count(),2);
    input.setFocus(false);
    QApplication::processEvents();
    QCOMPARE(ic.openInputPanelReceived, false);
    QCOMPARE(ic.closeInputPanelReceived, false);
    ic.closeInputPanelReceived = false;

    // input panel should not re-open
    // if focus has already been set
    input.setFocus(true);
    QCOMPARE(ic.openInputPanelReceived, true);
    ic.openInputPanelReceived = false;
    input.setFocus(true);
    QCOMPARE(ic.openInputPanelReceived, false);

    // input method should be disabled
    // if TextInput loses focus
    input.setFocus(false);
    QApplication::processEvents();
    QVERIFY(view.inputContext() == 0);
    QVERIFY(!view.testAttribute(Qt::WA_InputMethodEnabled));
}

class MyTextInput : public QDeclarativeTextInput
{
public:
    MyTextInput(QDeclarativeItem *parent = 0) : QDeclarativeTextInput(parent)
    {
        nbPaint = 0;
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
       nbPaint++;
       QDeclarativeTextInput::paint(painter, option, widget);
    }
    int nbPaint;
};

void tst_qdeclarativetextinput::setHAlignClearCache()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyTextInput input;
    input.setText("Hello world");
    scene.addItem(&input);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(input.nbPaint, 1);
    input.setHAlign(QDeclarativeTextInput::AlignRight);
    QApplication::processEvents();
    //Changing the alignment should trigger a repaint
    QCOMPARE(input.nbPaint, 2);
}

void tst_qdeclarativetextinput::focusOutClearSelection()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QDeclarativeTextInput input;
    QDeclarativeTextInput input2;
    input.setText(QLatin1String("Hello world"));
    input.setFocus(true);
    scene.addItem(&input2);
    scene.addItem(&input);
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    input.select(2,5);
    //The selection should work
    QTRY_COMPARE(input.selectedText(), QLatin1String("llo"));
    input2.setFocus(true);
    QApplication::processEvents();
    //The input lost the focus selection should be cleared
    QTRY_COMPARE(input.selectedText(), QLatin1String(""));
}

void tst_qdeclarativetextinput::geometrySignals()
{
    QDeclarativeComponent component(&engine, SRCDIR "/data/geometrySignals.qml");
    QObject *o = component.create();
    QVERIFY(o);
    QCOMPARE(o->property("bindingWidth").toInt(), 400);
    QCOMPARE(o->property("bindingHeight").toInt(), 500);
    delete o;
}

QTEST_MAIN(tst_qdeclarativetextinput)

#include "tst_qdeclarativetextinput.moc"

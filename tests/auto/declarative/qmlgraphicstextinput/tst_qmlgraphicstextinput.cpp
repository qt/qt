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
#include "../../../shared/util.h"
#include <QtDeclarative/qmlengine.h>
#include <QFile>
#include <QtDeclarative/qmlview.h>
#include <private/qmlgraphicstextinput_p.h>
#include <QDebug>
#include <QStyle>
#include <QInputContext>

class tst_qmlgraphicstextinput : public QObject

{
    Q_OBJECT
public:
    tst_qmlgraphicstextinput();

private slots:
    void text();
    void width();
    void font();
    void color();
    void selection();

    void maxLength();
    void masks();
    void validators();
    void inputMethodHints();

    void cursorDelegate();
    void navigation();
    void readOnly();

    void sendRequestSoftwareInputPanelEvent();

private:
    void simulateKey(QmlView *, int key);
    QmlView *createView(const QString &filename);

    QmlEngine engine;
    QStringList standard;
    QStringList colorStrings;
};

tst_qmlgraphicstextinput::tst_qmlgraphicstextinput()
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

void tst_qmlgraphicstextinput::text()
{
    {
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData("import Qt 4.6\nTextInput {  text: \"\"  }", QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->text(), QString(""));
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextInput { text: \"" + standard.at(i) + "\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->text(), standard.at(i));
    }

}

void tst_qmlgraphicstextinput::width()
{
    // uses Font metrics to find the width for standard
    {
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData("import Qt 4.6\nTextInput {  text: \"\" }", QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->width(), 1.);//1 for the cursor
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetricsF fm(f);
        qreal metricWidth = fm.width(standard.at(i));

        QString componentStr = "import Qt 4.6\nTextInput { text: \"" + standard.at(i) + "\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->width(), qreal(metricWidth) + 1.);//1 for the cursor
    }
}

void tst_qmlgraphicstextinput::font()
{
    //test size, then bold, then italic, then family
    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.pointSize: 40; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().pointSize(), 40);
        QCOMPARE(textinputObject->font().bold(), false);
        QCOMPARE(textinputObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.bold: true; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().bold(), true);
        QCOMPARE(textinputObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.italic: true; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().italic(), true);
        QCOMPARE(textinputObject->font().bold(), false);
    }
 
    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.family: \"Helvetica\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().family(), QString("Helvetica"));
        QCOMPARE(textinputObject->font().bold(), false);
        QCOMPARE(textinputObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.family: \"\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().family(), QString(""));
    }
}

void tst_qmlgraphicstextinput::color()
{
    //test color
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import Qt 4.6\nTextInput {  color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->color(), QColor(colorStrings.at(i)));
    }

    //test selection color
    for (int i = 0; i < colorStrings.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextInput {  selectionColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->selectionColor(), QColor(colorStrings.at(i)));
    }

    //test selected text color
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import Qt 4.6\nTextInput {  selectedTextColor: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->selectedTextColor(), QColor(colorStrings.at(i)));
    }

    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import Qt 4.6\nTextInput {  color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine);
        textinputComponent.setData(componentStr.toLatin1(), QUrl());
        QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->color(), testColor);
    }
}

void tst_qmlgraphicstextinput::selection()
{
    QString testStr = standard[0];
    QString componentStr = "import Qt 4.6\nTextInput {  text: \""+ testStr +"\"; }";
    QmlComponent textinputComponent(&engine);
    textinputComponent.setData(componentStr.toLatin1(), QUrl());
    QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput*>(textinputComponent.create());
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
        textinputObject->setSelectionEnd(i);
        QCOMPARE(testStr.mid(0,i), textinputObject->selectedText());
    }
    for(int i=0; i<= testStr.size(); i++) {
        textinputObject->setSelectionStart(i);
        QCOMPARE(testStr.mid(i,testStr.size()-i), textinputObject->selectedText());
    }

    textinputObject->setCursorPosition(0);
    QVERIFY(textinputObject->cursorPosition() == 0);
    QVERIFY(textinputObject->selectionStart() == 0);
    QVERIFY(textinputObject->selectionEnd() == 0);
    QVERIFY(textinputObject->selectedText().isNull());

    for(int i=0; i< testStr.size(); i++) {
        textinputObject->setSelectionStart(i);
        QCOMPARE(textinputObject->selectionEnd(), i);
        QCOMPARE(testStr.mid(i,0), textinputObject->selectedText());
        textinputObject->setSelectionEnd(i+1);
        QCOMPARE(textinputObject->selectionStart(), i);
        QCOMPARE(testStr.mid(i,1), textinputObject->selectedText());
    }

    for(int i= testStr.size() - 1; i>0; i--) {
        textinputObject->setSelectionEnd(i);
        QCOMPARE(testStr.mid(i,0), textinputObject->selectedText());
        textinputObject->setSelectionStart(i-1);
        QCOMPARE(testStr.mid(i-1,1), textinputObject->selectedText());
    }

    //Test Error Ignoring behaviour
    textinputObject->setCursorPosition(0);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->setSelectionStart(-10);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->setSelectionStart(100);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->setSelectionEnd(-10);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->setSelectionEnd(100);
    QVERIFY(textinputObject->selectedText().isNull());
    textinputObject->setSelectionStart(0);
    textinputObject->setSelectionEnd(10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->setSelectionStart(-10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->setSelectionStart(100);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->setSelectionEnd(-10);
    QVERIFY(textinputObject->selectedText().size() == 10);
    textinputObject->setSelectionEnd(100);
    QVERIFY(textinputObject->selectedText().size() == 10);
}

void tst_qmlgraphicstextinput::maxLength()
{
    //QString componentStr = "import Qt 4.6\nTextInput {  maximumLength: 10; }";
    QmlView *canvas = createView(SRCDIR "/data/maxLength.qml");
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);
    QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput *>(canvas->rootObject());
    QVERIFY(textinputObject != 0);
    QVERIFY(textinputObject->text().isEmpty());
    QVERIFY(textinputObject->maxLength() == 10);
    foreach(const QString &str, standard){
        QVERIFY(textinputObject->text().length() <= 10);
        textinputObject->setText(str);
        QVERIFY(textinputObject->text().length() <= 10);
    }

    textinputObject->setText("");
    QTRY_VERIFY(textinputObject->hasFocus() == true);
    for(int i=0; i<20; i++){
        QCOMPARE(textinputObject->text().length(), qMin(i,10));
        //simulateKey(canvas, Qt::Key_A);
        QTest::keyPress(canvas, Qt::Key_A);
        QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    }
}

void tst_qmlgraphicstextinput::masks()
{
    //Not a comprehensive test of the possible masks, that's done elsewhere (QLineEdit)
    //QString componentStr = "import Qt 4.6\nTextInput {  inputMask: 'HHHHhhhh'; }";
    QmlView *canvas = createView(SRCDIR "/data/masks.qml");
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);
    QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput *>(canvas->rootObject());
    QVERIFY(textinputObject != 0);
    QTRY_VERIFY(textinputObject->hasFocus() == true);
    QVERIFY(textinputObject->text().length() == 0);
    QCOMPARE(textinputObject->inputMask(), QString("HHHHhhhh; "));
    for(int i=0; i<10; i++){
        QCOMPARE(qMin(i,8), textinputObject->text().length());
        QCOMPARE(i>=4, textinputObject->hasAcceptableInput());
        //simulateKey(canvas, Qt::Key_A);
        QTest::keyPress(canvas, Qt::Key_A);
        QTest::keyRelease(canvas, Qt::Key_A, Qt::NoModifier ,10);
    }
}

void tst_qmlgraphicstextinput::validators()
{
    // Note that this test assumes that the validators are working properly
    // so you may need to run their tests first. All validators are checked
    // here to ensure that their exposure to QML is working.

    QmlView *canvas = createView(SRCDIR "/data/validators.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QmlGraphicsTextInput *intInput = qobject_cast<QmlGraphicsTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("intInput")));
    QVERIFY(intInput);
    intInput->setFocus(true);
    QTRY_VERIFY(intInput->hasFocus());
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

    QmlGraphicsTextInput *dblInput = qobject_cast<QmlGraphicsTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("dblInput")));
    QTRY_VERIFY(dblInput);
    dblInput->setFocus(true);
    QVERIFY(dblInput->hasFocus() == true);
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

    QmlGraphicsTextInput *strInput = qobject_cast<QmlGraphicsTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("strInput")));
    QTRY_VERIFY(strInput);
    strInput->setFocus(true);
    QVERIFY(strInput->hasFocus() == true);
    QTest::keyPress(canvas, Qt::Key_1);
    QTest::keyRelease(canvas, Qt::Key_1, Qt::NoModifier ,10);
    QEXPECT_FAIL("","Will not work until QTBUG-8025 is resolved", Abort);
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
}

void tst_qmlgraphicstextinput::inputMethodHints()
{
    QmlView *canvas = createView(SRCDIR "/data/inputmethodhints.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);
    QmlGraphicsTextInput *textinputObject = qobject_cast<QmlGraphicsTextInput *>(canvas->rootObject());
    QVERIFY(textinputObject != 0);
    QVERIFY(textinputObject->inputMethodHints() & Qt::ImhNoPredictiveText);
    textinputObject->setInputMethodHints(Qt::ImhUppercaseOnly);
    QVERIFY(textinputObject->inputMethodHints() & Qt::ImhUppercaseOnly);
}

/*
TextInput element should only handle left/right keys until the cursor reaches
the extent of the text, then they should ignore the keys.

*/
void tst_qmlgraphicstextinput::navigation()
{
    QmlView *canvas = createView(SRCDIR "/data/navigation.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QmlGraphicsTextInput *input = qobject_cast<QmlGraphicsTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    input->setCursorPosition(0);
    QTRY_VERIFY(input->hasFocus() == true);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasFocus() == false);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasFocus() == true);
    //QT-2944: If text is selected, then we should deselect first.
    input->setCursorPosition(input->text().length());
    input->setSelectionStart(0);
    input->setSelectionEnd(input->text().length());
    QVERIFY(input->selectionStart() != input->selectionEnd());
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->selectionStart() == input->selectionEnd());
    QVERIFY(input->selectionStart() == input->text().length());
    QVERIFY(input->hasFocus() == true);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasFocus() == false);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasFocus() == true);
}

void tst_qmlgraphicstextinput::cursorDelegate()
{
    QmlView* view = createView(SRCDIR "/data/cursorTest.qml");
    view->show();
    view->setFocus();
    QmlGraphicsTextInput *textInputObject = view->rootObject()->findChild<QmlGraphicsTextInput*>("textInputObject");
    QVERIFY(textInputObject != 0);
    QVERIFY(textInputObject->findChild<QmlGraphicsItem*>("cursorInstance"));
    //Test Delegate gets created
    textInputObject->setFocus(true);
    QmlGraphicsItem* delegateObject = textInputObject->findChild<QmlGraphicsItem*>("cursorInstance");
    QVERIFY(delegateObject);
    //Test Delegate gets moved
    for(int i=0; i<= textInputObject->text().length(); i++){
        textInputObject->setCursorPosition(i);
        //+5 is because the TextInput cursorRect is just a 10xHeight area centered on cursor position
        QCOMPARE(textInputObject->cursorRect().x() + 5, qRound(delegateObject->x()));
        QCOMPARE(textInputObject->cursorRect().y(), qRound(delegateObject->y()));
    }
    textInputObject->setCursorPosition(0);
    QCOMPARE(textInputObject->cursorRect().x()+5, qRound(delegateObject->x()));
    QCOMPARE(textInputObject->cursorRect().y(), qRound(delegateObject->y()));
    //Test Delegate gets deleted
    textInputObject->setCursorDelegate(0);
    QVERIFY(!textInputObject->findChild<QmlGraphicsItem*>("cursorInstance"));
}

void tst_qmlgraphicstextinput::readOnly()
{
    QmlView *canvas = createView(SRCDIR "/data/readOnly.qml");
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->rootObject() != 0);

    QmlGraphicsTextInput *input = qobject_cast<QmlGraphicsTextInput *>(qvariant_cast<QObject *>(canvas->rootObject()->property("myInput")));

    QVERIFY(input != 0);
    QTRY_VERIFY(input->hasFocus() == true);
    QVERIFY(input->isReadOnly() == true);
    QString initial = input->text();
    for(int k=Qt::Key_0; k<=Qt::Key_Z; k++)
        simulateKey(canvas, k);
    simulateKey(canvas, Qt::Key_Return);
    simulateKey(canvas, Qt::Key_Space);
    simulateKey(canvas, Qt::Key_Escape);
    QCOMPARE(input->text(), initial);
}

void tst_qmlgraphicstextinput::simulateKey(QmlView *view, int key)
{
    QKeyEvent press(QKeyEvent::KeyPress, key, 0);
    QKeyEvent release(QKeyEvent::KeyRelease, key, 0);

    QApplication::sendEvent(view, &press);
    QApplication::sendEvent(view, &release);
}

QmlView *tst_qmlgraphicstextinput::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);

    canvas->setSource(QUrl::fromLocalFile(filename));

    return canvas;
}

class MyInputContext : public QInputContext
{
public:
    MyInputContext() : softwareInputPanelEventReceived(false) {}
    ~MyInputContext() {}

    QString identifierName() { return QString(); }
    QString language() { return QString(); }

    void reset() {}

    bool isComposing() const { return false; }

    bool filterEvent( const QEvent *event )
    {
        if (event->type() == QEvent::RequestSoftwareInputPanel)
            softwareInputPanelEventReceived = true;
        return QInputContext::filterEvent(event);
    }
    bool softwareInputPanelEventReceived;
};

void tst_qmlgraphicstextinput::sendRequestSoftwareInputPanelEvent()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    MyInputContext ic;
    view.viewport()->setInputContext(&ic);
    QStyle::RequestSoftwareInputPanel behavior = QStyle::RequestSoftwareInputPanel(
            view.style()->styleHint(QStyle::SH_RequestSoftwareInputPanel));
    if ((behavior != QStyle::RSIP_OnMouseClick))
        QSKIP("This test need to have a style with RSIP_OnMouseClick", SkipSingle);
    QmlGraphicsTextInput input;
    input.setText("Hello world");
    input.setPos(0, 0);
    scene.addItem(&input);
    view.show();
    qApp->setAutoSipEnabled(true);
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(input.scenePos()));
    QApplication::processEvents();
    QCOMPARE(ic.softwareInputPanelEventReceived, true);
}

QTEST_MAIN(tst_qmlgraphicstextinput)

#include "tst_qmlgraphicstextinput.moc"

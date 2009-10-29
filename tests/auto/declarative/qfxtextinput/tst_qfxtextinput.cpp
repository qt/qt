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
#include <QtDeclarative/qmlengine.h>
#include <QFile>
#include <QtDeclarative/qmlview.h>
#include <QFxTextInput>
#include <QDebug>

class tst_qfxtextinput : public QObject

{
    Q_OBJECT
public:
    tst_qfxtextinput();

private slots:
    void text();
    void width();
    void font();
    void color();
    void selection();

    void maxLength();
    void masks();
    void validators();

    void cursorDelegate();
    void navigation();

private:
    void simulateKey(QmlView *, int key);
    QmlView *createView(const QString &filename);

    QmlEngine engine;
    QStringList standard;
    QStringList colorStrings;
};

tst_qfxtextinput::tst_qfxtextinput()
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

void tst_qfxtextinput::text()
{
    {
        QmlComponent textinputComponent(&engine, "import Qt 4.6\nTextInput {  text: \"\"  }", QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->text(), QString(""));
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextInput { text: \"" + standard.at(i) + "\" }";
        QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->text(), standard.at(i));
    }

}

void tst_qfxtextinput::width()
{
    // uses Font metrics to find the width for standard
    {
        QmlComponent textinputComponent(&engine, "import Qt 4.6\nTextInput {  text: \"\" }", QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->width(), 1.);//1 for the cursor
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetrics fm(f);
        int metricWidth = fm.width(standard.at(i));

        QString componentStr = "import Qt 4.6\nTextInput { text: \"" + standard.at(i) + "\" }";
        QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->width(), qreal(metricWidth) + 1.);//1 for the cursor
    }
}

void tst_qfxtextinput::font()
{
    //test size, then bold, then italic, then family
    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.pointSize: 40; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().pointSize(), 40);
        QCOMPARE(textinputObject->font().bold(), false);
        QCOMPARE(textinputObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.bold: true; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().bold(), true);
        QCOMPARE(textinputObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.italic: true; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().italic(), true);
        QCOMPARE(textinputObject->font().bold(), false);
    }
 
    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.family: \"Helvetica\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().family(), QString("Helvetica"));
        QCOMPARE(textinputObject->font().bold(), false);
        QCOMPARE(textinputObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextInput {  font.family: \"\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->font().family(), QString(""));
    }
}

void tst_qfxtextinput::color()
{
    //test style
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import Qt 4.6\nTextInput {  color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());
        //qDebug() << "textinputObject: " << textinputObject->color() << "vs. " << QColor(colorStrings.at(i));
        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->color(), QColor(colorStrings.at(i)));
    }

    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import Qt 4.6\nTextInput {  color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());

        QVERIFY(textinputObject != 0);
        QCOMPARE(textinputObject->color(), testColor);
    }
}

void tst_qfxtextinput::selection()
{
    QString testStr = standard[0];
    QString componentStr = "import Qt 4.6\nTextInput {  text: \""+ testStr +"\"; }";
    QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
    QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());
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

void tst_qfxtextinput::maxLength()
{
    QString componentStr = "import Qt 4.6\nTextInput {  maximumLength: 10; }";
    QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
    QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());
    QVERIFY(textinputObject != 0);
    QVERIFY(textinputObject->text().isEmpty());
    foreach(const QString &str, standard){
        QVERIFY(textinputObject->text().length() <= 10);
        textinputObject->setText(str);
        QVERIFY(textinputObject->text().length() <= 10);
    }
    //TODO: Simulated keypress input adding 11 chars at a time
}

void tst_qfxtextinput::masks()
{
    QString componentStr = "import Qt 4.6\nTextInput {  maximumLength: 10; }";
    QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
    QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());
    QVERIFY(textinputObject != 0);

    //TODO: Me
}

void tst_qfxtextinput::validators()
{
    QString componentStr = "import Qt 4.6\nTextInput {  maximumLength: 10; }";
    QmlComponent textinputComponent(&engine, componentStr.toLatin1(), QUrl());
    QFxTextInput *textinputObject = qobject_cast<QFxTextInput*>(textinputComponent.create());
    QVERIFY(textinputObject != 0);

    //TODO: Me
}

/*
TextInput element should only handle left/right keys until the cursor reaches
the extent of the text, then they should ignore the keys.
*/
void tst_qfxtextinput::navigation()
{
    QmlView *canvas = createView(SRCDIR "/data/navigation.qml");
    canvas->execute();
    canvas->show();
    canvas->setFocus();

    QVERIFY(canvas->root() != 0);

    QFxItem *input = qobject_cast<QFxItem *>(qvariant_cast<QObject *>(canvas->root()->property("myInput")));

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

void tst_qfxtextinput::cursorDelegate()
{
    QmlView* view = createView(SRCDIR "/data/cursorTest.qml");
    view->execute();
    view->show();
    view->setFocus();
    QFxTextInput *textInputObject = view->root()->findChild<QFxTextInput*>("textInputObject");
    QVERIFY(textInputObject != 0);
    QVERIFY(textInputObject->findChild<QFxItem*>("cursorInstance"));
    //Test Delegate gets created
    textInputObject->setFocus(true);
    QFxItem* delegateObject = textInputObject->findChild<QFxItem*>("cursorInstance");
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
    QVERIFY(!textInputObject->findChild<QFxItem*>("cursorInstance"));
}

void tst_qfxtextinput::simulateKey(QmlView *view, int key)
{
    QKeyEvent press(QKeyEvent::KeyPress, key, 0);
    QKeyEvent release(QKeyEvent::KeyRelease, key, 0);

    QApplication::sendEvent(view, &press);
    QApplication::sendEvent(view, &release);
}

QmlView *tst_qfxtextinput::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString xml = file.readAll();
    canvas->setQml(xml, filename);

    return canvas;
}

QTEST_MAIN(tst_qfxtextinput)

#include "tst_qfxtextinput.moc"

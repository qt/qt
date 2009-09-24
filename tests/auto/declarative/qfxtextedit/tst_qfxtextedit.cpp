#include <qtest.h>
#include <QTextDocument>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlexpression.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxtextedit.h>
#include <QFontMetrics>
#include <QLibraryInfo>
#include <QProcess>
#include <QDir>

class tst_qfxtextedit : public QObject

{
    Q_OBJECT
public:
    tst_qfxtextedit();

private slots:
    void text();
    void width();
    void wrap();

    // ### these tests may be trivial    
    void hAlign();
    void vAlign();
    void font();
    void color();
    void selection();

    void cursorDelegate();

    void visualTests_data();
    void visualTests();
private:
    QStringList standard;
    QStringList richText;

    QStringList hAlignmentStrings;
    QStringList vAlignmentStrings;

    QList<Qt::Alignment> vAlignments;
    QList<Qt::Alignment> hAlignments;

    QStringList colorStrings;

    QmlEngine engine;
    QString qmlviewerBinary;
};

tst_qfxtextedit::tst_qfxtextedit()
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

    QString binaries = QLibraryInfo::location(QLibraryInfo::BinariesPath);

#if defined(Q_WS_MAC)
    qmlviewerBinary = QDir(binaries).absoluteFilePath("qmlviewer.app/Contents/MacOS/qmlviewer");
#elif defined(Q_WS_WIN)
    qmlviewerBinary = QDir(binaries).absoluteFilePath("qmlviewer.exe");
#else
    qmlviewerBinary = QDir(binaries).absoluteFilePath("qmlviewer");
#endif
}

void tst_qfxtextedit::text()
{
    {
        QmlComponent texteditComponent(&engine, "import Qt 4.6\nTextEdit {  text: \"\"  }", QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), QString(""));
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit { text: \"" + standard.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), standard.at(i));
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit { text: \"" + richText.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QString actual = textEditObject->text();
        QString expected = richText.at(i);
        actual.replace(QRegExp(".*<body[^>]*>"),"");
        actual.replace(QRegExp("(<[^>]*>)+"),"<>");
        expected.replace(QRegExp("(<[^>]*>)+"),"<>");
        QCOMPARE(actual.simplified(),expected.simplified());
    }
}

void tst_qfxtextedit::width()
{
    // uses Font metrics to find the width for standard and document to find the width for rich
    {
        QmlComponent texteditComponent(&engine, "import Qt 4.6\nTextEdit {  text: \"\" }", QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 1.);//+1 for cursor
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetrics fm(f);
        int metricWidth = fm.size(Qt::TextExpandTabs && Qt::TextShowMnemonic, standard.at(i)).width();

        QString componentStr = "import Qt 4.6\nTextEdit { text: \"" + standard.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

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
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), qreal(documentWidth + 1 + 3));
    }
}

void tst_qfxtextedit::wrap()
{
    // for specified width and wrap set true
    {
        QmlComponent texteditComponent(&engine, "import Qt 4.6\nTextEdit {  text: \"\"; wrap: true; width: 300 }", QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit {  wrap: true; width: 300; text: \"" + standard.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "import Qt 4.6\nTextEdit {  wrap: true; width: 300; text: \"" + richText.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

}

//the alignment tests may be trivial o.oa
void tst_qfxtextedit::hAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nTextEdit {  horizontalAlignment: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
            QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->hAlign(), (int)hAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nTextEdit {  horizontalAlignment: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
            QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->hAlign(), (int)hAlignments.at(j));
        }
    }

}

void tst_qfxtextedit::vAlign()
{
    //test one align each, and then test if two align fails.

    for (int i = 0; i < standard.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nTextEdit {  verticalAlignment: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
            QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->vAlign(), (int)vAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "import Qt 4.6\nTextEdit {  verticalAlignment: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
            QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->vAlign(), (int)vAlignments.at(j));
        }
    }

}

void tst_qfxtextedit::font()
{
    //test size, then bold, then italic, then family
    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.pointSize: 40; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().pointSize(), 40);
        QCOMPARE(textEditObject->font().bold(), false);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.bold: true; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().bold(), true);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.italic: true; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().italic(), true);
        QCOMPARE(textEditObject->font().bold(), false);
    }
 
    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.family: \"Helvetica\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().family(), QString("Helvetica"));
        QCOMPARE(textEditObject->font().bold(), false);
        QCOMPARE(textEditObject->font().italic(), false);
    }

    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  font.family: \"\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font().family(), QString(""));
    }
}

void tst_qfxtextedit::color()
{
    //test style
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "import Qt 4.6\nTextEdit {  color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());
        //qDebug() << "textEditObject: " << textEditObject->color() << "vs. " << QColor(colorStrings.at(i));
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), QColor(colorStrings.at(i)));
    }

    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "import Qt 4.6\nTextEdit {  color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), testColor);
    }
}

void tst_qfxtextedit::selection()
{
    QString testStr = standard[0];//TODO: What should happen for multiline/rich text?
    QString componentStr = "import Qt 4.6\nTextEdit {  text: \""+ testStr +"\"; }";
    QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
    QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());
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

#include <QmlView>
void tst_qfxtextedit::cursorDelegate()
{
    QmlView* view = new QmlView(0);
    view->show();
    view->setUrl(QUrl("data/cursorTest.qml"));
    view->execute();
    QFxTextEdit *textEditObject = view->root()->findChild<QFxTextEdit*>("textEditObject");
    QVERIFY(textEditObject != 0);
    QVERIFY(textEditObject->findChild<QFxItem*>("cursorInstance"));
    //Test Delegate gets created
    textEditObject->setFocus(true);
    QFxItem* delegateObject = textEditObject->findChild<QFxItem*>("cursorInstance");
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
    QVERIFY(!textEditObject->findChild<QFxItem*>("cursorInstance"));
}

void tst_qfxtextedit::visualTests_data()
{
    QTest::addColumn<QString>("qmlFile");
    QTest::addColumn<QString>("scriptFile");
    QTest::newRow("basic") << "data/basic.qml" << "data/basic-test";
    QTest::newRow("colorful") << "data/colorful.qml" << "data/colorful-test";
    QTest::newRow("QT-669") << "data/QT-669.qml" << "data/QT-669-test";
}

void tst_qfxtextedit::visualTests()
{
    QFETCH(QString, qmlFile);
    QFETCH(QString, scriptFile);

    QStringList arguments;
    arguments << "-script" << scriptFile
              << "-scriptopts" << "play,exitoncomplete,exitonfailure" 
              << qmlFile;
    QProcess p;
    p.start(qmlviewerBinary, arguments);
    QVERIFY(p.waitForFinished());
    QCOMPARE(p.exitStatus(), QProcess::NormalExit);
    QCOMPARE(p.exitCode(), 0);
}

QTEST_MAIN(tst_qfxtextedit)

#include "tst_qfxtextedit.moc"

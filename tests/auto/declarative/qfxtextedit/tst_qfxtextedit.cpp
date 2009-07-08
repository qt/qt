#include <qtest.h>
#include <QTextDocument>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlexpression.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxtextedit.h>
#include <QFontMetrics>

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

    void cursorDelegate();

private:
    QStringList standard;
    QStringList richText;

    QStringList hAlignmentStrings;
    QStringList vAlignmentStrings;

    QList<Qt::Alignment> vAlignments;
    QList<Qt::Alignment> hAlignments;

    QStringList colorStrings;

    QmlEngine engine;
};

/*
   Find an item with the specified id.  If index is supplied then the
   item must also evaluate the {index} expression equal to index

   Copied from ListView test
*/
template<typename T>
T *findItem(QFxItem *parent, const QString &id, int index=0)
{
    const QMetaObject &mo = T::staticMetaObject;
    qDebug() << parent->children()->count() << "children";
    for (int i = 0; i < parent->children()->count(); ++i) {
        QFxItem *item = parent->children()->at(i);
        qDebug() << "try" << item;
        if (mo.cast(item) && (id.isEmpty() || item->id() == id)) {
            if (index != -1) {
                QmlExpression e(qmlContext(item), "index", item);
                e.setTrackChange(false);
                if (e.value().toInt() == index)
                    return static_cast<T*>(item);
            } else {
                return static_cast<T*>(item);
            }
        }
        item = findItem<T>(item, id, index);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

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
}

void tst_qfxtextedit::text()
{
    {
        QmlComponent texteditComponent(&engine, "TextEdit {  text: \"\"  }");
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), QString(""));
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "TextEdit { text: \"" + standard.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->text(), standard.at(i));
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "TextEdit { text: \"" + richText.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
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
        QmlComponent texteditComponent(&engine, "TextEdit {  text: \"\" }");
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 0.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QFont f;
        QFontMetrics fm(f);
        int metricWidth = fm.size(Qt::TextExpandTabs && Qt::TextShowMnemonic, standard.at(i)).width();

        QString componentStr = "TextEdit { text: \"" + standard.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), qreal(metricWidth));
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QTextDocument document;
        document.setHtml(richText.at(i));
        document.setDocumentMargin(0);

        int documentWidth = document.idealWidth();

        QString componentStr = "TextEdit { text: \"" + richText.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), qreal(documentWidth));
    }
}

void tst_qfxtextedit::wrap()
{
    // for specified width and wrap set true
    {
        QmlComponent texteditComponent(&engine, "TextEdit {  text: \"\"; wrap: true; width: 300 }");
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < standard.size(); i++)
    {
        QString componentStr = "TextEdit {  wrap: true; width: 300; text: \"" + standard.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->width(), 300.);
    }

    for (int i = 0; i < richText.size(); i++)
    {
        QString componentStr = "TextEdit {  wrap: true; width: 300; text: \"" + richText.at(i) + "\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
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
            QString componentStr = "TextEdit {  hAlign: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QmlComponent texteditComponent(&engine, componentStr.toLatin1());
            QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->hAlign(), (int)hAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < hAlignmentStrings.size(); j++)
        {
            QString componentStr = "TextEdit {  hAlign: \"" + hAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QmlComponent texteditComponent(&engine, componentStr.toLatin1());
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
            QString componentStr = "TextEdit {  vAlign: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + standard.at(i) + "\" }";
            QmlComponent texteditComponent(&engine, componentStr.toLatin1());
            QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

            QVERIFY(textEditObject != 0);
            QCOMPARE((int)textEditObject->vAlign(), (int)vAlignments.at(j));
        }
    }

    for (int i = 0; i < richText.size(); i++)
    {
        for (int j=0; j < vAlignmentStrings.size(); j++)
        {
            QString componentStr = "TextEdit {  vAlign: \"" + vAlignmentStrings.at(j) + "\"; text: \"" + richText.at(i) + "\" }";
            QmlComponent texteditComponent(&engine, componentStr.toLatin1());
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
        QString componentStr = "TextEdit {  font.size: 40; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font()->size(), qreal(40));
        QCOMPARE(textEditObject->font()->bold(), false);
        QCOMPARE(textEditObject->font()->italic(), false);
    }

    { 
        QString componentStr = "TextEdit {  font.bold: true; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font()->bold(), true);
        QCOMPARE(textEditObject->font()->italic(), false);
    }

    { 
        QString componentStr = "TextEdit {  font.italic: true; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font()->italic(), true);
        QCOMPARE(textEditObject->font()->bold(), false);
    }
 
    { 
        QString componentStr = "TextEdit {  font.family: \"Helvetica\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font()->family(), QString("Helvetica"));
        QCOMPARE(textEditObject->font()->bold(), false);
        QCOMPARE(textEditObject->font()->italic(), false);
    }

    { 
        QString componentStr = "TextEdit {  font.family: \"\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->font()->family(), QString(""));
    }
}

void tst_qfxtextedit::color()
{
    //test style
    for (int i = 0; i < colorStrings.size(); i++)
    { 
        QString componentStr = "TextEdit {  color: \"" + colorStrings.at(i) + "\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());
        //qDebug() << "textEditObject: " << textEditObject->color() << "vs. " << QColor(colorStrings.at(i));
        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), QColor(colorStrings.at(i)));
    }

    {
        QString colorStr = "#AA001234";
        QColor testColor("#001234");
        testColor.setAlpha(170);

        QString componentStr = "TextEdit {  color: \"" + colorStr + "\"; text: \"Hello World\" }";
        QmlComponent texteditComponent(&engine, componentStr.toLatin1());
        QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());

        QVERIFY(textEditObject != 0);
        QCOMPARE(textEditObject->color(), testColor);
    }
}

void tst_qfxtextedit::cursorDelegate()
{
    QString componentStr = "TextEdit {  text: \""+ standard[1] +"\"; focusable: true; resources [ Component { id:cursor; Item { id:cursorInstance } } ] cursorDelegate: cursor}";
    QTest::qWait(10000);
    QmlComponent texteditComponent(&engine, componentStr.toLatin1(), QUrl());
    QFxTextEdit *textEditObject = qobject_cast<QFxTextEdit*>(texteditComponent.create());
    QVERIFY(textEditObject != 0);
    QVERIFY(!findItem<QFxItem>(textEditObject, "cursorInstance"));
    //Test Delegate gets created
    textEditObject->setFocus(true);
    QFxItem* delegateObject = findItem<QFxItem>(textEditObject, "cursorInstance");
    QVERIFY(delegateObject);
    //Test Delegate gets moved
    for(int i=0; i<= standard[1].size(); i++){
        textEditObject->setCursorPosition(i);
        QCOMPARE(textEditObject->cursorRect().x(), qRound(delegateObject->x()));
        QCOMPARE(textEditObject->cursorRect().y(), qRound(delegateObject->y()));
    }
    textEditObject->setCursorPosition(0);
    QCOMPARE(textEditObject->cursorRect().x(), qRound(delegateObject->x()));
    QCOMPARE(textEditObject->cursorRect().y(), qRound(delegateObject->y()));
    //Test Delegate gets deleted
    textEditObject->setCursorDelegate(0);
    QVERIFY(!findItem<QFxItem>(textEditObject, "cursorInstance"));
}

QTEST_MAIN(tst_qfxtextedit)

#include "tst_qfxtextedit.moc"

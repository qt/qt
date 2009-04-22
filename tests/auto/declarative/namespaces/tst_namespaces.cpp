#include <qtest.h>
#include <QtDeclarative/qfxview.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qfxrect.h>
#include <QtDeclarative/qfxtext.h>
#include <QDir>

class tst_namespaces : public QObject

{
    Q_OBJECT
public:
    tst_namespaces();

private slots:
    void simple();
    void simple_data();
};

tst_namespaces::tst_namespaces()
{
}

void tst_namespaces::simple_data()
{
    QTest::addColumn<QString>("qml");    
    QTest::addColumn<bool>("valid");
    QTest::addColumn<QString>("texts");
    QTest::addColumn<QString>("rects");

    QTest::newRow("Control") <<
        "<Rect objectName=\"a\">"
            "<Rect objectName=\"b\"/>"
        "</Rect>"
        << true << "" << "ab";

    QTest::newRow("Control2") <<
        "<Rect objectName=\"a\">"
            "<Text objectName=\"b\"/>"
        "</Rect>"
        << true << "b" << "a";

    QTest::newRow("Replace builtin") <<
        "<Rect objectName=\"a\" xmlns:rs=\"http://nokia.com/qml/Red\">"
            "<rs:Rect objectName=\"b\"/>"
        "</Rect>"
        << true << "b" << "a";
}

void tst_namespaces::simple()
{
    QFETCH(QString, qml);
    QFETCH(bool, valid);
    QFETCH(QString, texts);
    QFETCH(QString, rects);

    QFxView canvas(0);
    canvas.rootContext()->engine()->addNameSpacePath("http://nokia.com/qml", SRCDIR "/data");
    canvas.setXml(qml);
    canvas.execute();
    qApp->processEvents();

    QFxItem *testObject = qobject_cast<QFxItem*>(canvas.root());

    QCOMPARE((testObject != 0),valid);

    if (valid && testObject != 0) {
        QString textids;
        QList<QFxText*> textobjects = testObject->findChildren<QFxText*>();
        if (qobject_cast<QFxText*>(testObject))
            textids += testObject->objectName();
        foreach (QFxText *obj, textobjects) {
            textids += obj->objectName();
        }
        if (textids != texts)
            testObject->dump();
        QCOMPARE(textids,texts);

        QString rectids;
        QList<QFxRect*> rectobjects = testObject->findChildren<QFxRect*>();
        if (qobject_cast<QFxRect*>(testObject))
            rectids += testObject->objectName();
        foreach (QFxRect *obj, rectobjects)
            rectids += obj->objectName();
        if (rectids != rects)
            testObject->dump();
        QCOMPARE(rectids,rects);
    }
}

QTEST_MAIN(tst_namespaces)

#include "tst_namespaces.moc"

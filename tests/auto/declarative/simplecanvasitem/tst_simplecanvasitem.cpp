#include <QtTest/QtTest>
#include <qfxview.h>
#include <qfxitem.h>
#include <qsimplecanvasitem.h>

/*
    Note: this autotest is specifically to test SimpleCanvasItem as a component of
    Qt Declarative, and therefore will have all items created in XML.
*/
class tst_SimpleCanvasItem : public QObject
{
    Q_OBJECT
public:
    tst_SimpleCanvasItem();

private slots:
    void test_pos();
    void test_scenePos();
private:
    QFxView *createView(const QString &filename);
};

tst_SimpleCanvasItem::tst_SimpleCanvasItem()
{
}

void tst_SimpleCanvasItem::test_pos()
{
    QFxView *canvas = createView(SRCDIR "/data/test.qml");
    canvas->execute();
    qApp->processEvents();
    QSimpleCanvasItem* root = qobject_cast<QSimpleCanvasItem*>(canvas->root());
    QVERIFY(root);

    QCOMPARE(root->pos(), QPointF(0,0));
    QCOMPARE(root->children().at(0)->pos(), QPointF(20,20));
    QCOMPARE(root->children().at(0)->children().at(0)->pos(), QPointF(20,20));
    QCOMPARE(root->children().at(2)->pos(), QPointF(60,20));
    QCOMPARE(root->children().at(3)->pos(), QPointF(20,40));
    QCOMPARE(root->children().at(5)->pos(), QPointF(40,40));
}

void tst_SimpleCanvasItem::test_scenePos()
{
    QFxView *canvas = createView(SRCDIR "/data/test.qml");
    canvas->execute();
    qApp->processEvents();
    QSimpleCanvasItem* root = qobject_cast<QSimpleCanvasItem*>(canvas->root());
    QVERIFY(root);

#ifdef CANVAS_GL
    QCOMPARE(root->transform(), QMatrix4x4());
#else
    QCOMPARE(root->transform(), QTransform());
#endif
    QCOMPARE(root->scenePos(), QPointF(0,0));
    QCOMPARE(root->children().at(0)->scenePos(), QPointF(20,20));
    QCOMPARE(root->children().at(0)->children().at(0)->scenePos(), QPointF(40,40));
    QCOMPARE(root->children().at(2)->scenePos(), QPointF(60,20));
    QCOMPARE(root->children().at(3)->scenePos(), QPointF(20,40));
    QCOMPARE(root->children().at(5)->scenePos(), QPointF(40,40));
}

QFxView *tst_SimpleCanvasItem::createView(const QString &filename)
{
    QFxView *canvas = new QFxView(0);
    canvas->setFixedSize(240,320);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString xml = file.readAll();
    canvas->setQml(xml, filename);

    return canvas;
}

QTEST_MAIN(tst_SimpleCanvasItem)

#include "tst_simplecanvasitem.moc"

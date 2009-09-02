#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QFile>
#include <QtDeclarative/qfxview.h>
#include <qfxtextinput.h>
#include <QDebug>

class tst_qfxtextinput : public QObject

{
    Q_OBJECT
public:
    tst_qfxtextinput();

private slots:
    void navigation();

private:
    void simulateKey(QFxView *, int key);
    QFxView *createView(const QString &filename);

    QmlEngine engine;
};

tst_qfxtextinput::tst_qfxtextinput()
{
}

/*
TextInput element should only handle left/right keys until the cursor reaches
the extent of the text, then they should ignore the keys.
*/
void tst_qfxtextinput::navigation()
{
    QFxView *canvas = createView(SRCDIR "/data/navigation.qml");
    canvas->execute();
    canvas->show();

    QVERIFY(canvas->root() != 0);

    QFxItem *input = qobject_cast<QFxItem *>(qvariant_cast<QObject *>(canvas->root()->property("myInput")));

    QVERIFY(input != 0);
    QVERIFY(input->hasFocus() == true);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasFocus() == false);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasFocus() == true);
    simulateKey(canvas, Qt::Key_Right);
    QVERIFY(input->hasFocus() == false);
    simulateKey(canvas, Qt::Key_Left);
    QVERIFY(input->hasFocus() == true);
}

void tst_qfxtextinput::simulateKey(QFxView *view, int key)
{
    QKeyEvent press(QKeyEvent::KeyPress, key, 0);
    QKeyEvent release(QKeyEvent::KeyRelease, key, 0);

    QApplication::sendEvent(view, &press);
    QApplication::sendEvent(view, &release);
}

QFxView *tst_qfxtextinput::createView(const QString &filename)
{
    QFxView *canvas = new QFxView(0);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString xml = file.readAll();
    canvas->setQml(xml, filename);

    return canvas;
}

QTEST_MAIN(tst_qfxtextinput)

#include "tst_qfxtextinput.moc"

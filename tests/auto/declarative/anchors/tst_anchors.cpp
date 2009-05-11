#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxview.h>
#include <QtDeclarative/qfximage.h>
#include <QtDeclarative/qfxtext.h>

class tst_anchors : public QObject
{
    Q_OBJECT
public:
    tst_anchors() {}

private slots:
    void loops();
};

// mostly testing that we don't crash
void tst_anchors::loops()
{
    {
        QFxView *view = new QFxView;

        view->setUrl(QUrl("file://" SRCDIR "/data/loop1.qml"));

        //### ignoreMessage doesn't seem to work
        //QTest::ignoreMessage(QtWarningMsg, "QML QFxText (unknown location): Anchor loop detected on horizontal anchor.");
        //QTest::ignoreMessage(QtWarningMsg, "QML QFxText (unknown location): Anchor loop detected on horizontal anchor.");
        view->execute();
        qApp->processEvents();

        delete view;
    }

    {
        QFxView *view = new QFxView;

        view->setUrl(QUrl("file://" SRCDIR "/data/loop2.qml"));

        //### ignoreMessage doesn't seem to work
        //QTest::ignoreMessage(QtWarningMsg, "QML QFxImage (unknown location): Anchor loop detected on horizontal anchor.");
        //QTest::ignoreMessage(QtWarningMsg, "QML QFxImage (unknown location): Anchor loop detected on horizontal anchor.");
        view->execute();
        qApp->processEvents();

        delete view;
    }
}

QTEST_MAIN(tst_anchors)

#include "tst_anchors.moc"

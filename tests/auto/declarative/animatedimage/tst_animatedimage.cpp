#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlview.h>
#include <QtDeclarative/qmlgraphicsrect.h>
#include <QtDeclarative/qmlgraphicsimage.h>
#include "qmlgraphicsanimatedimageitem.h"

class tst_animatedimage : public QObject
{
    Q_OBJECT
public:
    tst_animatedimage() {}

private slots:
    void play();
    void pause();
    void setFrame();
    void frameCount();
};

void tst_animatedimage::play()
{
    QmlGraphicsAnimatedImageItem anim;
    anim.setSource(QUrl("file://" SRCDIR "/data/stickman.gif"));
    QVERIFY(anim.isPlaying());
}

void tst_animatedimage::pause()
{
    QmlGraphicsAnimatedImageItem anim;
    anim.setSource(QUrl("file://" SRCDIR "/data/stickman.gif"));
    anim.setPaused(true);
    QVERIFY(!anim.isPlaying());
}

void tst_animatedimage::setFrame()
{
    QmlGraphicsAnimatedImageItem anim;
    anim.setSource(QUrl("file://" SRCDIR "/data/stickman.gif"));
    anim.setPaused(true);
    QVERIFY(!anim.isPlaying());
    anim.setCurrentFrame(2);
    QCOMPARE(anim.currentFrame(), 2);
}

void tst_animatedimage::frameCount()
{
    QmlGraphicsAnimatedImageItem anim;
    anim.setSource(QUrl("file://" SRCDIR "/data/stickman.gif"));
    QCOMPARE(anim.frameCount(), 299);
}

QTEST_MAIN(tst_animatedimage)

#include "tst_animatedimage.moc"

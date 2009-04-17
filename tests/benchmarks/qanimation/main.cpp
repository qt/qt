#include <QtGui>
#include <qtest.h>

#include "dummyobject.h"
#include "dummyanimation.h"
#include "rectanimation.h"

#define ITERATION_COUNT 10e3

class tst_qanimation : public QObject
{
    Q_OBJECT
private slots:
    void itemAnimation();
    void itemAnimation_data() { data();}
    void itemPropertyAnimation();
    void itemPropertyAnimation_data() { data();}
    void dummyAnimation();
    void dummyAnimation_data() { data();}
    void dummyPropertyAnimation();
    void dummyPropertyAnimation_data() { data();}
    void rectAnimation();
    void rectAnimation_data() { data();}

    void floatAnimation_data() { data(); }
    void floatAnimation();

private:
    void data();
};


void tst_qanimation::data()
{
    QTest::addColumn<bool>("paused");
    QTest::newRow("NotRunning") << false;
    QTest::newRow("Paused") << true;
}

void tst_qanimation::itemAnimation()
{
    QFETCH(bool, paused);
    QGraphicsWidget item;

    //first the item animation
    {
        QItemAnimation anim(&item, QItemAnimation::Position);
        anim.setDuration(ITERATION_COUNT);
        anim.setStartValue(QPointF(0,0));
        anim.setEndValue(QPointF(ITERATION_COUNT,ITERATION_COUNT));
        if (paused)
            anim.pause();
        QBENCHMARK {
            for(int i = 0; i < anim.duration(); ++i) {
                anim.setCurrentTime(i);
            }
        }
    }
}

void tst_qanimation::itemPropertyAnimation()
{
    QFETCH(bool, paused);
    QGraphicsWidget item;

    //then the property animation
    {
        QPropertyAnimation anim(&item, "pos");
        anim.setDuration(ITERATION_COUNT);
        anim.setStartValue(QPointF(0,0));
        anim.setEndValue(QPointF(ITERATION_COUNT,ITERATION_COUNT));
        if (paused)
            anim.pause();
        QBENCHMARK {
            for(int i = 0; i < ITERATION_COUNT; ++i) {
                anim.setCurrentTime(i);
            }
        }
    }

}

void tst_qanimation::dummyAnimation()
{
    QFETCH(bool, paused);
    DummyObject dummy;

    //first the dummy animation
    {
        DummyAnimation anim(&dummy);
        anim.setDuration(ITERATION_COUNT);
        anim.setStartValue(QRect(0, 0, 0, 0));
        anim.setEndValue(QRect(0, 0, ITERATION_COUNT,ITERATION_COUNT));
        if (paused)
            anim.pause();
        QBENCHMARK {
            for(int i = 0; i < anim.duration(); ++i) {
                anim.setCurrentTime(i);
            }
        }
    }
}

void tst_qanimation::dummyPropertyAnimation()
{
    QFETCH(bool, paused);
    DummyObject dummy;

    //then the property animation
    {
        QPropertyAnimation anim(&dummy, "rect");
        anim.setDuration(ITERATION_COUNT);
        anim.setStartValue(QRect(0, 0, 0, 0));
        anim.setEndValue(QRect(0, 0, ITERATION_COUNT,ITERATION_COUNT));
        if (paused)
            anim.pause();
        QBENCHMARK {
            for(int i = 0; i < ITERATION_COUNT; ++i) {
                anim.setCurrentTime(i);
            }
        }
    }
}

void tst_qanimation::rectAnimation()
{
    //this is the simplest animation you can do
    QFETCH(bool, paused);
    DummyObject dummy;

    //then the property animation
    {
        RectAnimation anim(&dummy);
        anim.setDuration(ITERATION_COUNT);
        anim.setStartValue(QRect(0, 0, 0, 0));
        anim.setEndValue(QRect(0, 0, ITERATION_COUNT,ITERATION_COUNT));
        if (paused)
            anim.pause();
        QBENCHMARK {
            for(int i = 0; i < ITERATION_COUNT; ++i) {
                anim.setCurrentTime(i);
            }
        }
    }
}

void tst_qanimation::floatAnimation()
{
    //this is the simplest animation you can do
    QFETCH(bool, paused);
    DummyObject dummy;

    //then the property animation
    {
        QPropertyAnimation anim(&dummy, "opacity");
        anim.setDuration(ITERATION_COUNT);
        anim.setStartValue(0.f);
        anim.setEndValue(1.f);
        if (paused)
            anim.pause();
        QBENCHMARK {
            for(int i = 0; i < ITERATION_COUNT; ++i) {
                anim.setCurrentTime(i);
            }
        }
    }
}



QTEST_MAIN(tst_qanimation)

#include "main.moc"

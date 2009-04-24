#include <QtGui>
#include <QtTest>

#include "ui_form.h"
#include "touchwidget.h"

class MultitouchTestWidget : public QWidget, public Ui::Form
{
    Q_OBJECT

public:
    MultitouchTestWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_QuitOnClose, false);\
        setupUi(this);
    }

    void closeEvent(QCloseEvent *event)
    {
        event->accept();
        QTimer::singleShot(1000, qApp, SLOT(quit()));
    }
};

class tst_ManualMultitouch : public QObject
{
    Q_OBJECT

public:
    tst_ManualMultitouch();
    ~tst_ManualMultitouch();

private slots:
    void basicEventHandling();
    void touchEventPropagation();
};

tst_ManualMultitouch::tst_ManualMultitouch()
{ }

tst_ManualMultitouch::~tst_ManualMultitouch()
{ }

void tst_ManualMultitouch::basicEventHandling()
{
    // first, make sure that we get mouse events when not enabling touch events
    MultitouchTestWidget testWidget;
    testWidget.testNameLabel->setText("Basic QTouchEvent handling test");
    testWidget.testDescriptionLabel->setText("Touch, hold, and release your finger on the green widget.");
    testWidget.redWidget->hide();
    testWidget.blueWidget->hide();
    testWidget.greenWidget->closeWindowOnMouseRelease = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(!testWidget.greenWidget->seenTouchBegin);
    QVERIFY(!testWidget.greenWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greenWidget->seenTouchEnd);
    QVERIFY(testWidget.greenWidget->seenMousePress);
    // QVERIFY(testWidget.greenWidget->seenMouseMove);
    QVERIFY(testWidget.greenWidget->seenMouseRelease);

    // now enable touch and make sure we get the touch events
    testWidget.greenWidget->reset();
    testWidget.greenWidget->setAttribute(Qt::WA_AcceptTouchEvents);
    testWidget.greenWidget->acceptTouchBegin = true;
    testWidget.greenWidget->acceptTouchUpdate = true;
    testWidget.greenWidget->acceptTouchEnd = true;
    testWidget.greenWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.greenWidget->seenTouchBegin
            && testWidget.greenWidget->seenTouchUpdate
            && testWidget.greenWidget->seenTouchEnd);
    QVERIFY(!testWidget.greenWidget->seenMousePress
            && !testWidget.greenWidget->seenMouseMove
            && !testWidget.greenWidget->seenMouseRelease);

    // again, ignoring the TouchEnd
    testWidget.greenWidget->reset();
    testWidget.greenWidget->acceptTouchBegin = true;
    testWidget.greenWidget->acceptTouchUpdate = true;
    // testWidget.greenWidget->acceptTouchEnd = true;
    testWidget.greenWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.greenWidget->seenTouchBegin);
    QVERIFY(testWidget.greenWidget->seenTouchUpdate);
    QVERIFY(testWidget.greenWidget->seenTouchEnd);
    QVERIFY(!testWidget.greenWidget->seenMousePress);
    QVERIFY(!testWidget.greenWidget->seenMouseMove);
    QVERIFY(!testWidget.greenWidget->seenMouseRelease);

    // again, ignoring TouchUpdates
    testWidget.greenWidget->reset();
    testWidget.greenWidget->acceptTouchBegin = true;
    // testWidget.greenWidget->acceptTouchUpdate = true;
    testWidget.greenWidget->acceptTouchEnd = true;
    testWidget.greenWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.greenWidget->seenTouchBegin);
    QVERIFY(testWidget.greenWidget->seenTouchUpdate);
    QVERIFY(testWidget.greenWidget->seenTouchEnd);
    QVERIFY(!testWidget.greenWidget->seenMousePress);
    QVERIFY(!testWidget.greenWidget->seenMouseMove);
    QVERIFY(!testWidget.greenWidget->seenMouseRelease);

    // last time, ignoring TouchUpdates and TouchEnd
    testWidget.greenWidget->reset();
    testWidget.greenWidget->acceptTouchBegin = true;
    // testWidget.greenWidget->acceptTouchUpdate = true;
    // testWidget.greenWidget->acceptTouchEnd = true;
    testWidget.greenWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.greenWidget->seenTouchBegin);
    QVERIFY(testWidget.greenWidget->seenTouchUpdate);
    QVERIFY(testWidget.greenWidget->seenTouchEnd);
    QVERIFY(!testWidget.greenWidget->seenMousePress);
    QVERIFY(!testWidget.greenWidget->seenMouseMove);
    QVERIFY(!testWidget.greenWidget->seenMouseRelease);
}

void tst_ManualMultitouch::touchEventPropagation()
{
    MultitouchTestWidget testWidget;
    testWidget.testNameLabel->setText("QTouchEvent propagation test, Blue blocks Green");
    testWidget.testDescriptionLabel->setText("Touch, hold, and release your finger on the blue widget.");
    testWidget.redWidget->hide();

    // test that accepting the TouchBegin event on the
    // blueWidget stops propagation to the greenWidget
    testWidget.blueWidget->setAttribute(Qt::WA_AcceptTouchEvents);
    testWidget.greenWidget->setAttribute(Qt::WA_AcceptTouchEvents);
    testWidget.blueWidget->acceptTouchBegin = true;
    testWidget.blueWidget->acceptTouchUpdate = true;
    testWidget.blueWidget->acceptTouchEnd = true;
    testWidget.blueWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.blueWidget->seenTouchBegin);
    QVERIFY(testWidget.blueWidget->seenTouchUpdate);
    QVERIFY(testWidget.blueWidget->seenTouchEnd);
    QVERIFY(!testWidget.blueWidget->seenMousePress);
    QVERIFY(!testWidget.blueWidget->seenMouseMove);
    QVERIFY(!testWidget.blueWidget->seenMouseRelease);
    QVERIFY(!testWidget.greenWidget->seenTouchBegin);
    QVERIFY(!testWidget.greenWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greenWidget->seenTouchEnd);
    QVERIFY(!testWidget.greenWidget->seenMousePress);
    QVERIFY(!testWidget.greenWidget->seenMouseMove);
    QVERIFY(!testWidget.greenWidget->seenMouseRelease);

    // ignoring TouchEnd
    testWidget.blueWidget->reset();
    testWidget.greenWidget->reset();
    testWidget.blueWidget->acceptTouchBegin = true;
    testWidget.blueWidget->acceptTouchUpdate = true;
    // testWidget.blueWidget->acceptTouchEnd = true;
    testWidget.blueWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.blueWidget->seenTouchBegin);
    QVERIFY(testWidget.blueWidget->seenTouchUpdate);
    QVERIFY(testWidget.blueWidget->seenTouchEnd);
    QVERIFY(!testWidget.blueWidget->seenMousePress);
    QVERIFY(!testWidget.blueWidget->seenMouseMove);
    QVERIFY(!testWidget.blueWidget->seenMouseRelease);
    QVERIFY(!testWidget.greenWidget->seenTouchBegin);
    QVERIFY(!testWidget.greenWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greenWidget->seenTouchEnd);
    QVERIFY(!testWidget.greenWidget->seenMousePress);
    QVERIFY(!testWidget.greenWidget->seenMouseMove);
    QVERIFY(!testWidget.greenWidget->seenMouseRelease);

    // ignoring TouchUpdate
    testWidget.blueWidget->reset();
    testWidget.greenWidget->reset();
    testWidget.blueWidget->acceptTouchBegin = true;
    // testWidget.blueWidget->acceptTouchUpdate = true;
    testWidget.blueWidget->acceptTouchEnd = true;
    testWidget.blueWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.blueWidget->seenTouchBegin);
    QVERIFY(testWidget.blueWidget->seenTouchUpdate);
    QVERIFY(testWidget.blueWidget->seenTouchEnd);
    QVERIFY(!testWidget.blueWidget->seenMousePress);
    QVERIFY(!testWidget.blueWidget->seenMouseMove);
    QVERIFY(!testWidget.blueWidget->seenMouseRelease);
    QVERIFY(!testWidget.greenWidget->seenTouchBegin);
    QVERIFY(!testWidget.greenWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greenWidget->seenTouchEnd);
    QVERIFY(!testWidget.greenWidget->seenMousePress);
    QVERIFY(!testWidget.greenWidget->seenMouseMove);
    QVERIFY(!testWidget.greenWidget->seenMouseRelease);

    // ignoring TouchUpdate and TouchEnd
    testWidget.blueWidget->reset();
    testWidget.greenWidget->reset();
    testWidget.blueWidget->acceptTouchBegin = true;
    // testWidget.blueWidget->acceptTouchUpdate = true;
    // testWidget.blueWidget->acceptTouchEnd = true;
    testWidget.blueWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.blueWidget->seenTouchBegin);
    QVERIFY(testWidget.blueWidget->seenTouchUpdate);
    QVERIFY(testWidget.blueWidget->seenTouchEnd);
    QVERIFY(!testWidget.blueWidget->seenMousePress);
    QVERIFY(!testWidget.blueWidget->seenMouseMove);
    QVERIFY(!testWidget.blueWidget->seenMouseRelease);
    QVERIFY(!testWidget.greenWidget->seenTouchBegin);
    QVERIFY(!testWidget.greenWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greenWidget->seenTouchEnd);
    QVERIFY(!testWidget.greenWidget->seenMousePress);
    QVERIFY(!testWidget.greenWidget->seenMouseMove);
    QVERIFY(!testWidget.greenWidget->seenMouseRelease);

    // repeat the test above, now ignoring touch events in the
    // greyWidget, they should be propagated to the redWidget
    testWidget.testNameLabel->setText("QTouchEvent propagation test, Red handles Grey's ignored events");
    testWidget.testDescriptionLabel->setText("Touch, hold, and release your finger on the grey widget.");
    testWidget.greenWidget->hide();
    testWidget.redWidget->show();
    testWidget.greyWidget->setAttribute(Qt::WA_AcceptTouchEvents, false);
    testWidget.redWidget->setAttribute(Qt::WA_AcceptTouchEvents);
    testWidget.greyWidget->reset();
    testWidget.redWidget->reset();
    testWidget.redWidget->acceptTouchBegin = true;
    testWidget.redWidget->acceptTouchUpdate = true;
    testWidget.redWidget->acceptTouchEnd = true;
    testWidget.redWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(!testWidget.greyWidget->seenTouchBegin);
    QVERIFY(!testWidget.greyWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greyWidget->seenTouchEnd);
    QVERIFY(!testWidget.greyWidget->seenMousePress);
    QVERIFY(!testWidget.greyWidget->seenMouseMove);
    QVERIFY(!testWidget.greyWidget->seenMouseRelease);
    QVERIFY(testWidget.redWidget->seenTouchBegin);
    QVERIFY(testWidget.redWidget->seenTouchUpdate);
    QVERIFY(testWidget.redWidget->seenTouchEnd);
    QVERIFY(!testWidget.redWidget->seenMousePress);
    QVERIFY(!testWidget.redWidget->seenMouseMove);
    QVERIFY(!testWidget.redWidget->seenMouseRelease);

    // again, but this time greyWidget should see the TouchBegin
    testWidget.greyWidget->reset();
    testWidget.redWidget->reset();
    testWidget.greyWidget->setAttribute(Qt::WA_AcceptTouchEvents);
    testWidget.redWidget->acceptTouchBegin = true;
    testWidget.redWidget->acceptTouchUpdate = true;
    testWidget.redWidget->acceptTouchEnd = true;
    testWidget.redWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.greyWidget->seenTouchBegin);
    QVERIFY(!testWidget.greyWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greyWidget->seenTouchEnd);
    QVERIFY(!testWidget.greyWidget->seenMousePress);
    QVERIFY(!testWidget.greyWidget->seenMouseMove);
    QVERIFY(!testWidget.greyWidget->seenMouseRelease);
    QVERIFY(testWidget.redWidget->seenTouchBegin);
    QVERIFY(testWidget.redWidget->seenTouchUpdate);
    QVERIFY(testWidget.redWidget->seenTouchEnd);
    QVERIFY(!testWidget.redWidget->seenMousePress);
    QVERIFY(!testWidget.redWidget->seenMouseMove);
    QVERIFY(!testWidget.redWidget->seenMouseRelease);

    // again, ignoring the TouchEnd
    testWidget.greyWidget->reset();
    testWidget.redWidget->reset();
    testWidget.redWidget->acceptTouchBegin = true;
    testWidget.redWidget->acceptTouchUpdate = true;
    // testWidget.redWidget->acceptTouchEnd = true;
    testWidget.redWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.greyWidget->seenTouchBegin);
    QVERIFY(!testWidget.greyWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greyWidget->seenTouchEnd);
    QVERIFY(!testWidget.greyWidget->seenMousePress);
    QVERIFY(!testWidget.greyWidget->seenMouseMove);
    QVERIFY(!testWidget.greyWidget->seenMouseRelease);
    QVERIFY(testWidget.redWidget->seenTouchBegin);
    QVERIFY(testWidget.redWidget->seenTouchUpdate);
    QVERIFY(testWidget.redWidget->seenTouchEnd);
    QVERIFY(!testWidget.redWidget->seenMousePress);
    QVERIFY(!testWidget.redWidget->seenMouseMove);
    QVERIFY(!testWidget.redWidget->seenMouseRelease);

    // again, ignoring TouchUpdates
    testWidget.greyWidget->reset();
    testWidget.redWidget->reset();
    testWidget.redWidget->acceptTouchBegin = true;
    // testWidget.redWidget->acceptTouchUpdate = true;
    testWidget.redWidget->acceptTouchEnd = true;
    testWidget.redWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.greyWidget->seenTouchBegin);
    QVERIFY(!testWidget.greyWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greyWidget->seenTouchEnd);
    QVERIFY(!testWidget.greyWidget->seenMousePress);
    QVERIFY(!testWidget.greyWidget->seenMouseMove);
    QVERIFY(!testWidget.greyWidget->seenMouseRelease);
    QVERIFY(testWidget.redWidget->seenTouchBegin);
    QVERIFY(testWidget.redWidget->seenTouchUpdate);
    QVERIFY(testWidget.redWidget->seenTouchEnd);
    QVERIFY(!testWidget.redWidget->seenMousePress);
    QVERIFY(!testWidget.redWidget->seenMouseMove);
    QVERIFY(!testWidget.redWidget->seenMouseRelease);

    // last time, ignoring TouchUpdates and TouchEnd
    testWidget.greyWidget->reset();
    testWidget.redWidget->reset();
    testWidget.redWidget->acceptTouchBegin = true;
    // testWidget.redWidget->acceptTouchUpdate = true;
    // testWidget.redWidget->acceptTouchEnd = true;
    testWidget.redWidget->closeWindowOnTouchEnd = true;
    testWidget.showMaximized();

    (void) qApp->exec();
    QVERIFY(testWidget.greyWidget->seenTouchBegin);
    QVERIFY(!testWidget.greyWidget->seenTouchUpdate);
    QVERIFY(!testWidget.greyWidget->seenTouchEnd);
    QVERIFY(!testWidget.greyWidget->seenMousePress);
    QVERIFY(!testWidget.greyWidget->seenMouseMove);
    QVERIFY(!testWidget.blueWidget->seenMouseRelease);
    QVERIFY(testWidget.redWidget->seenTouchBegin);
    QVERIFY(testWidget.redWidget->seenTouchUpdate);
    QVERIFY(testWidget.redWidget->seenTouchEnd);
    QVERIFY(!testWidget.redWidget->seenMousePress);
    QVERIFY(!testWidget.redWidget->seenMouseMove);
    QVERIFY(!testWidget.redWidget->seenMouseRelease);
}

QTEST_MAIN(tst_ManualMultitouch)

#include "main.moc"

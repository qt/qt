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
        setupUi(this);
    }
};

class tst_ManualMultitouch : public QObject
{
    Q_OBJECT

public:
    tst_ManualMultitouch();
    ~tst_ManualMultitouch();

private slots:
    void touchBeginPropagation();
};

tst_ManualMultitouch::tst_ManualMultitouch()
{ }

tst_ManualMultitouch::~tst_ManualMultitouch()
{ }

void tst_ManualMultitouch::touchBeginPropagation()
{
    MultitouchTestWidget testWidget;
    testWidget.testNameLabel->setText("Touch event propagation");
    testWidget.testDescriptionLabel->setText("Touch, move, and release your finger over the green widget.");
    testWidget.greenWidget->setAttribute(Qt::WA_AcceptTouchEvents);
    testWidget.greenWidget->acceptTouchBegin = true;
    testWidget.greenWidget->acceptTouchUpdate = true;
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

QTEST_MAIN(tst_ManualMultitouch)

#include "main.moc"

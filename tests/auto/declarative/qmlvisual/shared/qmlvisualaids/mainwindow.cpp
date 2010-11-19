#include "mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), testIdx(-1)
{
    createMenus();
    view = new QDeclarativeView(this);
    setCentralWidget(view);
    view->setResizeMode(QDeclarativeView::SizeViewToRootObject);
    curTest = new TestModel(this);
    connect(curTest, SIGNAL(moveOn()),
            this, SLOT(runTests()));

    view->engine()->rootContext()->setContextProperty("test", curTest);
    view->setSource(QUrl("qrc:qml/Comparison.qml"));
}

void MainWindow::runAllTests()
{
    tests.clear();
    testIdx = 0;

    QString visualTest = "./tst_qmlvisual";//TODO: Crossplatform

    QProcess p;//TODO: Error checking here
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(visualTest, QStringList());
    p.waitForFinished(-1);//Can't time out, because it takes an indeterminate and long time

    QString output = QString(p.readAllStandardOutput());
    QRegExp re("QDeclarativeTester\\( \"([^\"]*)\" \\)");
    int offset=0;
    while((offset = re.indexIn(output, offset)) != -1){
        tests << re.cap(1);
        offset++;
    }

    if(tests.count())
        QMessageBox::information(this, "Test Results", QString("Tests completed. %1 test failures occurred.").arg(tests.count()));
    else
        QMessageBox::information(this, "Test Results", "Tests completed. All tests passed!");

    runTests();
}

void MainWindow::runTests()
{
    if(testIdx >= tests.size())
        testIdx = -1;
    if(testIdx == -1)
        return;
    showFixScreen(tests[testIdx++]);
}

void MainWindow::showFixScreen(const QString &path)
{
    if(curTest->setTest(path)){
        view->engine()->rootContext()->setContextProperty("test", curTest); //signal connects to runTests
    }else{
        QMessageBox::critical(this, "Test Error", QString("Cannot find test %1.").arg(path));
        runTests();
    }
}

void MainWindow::createMenus()
{
    QMenu *tests = this->menuBar()->addMenu("Tests");
    tests->addAction("Run All", this, SLOT(runAllTests()));
    tests->addSeparator();
    tests->addAction("About Qt...", qApp, SLOT(aboutQt()));
    tests->addAction("Quit", qApp, SLOT(quit()));
}

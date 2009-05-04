#include <QtTest/QtTest>
#include <QtCore/QDate>
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtGui>
#include <windows.h>

int main(int argc, char * argv[])
{
    int widgetNum = 20;

    QList<QWidget*> widgets;
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Test");
    QMenu *fileMenu = mainWindow.menuBar()->addMenu("File");
    QMenu *editMenu = mainWindow.menuBar()->addMenu("Edit");
    QMenu *viewMenu = mainWindow.menuBar()->addMenu("View");
    QMenu *toolsMenu = mainWindow.menuBar()->addMenu("Tools");
    QMenu *optionsMenu = mainWindow.menuBar()->addMenu("Options");
    QMenu *helpMenu = mainWindow.menuBar()->addMenu("Help");

    qApp->processEvents();

    fileMenu->addAction("Open");
    QAction *close = fileMenu->addAction("Close");
    fileMenu->addSeparator();
    fileMenu->addAction("Exit");

    close->setEnabled(false);

    editMenu->addAction("Cut");
    editMenu->addAction("Pase");
    editMenu->addAction("Copy");
    editMenu->addSeparator();
    editMenu->addAction("Find");

    viewMenu->addAction("Hide");
    viewMenu->addAction("Show");
    viewMenu->addAction("Explore");
    QAction *visible = viewMenu->addAction("Visible");
    visible->setCheckable(true);
    visible->setChecked(true);

    toolsMenu->addMenu("Hammer");
    toolsMenu->addMenu("Caliper");
    toolsMenu->addMenu("Helm");

    optionsMenu->addMenu("Settings");
    optionsMenu->addMenu("Standard");
    optionsMenu->addMenu("Extended");
    
    QMenu *subMenu = helpMenu->addMenu("Help");
    subMenu->addAction("Index");
    subMenu->addSeparator();
    subMenu->addAction("Vodoo Help");
    helpMenu->addAction("Contens");
    helpMenu->addSeparator();
    helpMenu->addAction("About");

    QToolBar toolbar;
    mainWindow.addToolBar(&toolbar);
    toolbar.addAction(QIcon(qApp->style()->standardPixmap(QStyle::SP_FileIcon)), QString("textAction"));

    QTextEdit textEdit;
    mainWindow.setCentralWidget(&textEdit);

    mainWindow.showMaximized();

    app.exec();
}

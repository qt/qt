#include "mainwindow.h"
#include "animationdialog.h"
#include "stickman.h"

#include <QMenuBar>
#include <QApplication>

MainWindow::MainWindow(StickMan *stickMan)
{
    initActions(stickMan);
}

MainWindow::~MainWindow()
{
}

void MainWindow::initActions(StickMan *stickMan)
{
    AnimationDialog *dialog = new AnimationDialog(stickMan, this);
    dialog->show();

    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *loadAction = fileMenu->addAction("&Open");
    QAction *saveAction = fileMenu->addAction("&Save");
    QAction *exitAction = fileMenu->addAction("E&xit");

    QMenu *animationMenu = menuBar()->addMenu("&Animation");
    QAction *newAnimationAction = animationMenu->addAction("&New animation");

    connect(loadAction, SIGNAL(triggered()), dialog, SLOT(loadAnimation()));
    connect(saveAction, SIGNAL(triggered()), dialog, SLOT(saveAnimation()));
    connect(exitAction, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
    connect(newAnimationAction, SIGNAL(triggered()), dialog, SLOT(newAnimation()));

}

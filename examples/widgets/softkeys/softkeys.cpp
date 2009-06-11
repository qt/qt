#include "softkeys.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)      
{
    central = new QWidget(this);
    setCentralWidget(central);
    infoLabel = new QLabel(tr("Funky stuff in menu!"));

    layout = new QVBoxLayout;
    layout->addWidget(infoLabel);
    central->setLayout(layout);
    central->setFocusPolicy(Qt::TabFocus);
    
    fileMenu = menuBar()->addMenu(tr("&File"));
    openDialogAct = new QAction(tr("&Open Dialog"), this);
    addSoftKeysAct = new QAction(tr("&Add Softkeys"), this);
    clearSoftKeysAct = new QAction(tr("&Clear Softkeys"), this);
    fileMenu->addAction(openDialogAct);
    fileMenu->addAction(addSoftKeysAct);
    fileMenu->addAction(clearSoftKeysAct);
    connect(openDialogAct, SIGNAL(triggered()), this, SLOT(openDialog()));    
    connect(addSoftKeysAct, SIGNAL(triggered()), this, SLOT(addSoftKeys()));    
    connect(clearSoftKeysAct, SIGNAL(triggered()), this, SLOT(clearSoftKeys()));    
}

MainWindow::~MainWindow()
{
}

void MainWindow::openDialog()
{
    QFileDialog::getOpenFileName(this);
}

void MainWindow::addSoftKeys()
{
    ok = new QAction(tr("Ok"), this);
    ok->setSoftKeyRole(QAction::OkSoftKey);
    connect(ok, SIGNAL(triggered()), this, SLOT(okPressed()));  
    
    cancel = new QAction(tr("Cancel"), this);
    cancel->setSoftKeyRole(QAction::OkSoftKey);
    connect(cancel, SIGNAL(triggered()), this, SLOT(cancelPressed()));  

    QList<QAction*> softkeys;
    softkeys.append(ok);
    softkeys.append(cancel);
    central->setSoftKeys(softkeys);
    central->setFocus();
    
}

void MainWindow::clearSoftKeys()
{
    central->setSoftKey(0);
}

void MainWindow::okPressed()
{
    infoLabel->setText(tr("OK pressed"));
    central->setSoftKey(0);
}

void MainWindow::cancelPressed()
{
    infoLabel->setText(tr("Cancel pressed"));
    central->setSoftKey(0);
}

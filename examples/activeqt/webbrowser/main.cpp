/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QMainWindow>
#include <QAbstractEventDispatcher>

#if defined(Q_WS_WINCE_WM)
#include "ui_mainwindow_windowsmobile.h"
#include <windows.h>
#else
#include "ui_mainwindow.h"
#endif

//! [0]
class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindow();

public slots:
    void on_WebBrowser_TitleChange(const QString &title);
    void on_WebBrowser_ProgressChange(int a, int b);
    void on_WebBrowser_CommandStateChange(int cmd, bool on);
    void on_WebBrowser_BeforeNavigate();
    void on_WebBrowser_NavigateComplete(QString);

    void on_actionGo_triggered();
    void on_actionNewWindow_triggered();
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();
    void on_actionFileClose_triggered();

private:
    QProgressBar *pb;
};
//! [0] //! [1]

MainWindow::MainWindow()
{
    setupUi(this);

    connect(addressEdit, SIGNAL(returnPressed()), actionGo, SLOT(trigger()));
    connect(actionBack, SIGNAL(triggered()), WebBrowser, SLOT(GoBack()));
    connect(actionForward, SIGNAL(triggered()), WebBrowser, SLOT(GoForward()));
    connect(actionStop, SIGNAL(triggered()), WebBrowser, SLOT(Stop()));
    connect(actionRefresh, SIGNAL(triggered()), WebBrowser, SLOT(Refresh()));
    connect(actionHome, SIGNAL(triggered()), WebBrowser, SLOT(GoHome()));
    connect(actionSearch, SIGNAL(triggered()), WebBrowser, SLOT(GoSearch()));

    pb = new QProgressBar(statusBar());
    pb->setTextVisible(false);
    pb->hide();
    statusBar()->addPermanentWidget(pb);

    WebBrowser->dynamicCall("GoHome()");
}

//! [1] //! [2]
void MainWindow::on_WebBrowser_TitleChange(const QString &title)
{
    setWindowTitle("Qt WebBrowser - " + title);
}

void MainWindow::on_WebBrowser_ProgressChange(int a, int b)
{
    if (a <= 0 || b <= 0) {
	pb->hide();
	return;
    }
    pb->show();
    pb->setRange(0, b);
    pb->setValue(a);
}

void MainWindow::on_WebBrowser_CommandStateChange(int cmd, bool on)
{
    switch (cmd) {
    case 1:
	actionForward->setEnabled(on);
	break;
    case 2:
	actionBack->setEnabled(on);
	break;
    }
}

void MainWindow::on_WebBrowser_BeforeNavigate()
{
    actionStop->setEnabled(true);
}

void MainWindow::on_WebBrowser_NavigateComplete(QString)
{
    actionStop->setEnabled(false);
}

//! [2] //! [3]
void MainWindow::on_actionGo_triggered()
{
    WebBrowser->dynamicCall("Navigate(const QString&)", addressEdit->text());
}

void MainWindow::on_actionNewWindow_triggered()
{
    MainWindow *window = new MainWindow;
    window->show();
    if (addressEdit->text().isEmpty())
	return;
    window->addressEdit->setText(addressEdit->text());
    window->actionStop->setEnabled(true);
    window->on_actionGo_triggered();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About WebBrowser"),
		tr("This Example has been created using the ActiveQt integration into Qt Designer.\n"
		   "It demonstrates the use of QAxWidget to embed the Internet Explorer ActiveX\n"
		   "control into a Qt application."));
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::on_actionFileClose_triggered()
{
    close();
}

#include "main.moc"

//! [3] //! [4]
int main(int argc, char ** argv)
{
    QApplication a(argc, argv);
    MainWindow w;
#if defined(Q_OS_WINCE)
    w.showMaximized();
#else
    w.show();
#endif
    return a.exec();
}
//! [4]

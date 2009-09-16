#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QTimer;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void keyPressEvent(QKeyEvent* event);
    
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

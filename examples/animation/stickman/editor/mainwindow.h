#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class StickMan;
class MainWindow: public QMainWindow
{
public:
    MainWindow(StickMan *stickMan);
    ~MainWindow();

private:
    void initActions(StickMan *stickMan);
};

#endif

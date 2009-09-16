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

private slots:
    void toggleOverrideCursor();

private:
    void keyPressEvent(QKeyEvent* event);
    
    Ui::MainWindow *ui;
    QTimer *timer;
    int override;
    
    QCursor ccurs;
    QCursor bcurs;
};

#endif // MAINWINDOW_H

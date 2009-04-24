#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

class QGraphicsScene;
class QStateMachine;
class QState;
class TankItem;
class MainWindow: public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addTank();
    void runStep();

signals:
    void gameOver();
    void mapFull();

private:
    void init();
    void addWall(const QRectF &wall);

    QGraphicsScene *m_scene;
    
    QStateMachine *m_machine;
    QState *m_runningState;

    QList<TankItem *> m_spawns;
    QTime m_time;

};

#endif


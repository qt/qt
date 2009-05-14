#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

class QGraphicsScene;
class QStateMachine;
class QState;
class GameOverTransition;
class TankItem;
class MainWindow: public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(bool started READ started WRITE setStarted)
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setStarted(bool b) { m_started = b; }
    bool started() const { return m_started; }

public slots:
    void addTank();
    void addRocket();
    void runStep();
    void gameOver();

signals:
    void mapFull();

private:
    void init();
    void addWall(const QRectF &wall);

    QGraphicsScene *m_scene;
    
    QStateMachine *m_machine;
    QState *m_runningState;
    GameOverTransition *m_gameOverTransition;

    QList<TankItem *> m_spawns;
    QTime m_time;
    
    bool m_started : 1;
};

#endif


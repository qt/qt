#ifndef GAMEOVERTRANSITION_H
#define GAMEOVERTRANSITION_H

#include <QSignalTransition>

class TankItem;
class GameOverTransition: public QSignalTransition
{
    Q_OBJECT
public:
    GameOverTransition(QAbstractState *targetState);

    void addTankItem(TankItem *tankItem);

protected:
    bool eventTest(QEvent *event);

private:
    QList<TankItem *> m_tankItems;
};

#endif

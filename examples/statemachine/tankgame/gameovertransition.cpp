#include "gameovertransition.h"
#include "tankitem.h"

#include <QSignalEvent>
#include <QSignalMapper>

GameOverTransition::GameOverTransition(QAbstractState *targetState)
    : QSignalTransition(new QSignalMapper(), SIGNAL(mapped(QObject*)))
{
    setTargetState(targetState);
    
    QSignalMapper *mapper = qobject_cast<QSignalMapper *>(senderObject());
    mapper->setParent(this);
}

void GameOverTransition::addTankItem(TankItem *tankItem)
{
    m_tankItems.append(tankItem);

    QSignalMapper *mapper = qobject_cast<QSignalMapper *>(senderObject());
    mapper->setMapping(tankItem, tankItem);
    connect(tankItem, SIGNAL(aboutToBeDestroyed()), mapper, SLOT(map()));
}

bool GameOverTransition::eventTest(QEvent *e)
{
    bool ret = QSignalTransition::eventTest(e);

    if (ret) {
        QSignalEvent *signalEvent = static_cast<QSignalEvent *>(e);
        QObject *sender = qvariant_cast<QObject *>(signalEvent->arguments().at(0));
        TankItem *tankItem = qobject_cast<TankItem *>(sender);
        m_tankItems.removeAll(tankItem);

        return m_tankItems.size() <= 1;
    } else {
        return false;
    }
}
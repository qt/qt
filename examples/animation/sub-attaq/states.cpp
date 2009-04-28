/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//Own
#include "states.h"
#include "graphicsscene.h"
#include "boat.h"
#include "submarine.h"
#include "torpedo.h"
#include "animationmanager.h"

//Qt
#include <QMessageBox>
#include <QGraphicsView>
#if defined(QT_EXPERIMENTAL_SOLUTION)
#include "qstatemachine.h"
#include "qkeyeventtransition.h"
#include "qsignalevent.h"
#include "qfinalstate.h"
#else
#include <QtCore/QStateMachine>
#include <QtGui/QKeyEventTransition>
#include <QtCore/QSignalEvent>
#include <QFinalState>
#endif

PlayState::PlayState(GraphicsScene *scene, QState *parent)
    : QState(parent),
    scene(scene),
    machine(0),
    currentLevel(0),
    score(0)
{
}

PlayState::~PlayState()
{
}

void PlayState::onEntry()
{
    //We are now playing?
    if (machine) {
        machine->stop();
        scene->clearScene();
        delete machine;
    }

    machine = new QStateMachine(this);

    //This state is when player is playing
    QState *playState = new QState(machine->rootState());

    initializeLevel();

    //This state is when the game is paused
    PauseState *pauseState = new PauseState(scene, machine->rootState());

    //We have one view, it receive the key press event
    QKeyEventTransition *pressPplay = new QKeyEventTransition(scene->views().at(0), QEvent::KeyPress, Qt::Key_P);
    pressPplay->setTargetState(pauseState);
    QKeyEventTransition *pressPpause = new QKeyEventTransition(scene->views().at(0), QEvent::KeyPress, Qt::Key_P);
    pressPpause->setTargetState(playState);

    //Pause "P" is triggered, the player pause the game
    playState->addTransition(pressPplay);

    //To get back playing when the game has been paused
    pauseState->addTransition(pressPpause);

    //This state is when player have lost
    LostState *lostState = new LostState(scene, this, machine->rootState());

    //This state is when player have won
    WinState *winState = new WinState(scene, this, machine->rootState());

    //The boat has been destroyed then the game is finished
    playState->addTransition(scene->boat, SIGNAL(boatExecutionFinished()),lostState);

    //This transition check if we won or not
    WinTransition *winTransition = new WinTransition(scene, this, winState);

    //The boat has been destroyed then the game is finished
    playState->addTransition(winTransition);

    //This state is an animation when the score changed
    UpdateScoreState *scoreState = new UpdateScoreState(this, machine->rootState());

    //This transition update the score when a submarine die
    UpdateScoreTransition *scoreTransition = new UpdateScoreTransition(scene, this, scoreState);

    //The boat has been destroyed then the game is finished
    playState->addTransition(scoreTransition);

    //We go back to play state
    scoreState->addTransition(playState);

    //We start playing!!!
    machine->setInitialState(playState);

    //Final state
    QFinalState *final = new QFinalState(machine->rootState());

    //We win we should reach the final state
    winState->addTransition(winState, SIGNAL(finished()), final);

    //We lost we should reach the final state
    lostState->addTransition(lostState, SIGNAL(finished()), final);

    machine->start();
}

void PlayState::initializeLevel()
{
    scene->boat = new Boat();
    scene->addItem(scene->boat);
    scene->setFocusItem(scene->boat,Qt::OtherFocusReason);
    scene->boat->setPos(scene->width()/2, scene->sealLevel() - scene->boat->size().height());

    GraphicsScene::LevelDescription currentLevelDescription = scene->levelsData.value(currentLevel);

    for (int i = 0; i < currentLevelDescription.submarines.size(); ++i ) {

        QPair<int,int> subContent = currentLevelDescription.submarines.at(i);
        GraphicsScene::SubmarineDescription submarineDesc = scene->submarinesData.at(subContent.first);

        for (int j = 0; j < subContent.second; ++j ) {
            SubMarine *sub = new SubMarine(submarineDesc.type, submarineDesc.name, submarineDesc.points);
            scene->addItem(sub);
            int random = (qrand() % 15 + 1);
            qreal x = random == 13 || random == 5 ? 0 : scene->width() - sub->size().width();
            qreal y = scene->height() -(qrand() % 150 + 1) - sub->size().height();
            sub->setPos(x,y);
            sub->setCurrentDirection(x == 0 ? SubMarine::Right : SubMarine::Left);
            sub->setCurrentSpeed(qrand() % 3 + 1);
        }
    }
}

/** Pause State */
PauseState::PauseState(GraphicsScene *scene, QState *parent) : QState(parent),scene(scene)
{
}
void PauseState::onEntry()
{
    AnimationManager::self()->pauseAll();
    scene->boat->setEnabled(false);
}
void PauseState::onExit()
{
    AnimationManager::self()->resumeAll();
    scene->boat->setEnabled(true);
    scene->boat->setFocus();
}

/** Lost State */
LostState::LostState(GraphicsScene *scene, PlayState *game, QState *parent) : QState(parent), scene(scene), game(game)
{
}

void LostState::onEntry()
{
    //The message to display
    QString message = QString("You lose on level %1. Your score is %2.").arg(game->currentLevel+1).arg(game->score);

    //We set the level back to 0
    game->currentLevel = 0;

    //We set the score back to 0
    game->score = 0;

    //We clear the scene
    scene->clearScene();

    //we have only one view
    QMessageBox::information(scene->views().at(0),"You lose",message);
}

/** Win State */
WinState::WinState(GraphicsScene *scene, PlayState *game, QState *parent) : QState(parent), scene(scene), game(game)
{
}

void WinState::onEntry()
{
    //We clear the scene
    scene->clearScene();

    QString message;
    if (scene->levelsData.size() - 1 != game->currentLevel) {
        message = QString("You win the level %1. Your score is %2.").arg(game->currentLevel+1).arg(game->score);
        //We increment the level number
        game->currentLevel++;
    } else {
        message = QString("You finish the game on level %1. Your score is %2.").arg(game->currentLevel+1).arg(game->score);
        //We set the level back to 0
        game->currentLevel = 0;
        //We set the score back to 0
        game->score = 0;
    }

    //we have only one view
    QMessageBox::information(scene->views().at(0),"You win",message);
}

/** UpdateScore State */
UpdateScoreState::UpdateScoreState(PlayState *game, QState *parent) : QState(parent)
{
    this->game = game;
}
void UpdateScoreState::onEntry()
{
    //### Make a nice anim to update the score in the scene
    QState::onEntry();
}

/** Win transition */
UpdateScoreTransition::UpdateScoreTransition(GraphicsScene *scene, PlayState *game, QAbstractState *target)
    : QSignalTransition(scene,SIGNAL(subMarineDestroyed(int)), QList<QAbstractState*>() << target),
    game(game)
{
}

bool UpdateScoreTransition::eventTest(QEvent *event) const
{
    if (!QSignalTransition::eventTest(event))
        return false;
    else {
        QSignalEvent *se = static_cast<QSignalEvent*>(event);
        game->score += se->arguments().at(0).toInt();
        return true;
    }
}

/** Win transition */
WinTransition::WinTransition(GraphicsScene *scene, PlayState *game, QAbstractState *target)
    : QSignalTransition(scene,SIGNAL(allSubMarineDestroyed(int)), QList<QAbstractState*>() << target),
    game(game)
{
}

bool WinTransition::eventTest(QEvent *event) const
{
    if (!QSignalTransition::eventTest(event))
        return false;
    else {
        QSignalEvent *se = static_cast<QSignalEvent*>(event);
        game->score += se->arguments().at(0).toInt();
        return true;
    }
}

/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gameengine.h"
#include "plugins/levelplugininterface.h"
#include "InvSounds.h"

#include <QDebug>
#include <QTimerEvent>
#include <QTime>
#include <QDesktopServices>

const int TIMER_SPEED = 80;

GameEngine::GameEngine(QObject* parent)
    :QObject(parent)
{
    m_timerId = 0;
    m_doEnemyMissile = 1500 / TIMER_SPEED;
    m_GameGml = 0;
    m_gameLevel = 0;

    clearQmlObjects();

    // For random
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    // Sound engine
    m_soundEngine = new CInvSounds(this);

    // Device profile
    m_silent = false;

#ifdef Q_OS_SYMBIAN
    iVibrate = CHWRMVibra::NewL();
#endif

    // Get device profile, is it silent?
#if defined Q_OS_SYMBIAN || defined Q_WS_MAEMO_5
    m_systemDeviceInfo = new QSystemDeviceInfo(this);
    QObject::connect(m_systemDeviceInfo,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),this,
                     SLOT(currentProfileChanged(QSystemDeviceInfo::Profile)));
    QSystemDeviceInfo::Profile p = m_systemDeviceInfo->currentProfile();
    if (p == QSystemDeviceInfo::SilentProfile) {
        m_silent = true;
    }
#endif

}


GameEngine::~GameEngine()
{
#ifdef Q_OS_SYMBIAN
    delete iVibrate;
#endif

}

void GameEngine::gameStartSound()
{
    if (!m_silent)
        m_soundEngine->gameStartSound();
}

#if defined Q_OS_SYMBIAN || defined Q_WS_MAEMO_5
void GameEngine::currentProfileChanged(QSystemDeviceInfo::Profile p)
{
    if (p == QSystemDeviceInfo::SilentProfile) {
        enableSounds(QVariant(false));
    } else {
        enableSounds(QVariant(true));
    }
}
#endif

void GameEngine::enableSounds(QVariant enable)
{
    m_silent = !enable.toBool();

    if (m_silent)
        this->m_soundEngine->enableSounds(false);
    else
        this->m_soundEngine->enableSounds(true);

}

QVariant GameEngine::randInt(QVariant low, QVariant high)
{
    // Random number between low and high
    return qrand() % ((high.toInt() + 1) - low.toInt()) + low.toInt();
}

void GameEngine::setGameLevel(LevelPluginInterface* level)
{
    // Set used game level
    m_gameLevel = level;

    if (m_gameLevel) {
        // Set used sound from the level into sound engine
        m_soundEngine->enableSounds(m_gameLevel->levelSounds());
        // Invoke QML to take new level in use
        QMetaObject::invokeMethod(m_GameGml, "levelReadyForCreation", Qt::AutoConnection);

        m_doEnemyMissile = m_gameLevel->enemyFireSpeed().toInt() / TIMER_SPEED;
    }
}

void GameEngine::setPluginList(QList<QPluginLoader*> plugins)
{
    m_pluginList = plugins;
}

QVariant GameEngine::pluginList()
{
    QStringList list;
    QPluginLoader* loader;
    foreach (loader,m_pluginList) {
        QString s = loader->fileName();
        s = s.mid(s.lastIndexOf("/")+1);
        s = s.left(s.lastIndexOf("."));
        s = s.toUpper();
#ifdef Q_WS_MAEMO_5
        if (s.contains("LIB")) {
            s = s.right(s.length() - (s.indexOf("LIB")+3));
        }
#endif
        list.append(s);
    }
    return QVariant(list);
}

void GameEngine::pauseLevel(QVariant doPause)
{
    bool enableTimer = !doPause.toBool();
    enableEngineTimer(QVariant(enableTimer));
    QMetaObject::invokeMethod(m_levelQml, "pause", Qt::AutoConnection,Q_ARG(QVariant, doPause));
}


void GameEngine::findQmlObjects()
{
    if (m_GameGml) {
        qDebug() << "GameEngine::findQmlObjects()";

        // Find Missiles objects
        m_missileList.clear();
        m_enemyMissileList.clear();
        findMissiles(m_GameGml);

        // Set QMLs
        setLevelQml(m_GameGml->findChild<QObject*>("level"));
        setEnemiesGridQml(m_GameGml->findChild<QObject*>("enemiesGrid"));
        setMyShipQml(m_GameGml->findChild<QObject*>("myShip"));

        // Find Enemies objects
        m_enemyList.clear();
        qDebug() << "GameEngine::findQmlObjects() find enemies from: level QML";
        findEnemies(m_levelQml);


    } else {
        qDebug() << "GameEngine::findQmlObjects() rootObject NULL";
    }
}

void GameEngine::clearQmlObjects()
{
    m_missileList.clear();
    m_enemyMissileList.clear();
    m_enemyList.clear();
    m_levelQml = 0;
    m_enemiesGridGml = 0;
    m_myShipGml = 0;
    //m_GameGml = 0; // NOTE: Do not delete this
}


void GameEngine::findMissiles(QObject *rootObject)
{
    if (rootObject) {
        QObjectList list = rootObject->children();
        QObject* item;
        foreach (item,list) {
            if (item->children().count()>0) {
                findMissiles(item);
            } else {
                if (rootObject->objectName()=="missile") {
                    QDeclarativeItem* missile = static_cast<QDeclarativeItem*>(rootObject);
                    m_missileList.append(missile);
                } else if (rootObject->objectName()=="enemy_missile") {
                    QDeclarativeItem* enemyMissile = static_cast<QDeclarativeItem*>(rootObject);
                    m_enemyMissileList.append(enemyMissile);
                }
            }
        }
    } else {
        qDebug() << "GameEngine::findMissiles() rootObject NULL";
    }
}

void GameEngine::findEnemies(QObject *rootObject)
{
    if (rootObject) {
        QObjectList list = rootObject->children();
        QObject* item;
        foreach (item,list) {
            if (item->children().count()>0 && item->objectName()!="enemy") {
                //qDebug() << "Enemy children found from: " << item->objectName();
                findEnemies(item);
            } else {
                if (item->objectName()=="enemy") {
                    //qDebug() << "Enemy child founds: " << item->objectName();
                    QDeclarativeItem* enemy = static_cast<QDeclarativeItem*>(item);
                    m_enemyList.append(enemy);
                }
            }
        }
    } else {
        qDebug() << "GameEngine::findEnemies() rootObject NULL";
    }
}

void GameEngine::setEnemiesGridQml(QObject* o)
{
    m_enemiesGridGml = static_cast<QDeclarativeItem*>(o);
}

void GameEngine::setMyShipQml(QObject* o)
{
    m_myShipGml = static_cast<QDeclarativeItem*>(o);
}

void GameEngine::setGameQml(QObject* o)
{
    m_GameGml = static_cast<QDeclarativeItem*>(o);
}


void GameEngine::timerEvent(QTimerEvent *e)
{
    if (e->timerId()==m_timerId) {
        // Do hit test
        doHitTest();

        m_doEnemyMissile--;

        if (m_gameLevel && m_doEnemyMissile<0) {
            m_doEnemyMissile = m_gameLevel->enemyFireSpeed().toInt() / TIMER_SPEED;
            // Do emeny missile launch
            doEnemyMissile();
        }
    }
}

void GameEngine::enableEngineTimer(QVariant enable)
{
    if (m_gameLevel) {
        if (m_timerId==0 && enable.toBool()) {
            m_timerId = QObject::startTimer(TIMER_SPEED);
        }
        else if (m_timerId != 0 && !enable.toBool()) {
            QObject::killTimer(m_timerId);
            m_timerId = 0;
        }
    }
}

void GameEngine::selectVisibleEnemy(int& start, int& end)
{
    QDeclarativeItem* enemy = 0;
    for (int i=0 ; i<m_enemyList.count() ; i++) {
        enemy = m_enemyList[i];
        if (enemy->opacity()==1) {
            start = i;
            break;
        }
    }
    enemy = 0;
    for (int e=m_enemyList.count()-1 ; e>0 ; e--) {
        enemy = m_enemyList[e];
        if (enemy->opacity()==1) {
            end = e;
            break;
        }
    }
}

void GameEngine::doEnemyMissile()
{
    QMutexLocker locker(&m_enemyListMutex);

    QDeclarativeItem* missile = 0;
    QDeclarativeItem* enemy = 0;

    // Find free missile
    foreach (missile, m_enemyMissileList) {
        if (missile->opacity()==0){
            // Random select enemy who fire
            int start=0; int end=0;
            selectVisibleEnemy(start,end);
            int whoWillFire = randInt(QVariant(start),QVariant(end)).toInt();
            if (m_enemyList.count() < whoWillFire+1)
                break;

            enemy = m_enemyList.at(whoWillFire);
            if (enemy && enemy->opacity()==1) {
                QPointF enemyP =  enemy->pos();
                if (m_enemiesGridGml) {
                    enemyP += m_enemiesGridGml->pos();
                }
                //qDebug() << "QMetaObject::invokeMethod() - fireEnemyMissile";
                QMetaObject::invokeMethod(m_GameGml, "fireEnemyMissile", Qt::AutoConnection,
                                          Q_ARG(QVariant, enemyP.x()+enemy->boundingRect().width()/4),
                                          Q_ARG(QVariant, enemyP.y()+enemy->boundingRect().height()),
                                          Q_ARG(QVariant, m_GameGml->boundingRect().height()));
            }
            break;
        }
    }
}

void GameEngine::doHitTest()
{
    QMutexLocker locker(&m_enemyListMutex);

    QDeclarativeItem* missile = 0;
    QDeclarativeItem* enemy = 0;

    // No enemies?
    if (m_enemyList.count()==0) {
        enableEngineTimer(QVariant(false));
        qDebug() << "No enemies left";
        gameOver(true);
        return;
    }

    if (!m_myShipGml) {
        return;
    }

    // Check ship collision
    if (m_myShipGml->opacity()==1) {
        for (int e=0; e<m_enemyList.count(); e++) {
            enemy = m_enemyList[e];
            if (enemy->opacity()==0) {
                break;
            }
            QPointF enemyP =  enemy->pos();
            if (m_enemiesGridGml) {
                enemyP += m_enemiesGridGml->pos();
            }
            QRectF enemyR(enemyP,QSize(enemy->boundingRect().width(),enemy->boundingRect().height()));
            // Collision?
            if (enemyR.contains(m_myShipGml->pos())) {
                enableEngineTimer(QVariant(false));

                // Collision explosion
                QPointF myP =  m_myShipGml->pos();
                playSound(1);
                QMetaObject::invokeMethod(m_levelQml, "explode", Qt::AutoConnection,
                                          Q_ARG(QVariant, myP.x()+m_myShipGml->boundingRect().width()/2),
                                          Q_ARG(QVariant, myP.y()+m_myShipGml->boundingRect().height()));
                m_myShipGml->setOpacity(0);

                gameOver(false);
                qDebug() << "Collision";
                return;
            }
            // Enemy too deep?
            else if (enemyR.bottomLeft().y() > m_myShipGml->pos().y()+m_myShipGml->pos().y()*0.1) {
                enableEngineTimer(QVariant(false));

                // Enemy too deep explosion
                QPointF myP =  m_myShipGml->pos();
                playSound(1);
                QMetaObject::invokeMethod(m_levelQml, "explode", Qt::AutoConnection,
                                          Q_ARG(QVariant, myP.x()+m_myShipGml->boundingRect().width()/2),
                                          Q_ARG(QVariant, myP.y()+m_myShipGml->boundingRect().height()));
                m_myShipGml->setOpacity(0);

                gameOver(false);
                qDebug() << "Enemy too deep";
                return;
            }
        }
    }

    // Check your missiles hit to enemies
    foreach (missile, m_missileList) {
        if (missile->opacity()==1){
            for (int e=0; e<m_enemyList.count(); e++) {
                enemy = m_enemyList[e];
                if (enemy->opacity()<1) {
                    break;
                }
                QPointF missileP = missile->pos();
                missileP.setX(missileP.rx() + missile->boundingRect().width()/2);

                QPointF enemyP =  enemy->pos();
                if (m_enemiesGridGml) {
                    enemyP += m_enemiesGridGml->pos();
                }

                QRectF r(enemyP,QSize(enemy->boundingRect().width(),enemy->boundingRect().height()));
                if (r.contains(missileP)) {
                    // Hit !
                    playSound(0);
                    //qDebug() << "QMetaObject::invokeMethod() - explode";
                    QMetaObject::invokeMethod(m_levelQml, "explode", Qt::AutoConnection,
                                              Q_ARG(QVariant, enemyP.x()+enemy->boundingRect().width()/2),
                                              Q_ARG(QVariant, enemyP.y()+enemy->boundingRect().height()));
                    missile->setOpacity(0);
                    //fastVibra();
                    if (m_enemiesGridGml) {
                        // Set transparent placeholder for enemy when using GridView
                        enemy->setProperty("source",QVariant("file:/"+m_gameLevel->pathToTransparentEnemyPic().toString()));
                    } else {
                        // Hide enemy after explode
                        enemy->setOpacity(0);
                    }

                    // Remove enemy from list
                    m_enemyList.removeAt(e);
                    e--;
                }
                enemy = 0;
            }
        }
    }

    // Check enemies missiles hit to you
    if (m_myShipGml->opacity()==1) {
        foreach (missile, m_enemyMissileList) {
            if (missile->opacity()==1){
                QPointF missileP = missile->pos();
                missileP.setX(missileP.rx() + missile->boundingRect().width()/2);

                QPointF myP =  m_myShipGml->pos();

                QRectF r(myP,QSize(m_myShipGml->boundingRect().width(),m_myShipGml->boundingRect().height()));
                if (r.contains(missileP)) {
                    // Hit !
                    playSound(1);
                    //qDebug() << "QMetaObject::invokeMethod() - explode";
                    QMetaObject::invokeMethod(m_levelQml, "explode", Qt::AutoConnection,
                                              Q_ARG(QVariant, myP.x()+m_myShipGml->boundingRect().width()/2),
                                              Q_ARG(QVariant, myP.y()+m_myShipGml->boundingRect().height()));
                    missile->setOpacity(0);
                    m_myShipGml->setOpacity(0);
                    break;
                }
            }
        }
    } else {
        // You was killed
        enableEngineTimer(QVariant(false));
        gameOver(false);
        qDebug() << "You was killed by enemy missile";
    }

}


void GameEngine::playSound(QVariant index)
{
    if (!m_silent) {
        int i = index.toInt();
        m_soundEngine->playSound(i);
    }
}

void GameEngine::playSounds(QVariant index, QVariant count)
{
    if (!m_silent) {
        m_soundEngine->playSounds(index.toInt(),count.toInt());
    }
}

void GameEngine::playInternalSound(QVariant index)
{
    if (!m_silent) {
        m_soundEngine->playInternalSound(index.toInt());
    }
}

void GameEngine::playInternalSounds(QVariant index, QVariant count)
{
    if (!m_silent) {
        m_soundEngine->playInternalSounds(index.toInt(),count.toInt());
    }
}

void GameEngine::gameOver(bool youWin)
{
    qDebug() << "GameEngine::gameOver() "<< youWin;
    QMetaObject::invokeMethod(m_GameGml, "gameOver", Qt::AutoConnection,Q_ARG(QVariant, youWin));
}

void GameEngine::pauseGame() {
    QMetaObject::invokeMethod(m_GameGml, "pauseGame", Qt::AutoConnection);
}


QVariant GameEngine::isSymbian()
{
#ifdef Q_OS_SYMBIAN
    return QVariant(true);
#else
    return QVariant(false);
#endif
}

QVariant GameEngine::isMaemo()
{
#ifdef Q_WS_MAEMO_5
    return QVariant(true);
#else
    return QVariant(false);
#endif
}

QVariant GameEngine::isWindows()
{
#ifdef Q_OS_WIN
    return QVariant(true);
#else
    return QVariant(false);
#endif
}

void GameEngine::vibra()
{
#ifdef Q_OS_SYMBIAN
    if (iVibrate){
        TRAPD(err, iVibrate->StartVibraL(4000,KHWRMVibraMaxIntensity));
    }
#endif
}

void GameEngine::fastVibra()
{
#ifdef Q_OS_SYMBIAN
    if (iVibrate){
        TRAPD(err, iVibrate->StartVibraL(100,KHWRMVibraMaxIntensity));
    }
#endif
}

void GameEngine::openLink(QVariant link)
{
QDesktopServices::openUrl(QUrl(link.toString()));
}


/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QDeclarativeItem>
#include <QMutex>
#include <QPluginLoader>
#include <QList>

#ifdef Q_OS_SYMBIAN
#include <hwrmvibra.h>
#endif

// QtMobility
#if defined Q_OS_SYMBIAN || defined Q_WS_MAEMO_5
#include <QSystemDeviceInfo>
QTM_USE_NAMESPACE
#endif

class CInvSounds;
class LevelPluginInterface;
class GameEngine : public QObject
{
    Q_OBJECT

public:
    GameEngine(QObject* parent);
    ~GameEngine();
    void timerEvent(QTimerEvent *);
    void setGameLevel(LevelPluginInterface* level);
    void setPluginList(QList<QPluginLoader*> plugins);
    void pauseGame();

public:
    Q_INVOKABLE void enableEngineTimer(QVariant enable);
    Q_INVOKABLE QVariant randInt(QVariant low, QVariant high);
    Q_INVOKABLE void enableSounds(QVariant enable);
    Q_INVOKABLE void playSound(QVariant index);
    Q_INVOKABLE void playSounds(QVariant index, QVariant count);
    Q_INVOKABLE void playInternalSound(QVariant index);
    Q_INVOKABLE void playInternalSounds(QVariant index, QVariant count);
    Q_INVOKABLE QVariant pluginList();
    Q_INVOKABLE void findQmlObjects();
    Q_INVOKABLE void clearQmlObjects();
    Q_INVOKABLE void pauseLevel(QVariant doPause);
    Q_INVOKABLE void gameStartSound();

    Q_INVOKABLE QVariant isSymbian();
    Q_INVOKABLE QVariant isMaemo();
    Q_INVOKABLE QVariant isWindows();

    Q_INVOKABLE void vibra();
    Q_INVOKABLE void fastVibra();

    Q_INVOKABLE void openLink(QVariant link);


private slots:
#if defined Q_OS_SYMBIAN || defined Q_WS_MAEMO_5
    void currentProfileChanged (QSystemDeviceInfo::Profile);
#endif

public:
    void setGameQml(QObject* o);

private:
    void findMissiles(QObject* rootObject);
    void findEnemies(QObject* rootObject);
    void setLevelQml(QObject* o) {m_levelQml = o;}
    void setEnemiesGridQml(QObject* o);
    void setMyShipQml(QObject* o);
    void gameOver(bool youWin);
    void selectVisibleEnemy(int& start, int& end);

private:
    void doHitTest();
    void doEnemyMissile();

private:
    bool                        m_enableEnemyMissiles;
    QMutex                      m_enemyListMutex;
    QList<QDeclarativeItem*>    m_enemyList;
    QList<QDeclarativeItem*>    m_missileList;
    QList<QDeclarativeItem*>    m_enemyMissileList;
    QObject*                    m_levelQml;
    QDeclarativeItem*           m_enemiesGridGml;
    QDeclarativeItem*           m_myShipGml;
    QDeclarativeItem*           m_GameGml;

    LevelPluginInterface*       m_gameLevel;
    CInvSounds*                 m_soundEngine;
    QList<QPluginLoader*>       m_pluginList;

    int                         m_timerId;
    int                         m_doEnemyMissile;

#if defined Q_OS_SYMBIAN || defined Q_WS_MAEMO_5
    QSystemDeviceInfo*          m_systemDeviceInfo;
#endif
    bool                        m_silent;

#ifdef Q_OS_SYMBIAN
    CHWRMVibra* iVibrate;
#endif

};

#endif // GAMEENGINE_H

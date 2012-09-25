/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

/*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */

#ifndef __AST_VIEW_H__
#define __AST_VIEW_H__

#include <QWidget>
#include <QList>
#include <QMultiHash>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimerEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include "sprites.h"

#define MAX_POWER_LEVEL          1000

class KAsteroidsView : public QWidget
{
    Q_OBJECT
public:
    KAsteroidsView( QWidget *parent = 0);
    virtual ~KAsteroidsView();

    int refreshRate;

    void reset();
    void setRockSpeed( double rs ) { rockSpeed = rs; }
    void addRocks( int num );
    void newGame();
    void endGame();
    void newShip();

    void rotateLeft( bool r ) { rotateL = r; rotateSlow = 5; }
    void rotateRight( bool r ) { rotateR = r; rotateSlow = 5; }
    void thrust( bool t ) { thrustShip = t && shipPower > 0; }
    void shoot( bool s ) { shootShip = s; shootDelay = 0; }
    void setShield( bool s );
    void teleport( bool te) { teleportShip = te && mTeleportCount; }
    void brake( bool b );
    void pause( bool p);

    void showText( const QString &text, const QColor &color, bool scroll=TRUE );
    void hideText();

    int shots() const { return shotsFired; }
    int hits() const { return shotsHit; }
    int power() const { return shipPower; }

    int teleportCount() const { return mTeleportCount; }
    int brakeCount() const { return mBrakeCount; }
    int shieldCount() const { return mShieldCount; }
    int shootCount() const { return mShootCount; }

signals:
    void shipKilled();
    void rockHit( int size );
    void rocksRemoved();
    void updateVitals();

private slots:
    void hideShield();

protected:
    bool readSprites();
    void wrapSprite( QGraphicsItem * );
    void rockHit( AnimatedPixmapItem * );
    void reducePower( int val );
    void addExhaust( double x, double y, double dx, double dy, int count );
    void processMissiles();
    void processShip();
    void processPowerups();
    void processShield();
    double randDouble();
    int randInt( int range );

    virtual void resizeEvent( QResizeEvent *event );
    virtual void timerEvent( QTimerEvent * );

    void showEvent( QShowEvent * );

private:
    QGraphicsScene field;
    QGraphicsView view;
    QMap<int, QList<QPixmap> > animation;
    QList<AnimatedPixmapItem*> rocks;
    QList<KMissile*> missiles;
    QList<KBit*> bits;
    QList<KExhaust*> exhaust;
    QList<KPowerup*> powerups;
    KShield *shield;
    AnimatedPixmapItem *ship;
    QGraphicsTextItem *textSprite;

    bool rotateL;
    bool rotateR;
    bool thrustShip;
    bool shootShip;
    bool teleportShip;
    bool brakeShip;
    bool pauseShip;
    bool shieldOn;

    bool vitalsChanged;

    int  shipAngle;
    int  rotateSlow;
    int  rotateRate;
    int  shipPower;

    int shotsFired;
    int shotsHit;
    int shootDelay;

    int mBrakeCount;
    int mShieldCount;
    int mTeleportCount;
    int mShootCount;

    double shipDx;
    double shipDy;

    int  textDy;
    int  mFrameNum;
    bool mPaused;
    int  mTimerId;

    double rockSpeed;
    double powerupSpeed;

    bool can_destroy_powerups;

    QTimer *shieldTimer;
    bool initialized;
};

#endif

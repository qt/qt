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

#ifndef __GRAPHICSSCENE__H__
#define __GRAPHICSSCENE__H__

//Qt
#include <QGraphicsScene>
#include <QSet>

#if defined(QT_EXPERIMENTAL_SOLUTION)
# include "qstate.h"
#else
# include <QState>
#endif


class Boat;
class SubMarine;
class Torpedo;
class Bomb;
class PixmapItem;
class QAction;

class GraphicsScene : public QGraphicsScene
{
Q_OBJECT
public:
    enum Mode {
        Big = 0,
        Small
    };

    struct SubmarineDescription {
        int type;
        int points;
        QString name;
    };

    struct LevelDescription {
        int id;
        QString name;
        QList<QPair<int,int> > submarines;
    };

    GraphicsScene(int x, int y, int width, int height, Mode mode = Big);
    qreal sealLevel() const;
    void setupScene(const QList<QAction *> &actions);
    void addItem(Bomb *bomb);
    void addItem(Torpedo *torpedo);
    void addItem(SubMarine *submarine);
    void addItem(QGraphicsItem *item);
    int remainingSubMarines() const;
    void clearScene();
    QGraphicsPixmapItem *addWelcomeItem(const QPixmap &pm);

Q_SIGNALS:
    void subMarineDestroyed(int);
    void allSubMarineDestroyed(int);

protected:
    void mousePressEvent (QGraphicsSceneMouseEvent * event);

private slots:
    void onQuitGameTriggered();
    void onBombExecutionFinished();
    void onTorpedoExecutionFinished();
    void onSubMarineExecutionFinished();
    void onIntroAnimationFinished();

private:
    Mode mode;
    PixmapItem *backgroundItem;
    QAction * newAction;
    QAction * quitAction;
    Boat *boat;
    QSet<SubMarine *> submarines;
    QSet<Bomb *> bombs;
    QSet<Torpedo *> torpedos;
    QVector<QGraphicsPixmapItem *> welcomeItems;
    QVector<SubmarineDescription> submarinesData;
    QHash<int, LevelDescription> levelsData;

    friend class PauseState;
    friend class PlayState;
    friend class LostState;
    friend class WinState;
};

#endif //__GRAPHICSSCENE__H__


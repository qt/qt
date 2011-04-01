/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SPRITEIMAGE_H
#define SPRITEIMAGE_H

#include <QSGItem>
#include <QTime>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGContext;
class SpriteState;
class SpriteEngine;
class QSGGeometryNode;
class SpriteMaterial;
class SpriteImage : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    //###try to share similar spriteEngines for less overhead?
    Q_PROPERTY(QDeclarativeListProperty<SpriteState> sprites READ sprites)
    Q_CLASSINFO("DefaultProperty", "sprites")

public:
    explicit SpriteImage(QSGItem *parent = 0);

    QDeclarativeListProperty<SpriteState> sprites();

    bool running() const
    {
        return m_running;
    }

signals:


    void runningChanged(bool arg);

public slots:

void setRunning(bool arg)
{
    if (m_running != arg) {
        m_running = arg;
        emit runningChanged(arg);
    }
}

private slots:
    void createEngine();
protected:
    void reset();
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
private:
    void prepareNextFrame();
    QSGGeometryNode* buildNode();
    QSGGeometryNode *m_node;
    SpriteMaterial *m_material;
    QList<SpriteState*> m_sprites;
    SpriteEngine* m_spriteEngine;
    QTime m_timestamp;
    int m_maxFrames;
    bool m_pleaseReset;
    bool m_running;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // SPRITEIMAGE_H

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

#ifndef LEVELONE_H
#define LEVELONE_H

#include <QObject>
#include <QDir>
#include <QImageReader>

#include "levelplugininterface.h"

class LevelOne : public LevelPluginInterface
{
    Q_OBJECT

    // This macro tells Qt which interfaces the class implements.
    // This is used when implementing plugins.
    Q_INTERFACES(LevelPluginInterface)

public:
    LevelOne();
    ~LevelOne();

    static QString pathPrefix();

public: // From LevelPluginInterface

    QStringList levelSounds();

    Q_INVOKABLE QVariant getData(QVariant key);

    Q_INVOKABLE QVariant graphSize(QVariant pathToGraph);

    Q_INVOKABLE QVariant enemyCount() { return QVariant(30); } // 6x5=30
    Q_INVOKABLE QVariant enemyRowCount() { return QVariant(6); } // 6 rows of enemies
    Q_INVOKABLE QVariant enemySpeed() { return QVariant(36000); }
    Q_INVOKABLE QVariant enemyFireSpeed() { return QVariant(200); }

    // Root paths for the QML, Pictures and Sounds
    Q_INVOKABLE QVariant qmlRootPath() { return QVariant(LevelOne::pathPrefix()+"quickhitdata/levelone/"); }
    Q_INVOKABLE QVariant pictureRootPath() { return QVariant(LevelOne::pathPrefix()+"quickhitdata/levelone/gfx/"); }

    // Full paths
    Q_INVOKABLE QVariant pathToTransparentEnemyPic() {return QVariant(LevelOne::pathPrefix()+"quickhitdata/levelone/gfx/transparent.png");}
    Q_INVOKABLE QVariant pathToMissilePic() {return QVariant(LevelOne::pathPrefix()+"quickhitdata/levelone/gfx/missile2.png");}
    Q_INVOKABLE QVariant pathToEnemyMissilePic() {return QVariant(LevelOne::pathPrefix()+"quickhitdata/levelone/gfx/enemy_missile2.png");}
    Q_INVOKABLE QVariant pathToMyShipPic() {return QVariant(LevelOne::pathPrefix()+"quickhitdata/levelone/gfx/ship.png");}

protected:
    QImageReader    m_imageReader;
};

#endif // LEVELONE_H

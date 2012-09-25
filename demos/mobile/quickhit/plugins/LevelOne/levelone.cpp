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

#include "levelone.h"
#include <QtCore/qplugin.h>


LevelOne::LevelOne()
{
}

LevelOne::~LevelOne()
{
}

QString LevelOne::pathPrefix()
{
    /*
QDir::rootPath()
For Unix operating systems this returns "/".
For Windows file systems this normally returns "c:/".
On Symbian this typically returns "c:/data",
*/

#ifdef Q_WS_MAEMO_5
    return "/home/user/.";
#elif defined Q_OS_WIN32
    return QDir::rootPath();
#else
    //return QDir::rootPath()+"/";
    return "c:/System/";
#endif
}

QStringList LevelOne::levelSounds()
{
    QStringList list;

    // NOTE: Mandatory sounds/indexs for the all levels
    // 0 = Enemy explode
    list.append(LevelOne::pathPrefix()+"quickhitdata/levelone/sound/rocket_explosion.wav");
    // 1 = You explode
    list.append(LevelOne::pathPrefix()+"quickhitdata/levelone/sound/crash.wav");
    // 2 = Missile fires
    list.append(LevelOne::pathPrefix()+"quickhitdata/levelone/sound/rocket_explosion.wav");
    // 3 = Emeny Missile fires
    list.append(LevelOne::pathPrefix()+"quickhitdata/levelone/sound/rocket.wav");

    // Additional sounds
    // Level starts
    list.append(LevelOne::pathPrefix()+"quickhitdata/levelone/sound/levelonestart.wav");

    return list;
}

QVariant LevelOne::getData(QVariant key)
{
    QVariant ret;
    switch (key.toInt()) {
    case 1: {
            // Any data what you need for this key
            // Set your data into QVariant (ret)
            break;
        }
    default: {
            break;
        }
    }
    return ret;
}

QVariant LevelOne::graphSize(QVariant pathToGraph)
{
    QVariant ret;
    m_imageReader.setFileName(pathToGraph.toString());
    QSize imageSize = m_imageReader.size();
    ret.setValue(imageSize);
    return ret;
}

// This Qt macro exports the plugin class LevelOne with the name levelplugins.
// There should be exactly one occurrence of this LevelOne macro in a Qt plugin's source code.
Q_EXPORT_PLUGIN2(levelplugins, LevelOne);

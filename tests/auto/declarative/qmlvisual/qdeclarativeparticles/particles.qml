/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

import Qt 4.7
import Qt.labs.particles 1.0

Rectangle {
    width: 640; height: 480; color: "black"

    Particles { id:particlesAnotEmitting
        y:60; width: 260; height:30; source: "star.png";
        lifeSpan:1000; count: 50; angle:70; angleDeviation:36;
        velocity:30; velocityDeviation:10; emissionRate: 0
        ParticleMotionWander { yvariance:5; xvariance:30; pace:100 }
    }
    Particles { id:particlesA
        y:0; width: 260; height:30; source: "star.png";
        lifeSpan:1000; count: 50; angle:70; angleDeviation:36;
        velocity:30; velocityDeviation:10; emissionRate: 10
        ParticleMotionWander { yvariance:5; xvariance:30; pace:100 }
    }

    Particles { id:particlesB
        y:280; x:180; width:1; height:1; lifeSpan:1000; source: "star.png"
        count: 100; angle:270; angleDeviation:45; velocity:50; velocityDeviation:30;
        emissionRate: 0
        ParticleMotionGravity { yattractor: 1000; xattractor:0; acceleration:25 }
    }

        Timer { running: true; interval: 1000; repeat: true; onTriggered: particlesB.burst(200, 2000); }

    Column{
        x: 340;
        Repeater{
            model: 5
            delegate: Component{
                Item{
                    width: 100; height: 100
                    Rectangle{
                        color: "blue"
                        width: 2; height: 2;
                        x: 49; y:49;
                    }
                    Particles{
                        x: 50; y:50; width: 0; height: 0;
                        fadeInDuration: 0; fadeOutDuration: 0
                        lifeSpan: 1000; lifeSpanDeviation:0;
                        source: "star.png"
                        count: -1; emissionRate: 120;
                        emissionVariance: index/2;
                        velocity: 250; velocityDeviation: 0;
                        angle: 0; angleDeviation: 0;
                    }
                }
            }
        }
    }
}

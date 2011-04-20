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

#include "V1/qdeclarativeparticles_p.h"
#include "pluginmain.h"
#include "spritestate.h"
#include "spriteengine.h"
#include "particleaffector.h"
#include "wanderaffector.h"
//#include "rockingaffector.h"
//#include "scalingaffector.h"
#include "resetaffector.h"
#include "gravityaffector.h"
#include "driftaffector.h"
#include "gravitationalsingularityaffector.h"
#include "frictionaffector.h"
#include "meanderaffector.h"
#include "attractoraffector.h"
#include "speedlimitaffector.h"
#include "killaffector.h"
//#include "zoneaffector.h"
//#include "toggleaffector.h"
#include "spritegoalaffector.h"
#include "swarmaffector.h"
#include "turbulenceaffector.h"
#include "eternalaffector.h"
#include "particlesystem.h"
#include "particleemitter.h"
//#include "spriteemitter.h"
#include "trailsemitter.h"
#include "particle.h"
#include "coloredparticle.h"
#include "spriteparticle.h"
#include "modelparticle.h"
//#include "pairedparticle.h"
#include "spriteimage.h"
#include "followemitter.h"
#include "particleextruder.h"
#include "ellipseextruder.h"
#include "lineextruder.h"
#include "maskextruder.h"
#include "varyingvector.h"
#include "pointvector.h"
#include "angledvector.h"
#include "directedvector.h"
//#include "followaffector.h"
#include "deformableparticle.h"
#include "pictureaffector.h"

QT_BEGIN_NAMESPACE

void ParticlesPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("Qt.labs.particles"));

    qmlRegisterType<QDeclarativeParticles>(uri, 1, 0, "Particles");
    qmlRegisterType<QDeclarativeParticleMotion>(uri,1,0,"ParticleMotion");
    qmlRegisterType<QDeclarativeParticleMotionGravity>(uri,1,0,"ParticleMotionGravity");
    qmlRegisterType<QDeclarativeParticleMotionLinear>(uri,1,0,"ParticleMotionLinear");
    qmlRegisterType<QDeclarativeParticleMotionWander>(uri,1,0,"ParticleMotionWander");
    qmlRegisterType<SpriteState>(uri, 2, 0, "Sprite");
    qmlRegisterType<SpriteEngine>(uri, 2, 0, "SpriteEngine");
    qmlRegisterType<SpriteImage>(uri, 2, 0, "SpriteImage");

    qmlRegisterType<ParticleSystem>(uri, 2, 0, "ParticleSystem");

    qmlRegisterType<ParticleType>(uri, 2, 0, "Particle");
    qmlRegisterType<ColoredParticle>(uri, 2, 0, "ColoredParticle");
    qmlRegisterType<SpriteParticle>(uri, 2, 0, "SpriteParticle");
    qmlRegisterType<ModelParticle>(uri, 2, 0, "ModelParticle");
    //qmlRegisterType<PairedParticle>(uri, 2, 0, "PairedParticle");
    qmlRegisterType<DeformableParticle>(uri, 2, 0, "DeformableParticle");

    qmlRegisterType<ParticleEmitter>(uri, 2, 0, "ParticleEmitter");
    qmlRegisterType<TrailsEmitter>(uri, 2, 0, "TrailEmitter");

    qmlRegisterType<FollowEmitter>(uri, 2, 0, "FollowEmitter");
    qmlRegisterType<ParticleExtruder>(uri, 2, 0, "Box");
    qmlRegisterType<EllipseExtruder>(uri, 2, 0, "Ellipse");
    qmlRegisterType<LineExtruder>(uri, 2, 0, "Line");
    qmlRegisterType<MaskExtruder>(uri, 2, 0, "Mask");

    qmlRegisterType<VaryingVector>(uri, 2, 0, "NullVector");
    qmlRegisterType<PointVector>(uri, 2, 0, "PointVector");
    qmlRegisterType<AngledVector>(uri, 2, 0, "AngleVector");
    qmlRegisterType<DirectedVector>(uri, 2, 0, "DirectedVector");

    qmlRegisterType<ParticleAffector>(uri, 2, 0, "ParticleAffector");
    qmlRegisterType<WanderAffector>(uri, 2, 0, "Wander");
    //qmlRegisterType<ScalingAffector>(uri, 2, 0, "Scale");
    //qmlRegisterType<RockingAffector>(uri, 2, 0, "Rocking");
    qmlRegisterType<DriftAffector>(uri, 2, 0, "Drift");
    qmlRegisterType<FrictionAffector>(uri, 2, 0, "Friction");
    qmlRegisterType<GravitationalSingularityAffector>(uri, 2, 0, "GravitationalSingularity");
    qmlRegisterType<AttractorAffector>(uri, 2, 0, "Attractor");
    qmlRegisterType<MeanderAffector>(uri, 2, 0, "Meander");
    qmlRegisterType<SpeedLimitAffector>(uri, 2, 0, "SpeedLimit");
    qmlRegisterType<GravityAffector>(uri, 2, 0, "Gravity");
    qmlRegisterType<EternalAffector>(uri, 2, 0, "Stasis");
    qmlRegisterType<ResetAffector>(uri, 2, 0, "Reset");
    //qmlRegisterType<ZoneAffector>(uri, 2, 0, "Zone");
    //qmlRegisterType<ToggleAffector>(uri, 2, 0, "Toggle");
    qmlRegisterType<KillAffector>(uri, 2, 0, "Kill");
    qmlRegisterType<SpriteGoalAffector>(uri, 2, 0, "SpriteGoal");
    qmlRegisterType<SwarmAffector>(uri, 2, 0 , "Swarm");
    qmlRegisterType<TurbulenceAffector>(uri, 2, 0 , "Turbulence");
    qmlRegisterType<PictureAffector>(uri, 2, 0, "Picture");
}

QT_END_NAMESPACE

Q_EXPORT_PLUGIN2(Particles, QT_PREPEND_NAMESPACE(ParticlesPlugin))

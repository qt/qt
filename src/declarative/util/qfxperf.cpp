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

#include "qfxperf_p_p.h"

QT_BEGIN_NAMESPACE

Q_DEFINE_PERFORMANCE_LOG(QDeclarativePerf, "QDeclarativeGraphics") {
    Q_DEFINE_PERFORMANCE_METRIC(QDeclarativeParsing, "Compilation: QML Parsing")
    Q_DEFINE_PERFORMANCE_METRIC(Compilation, "             QML Compilation")
    Q_DEFINE_PERFORMANCE_METRIC(VMEExecution, "Execution:   QML VME Execution")
    Q_DEFINE_PERFORMANCE_METRIC(BindInit, "BindValue Initialization")
    Q_DEFINE_PERFORMANCE_METRIC(BindValue, "BindValue execution")
    Q_DEFINE_PERFORMANCE_METRIC(BindValueSSE, "BindValue execution SSE")
    Q_DEFINE_PERFORMANCE_METRIC(BindValueQt, "BindValue execution QtScript")
    Q_DEFINE_PERFORMANCE_METRIC(BindableValueUpdate, "QDeclarativeBinding::update")
    Q_DEFINE_PERFORMANCE_METRIC(PixmapLoad, "Pixmap loading")
    Q_DEFINE_PERFORMANCE_METRIC(FontDatabase, "Font database creation")
    Q_DEFINE_PERFORMANCE_METRIC(QDeclarativePathViewPathCache, "FX Items:    QDeclarativePathView: Path cache")
    Q_DEFINE_PERFORMANCE_METRIC(CreateParticle, "             QDeclarativeParticles: Particle creation")
    Q_DEFINE_PERFORMANCE_METRIC(ItemComponentComplete, "             QDeclarativeItem::componentComplete")
    Q_DEFINE_PERFORMANCE_METRIC(ImageComponentComplete, "             QDeclarativeImage::componentComplete")
    Q_DEFINE_PERFORMANCE_METRIC(BaseLayoutComponentComplete, "             QDeclarativeBasePositioner::componentComplete")
    Q_DEFINE_PERFORMANCE_METRIC(TextComponentComplete, "             QDeclarativeText::componentComplete")
    Q_DEFINE_PERFORMANCE_METRIC(QDeclarativeText_setText, "             QDeclarativeText::setText")
    Q_DEFINE_PERFORMANCE_METRIC(AddScript, "QDeclarativeScript::addScriptToEngine")
}

QT_END_NAMESPACE

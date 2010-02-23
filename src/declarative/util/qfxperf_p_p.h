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
#ifndef QFXPERF_H
#define QFXPERF_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qperformancelog_p_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

Q_DECLARE_PERFORMANCE_LOG(QmlPerf) {
    Q_DECLARE_PERFORMANCE_METRIC(QmlParsing)

    Q_DECLARE_PERFORMANCE_METRIC(Compilation)
    Q_DECLARE_PERFORMANCE_METRIC(VMEExecution)

    Q_DECLARE_PERFORMANCE_METRIC(BindInit)
    Q_DECLARE_PERFORMANCE_METRIC(BindValue)
    Q_DECLARE_PERFORMANCE_METRIC(BindValueSSE)
    Q_DECLARE_PERFORMANCE_METRIC(BindValueQt)
    Q_DECLARE_PERFORMANCE_METRIC(BindableValueUpdate)
    Q_DECLARE_PERFORMANCE_METRIC(PixmapLoad)
    Q_DECLARE_PERFORMANCE_METRIC(FontDatabase)
    Q_DECLARE_PERFORMANCE_METRIC(QmlGraphicsPathViewPathCache)
    Q_DECLARE_PERFORMANCE_METRIC(CreateParticle)
    Q_DECLARE_PERFORMANCE_METRIC(ItemComponentComplete)
    Q_DECLARE_PERFORMANCE_METRIC(ImageComponentComplete)
    Q_DECLARE_PERFORMANCE_METRIC(BaseLayoutComponentComplete)
    Q_DECLARE_PERFORMANCE_METRIC(TextComponentComplete)
    Q_DECLARE_PERFORMANCE_METRIC(QmlGraphicsText_setText)
    Q_DECLARE_PERFORMANCE_METRIC(AddScript)
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFXPERF_H

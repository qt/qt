/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLCOMPOSITETYPEMANAGER_P_H
#define QMLCOMPOSITETYPEMANAGER_P_H

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

#include "qmlscriptparser_p.h"
#include "qmlrefcount_p.h"
#include "qmlerror.h"
#include "qmlengine.h"

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QmlCompiledData;
class QmlComponentPrivate;
class QmlComponent;
class QmlDomDocument;

class QmlCompositeTypeData;
class QmlCompositeTypeResource;

class QmlCompositeTypeManager : public QObject
{
    Q_OBJECT
public:
    QmlCompositeTypeManager(QmlEngine *);
    ~QmlCompositeTypeManager();

    // Return a QmlCompositeTypeData for url.  The QmlCompositeTypeData may be 
    // cached.
    QmlCompositeTypeData *get(const QUrl &url);
    // Return a QmlCompositeTypeData for data, with the provided base url.  The
   //  QmlCompositeTypeData will not be cached.
    QmlCompositeTypeData *getImmediate(const QByteArray &data, const QUrl &url);

    // Clear cached types.  Only types that aren't in the Waiting state will
    // be cleared.
    void clearCache();

private Q_SLOTS:
    void replyFinished();
    void resourceReplyFinished();
    void requestProgress(qint64 received, qint64 total);

private:
    void loadSource(QmlCompositeTypeData *);
    void loadResource(QmlCompositeTypeResource *);
    void compile(QmlCompositeTypeData *);
    void setData(QmlCompositeTypeData *, const QByteArray &, const QUrl &);

    void doComplete(QmlCompositeTypeData *);
    void doComplete(QmlCompositeTypeResource *);
    void checkComplete(QmlCompositeTypeData *);
    int resolveTypes(QmlCompositeTypeData *);

    QmlEngine *engine;
    typedef QHash<QUrl, QmlCompositeTypeData *> Components;
    Components components;
    typedef QHash<QUrl, QmlCompositeTypeResource *> Resources;
    Resources resources;
};

QT_END_NAMESPACE

#endif // QMLCOMPOSITETYPEMANAGER_P_H


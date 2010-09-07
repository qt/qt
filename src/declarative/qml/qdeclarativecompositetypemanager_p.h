/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECOMPOSITETYPEMANAGER_P_H
#define QDECLARATIVECOMPOSITETYPEMANAGER_P_H

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

#include "private/qdeclarativescriptparser_p.h"
#include "private/qdeclarativerefcount_p.h"
#include "qdeclarativeerror.h"
#include "qdeclarativeengine.h"

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QDeclarativeCompiledData;
class QDeclarativeComponentPrivate;
class QDeclarativeComponent;
class QDeclarativeDomDocument;

class QDeclarativeCompositeTypeData;
class QDeclarativeCompositeTypeResource;

class QDeclarativeCompositeTypeManager : public QObject
{
    Q_OBJECT
public:
    QDeclarativeCompositeTypeManager(QDeclarativeEngine *);
    ~QDeclarativeCompositeTypeManager();

    // Return a QDeclarativeCompositeTypeData for url.  The QDeclarativeCompositeTypeData may be 
    // cached.
    QDeclarativeCompositeTypeData *get(const QUrl &url);
    // Return a QDeclarativeCompositeTypeData for data, with the provided base url.  The
   //  QDeclarativeCompositeTypeData will not be cached.
    QDeclarativeCompositeTypeData *getImmediate(const QByteArray &data, const QUrl &url);

    // Clear cached types.  Only types that aren't in the Waiting state will
    // be cleared.
    void clearCache();

private Q_SLOTS:
    void replyFinished();
    void resourceReplyFinished();
    void requestProgress(qint64 received, qint64 total);

private:
    void loadSource(QDeclarativeCompositeTypeData *);
    void loadResource(QDeclarativeCompositeTypeResource *);
    void compile(QDeclarativeCompositeTypeData *);
    void setData(QDeclarativeCompositeTypeData *, const QByteArray &, const QUrl &);

    void doComplete(QDeclarativeCompositeTypeData *);
    void doComplete(QDeclarativeCompositeTypeResource *);
    void checkComplete(QDeclarativeCompositeTypeData *);
    int resolveTypes(QDeclarativeCompositeTypeData *);

    QDeclarativeEngine *engine;
    typedef QHash<QUrl, QDeclarativeCompositeTypeData *> Components;
    Components components;
    typedef QHash<QUrl, QDeclarativeCompositeTypeResource *> Resources;
    Resources resources;
    typedef QHash<QUrl, QUrl> Redirects;
    Redirects redirects;
    int redirectCount;
};

QT_END_NAMESPACE

#endif // QDECLARATIVECOMPOSITETYPEMANAGER_P_H


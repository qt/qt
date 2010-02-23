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

#include "qmlcompiler_p.h"
#include "qmlengine.h"
#include "qmlcomponent.h"
#include "qmlcomponent_p.h"
#include "qmlcontext.h"
#include "qmlcontext_p.h"

#include <QtCore/qdebug.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

int QmlCompiledData::pack(const char *data, size_t size)
{
    const char *p = packData.constData();
    unsigned int ps = packData.size();

    for (unsigned int ii = 0; (ii + size) <= ps; ii += sizeof(int)) {
        if (0 == ::memcmp(p + ii, data, size))
            return ii;
    }

    int rv = packData.size();
    packData.append(data, size);
    return rv;
}

int QmlCompiledData::indexForString(const QString &data)
{
    int idx = primitives.indexOf(data);
    if (idx == -1) {
        idx = primitives.count();
        primitives << data;
    }
    return idx;
}

int QmlCompiledData::indexForByteArray(const QByteArray &data)
{
    int idx = datas.indexOf(data);
    if (idx == -1) {
        idx = datas.count();
        datas << data;
    }
    return idx;
}

int QmlCompiledData::indexForUrl(const QUrl &data)
{
    int idx = urls.indexOf(data);
    if (idx == -1) {
        idx = urls.count();
        urls << data;
    }
    return idx;
}

int QmlCompiledData::indexForFloat(float *data, int count)
{
    Q_ASSERT(count > 0);

    for (int ii = 0; ii <= floatData.count() - count; ++ii) {
        bool found = true;
        for (int jj = 0; jj < count; ++jj) {
            if (floatData.at(ii + jj) != data[jj]) {
                found = false;
                break;
            }
        }

        if (found)
            return ii;
    }

    int idx = floatData.count();
    for (int ii = 0; ii < count; ++ii)
        floatData << data[ii];

    return idx;
}

int QmlCompiledData::indexForInt(int *data, int count)
{
    Q_ASSERT(count > 0);

    for (int ii = 0; ii <= intData.count() - count; ++ii) {
        bool found = true;
        for (int jj = 0; jj < count; ++jj) {
            if (intData.at(ii + jj) != data[jj]) {
                found = false;
                break;
            }
        }

        if (found)
            return ii;
    }

    int idx = intData.count();
    for (int ii = 0; ii < count; ++ii)
        intData << data[ii];

    return idx;
}

int QmlCompiledData::indexForLocation(const QmlParser::Location &l)
{
    // ### FIXME
    int rv = locations.count();
    locations << l;
    return rv;
}

int QmlCompiledData::indexForLocation(const QmlParser::LocationSpan &l)
{
    // ### FIXME
    int rv = locations.count();
    locations << l.start << l.end;
    return rv;
}

QmlCompiledData::QmlCompiledData(QmlEngine *engine)
: QmlCleanup(engine), importCache(0), root(0), rootPropertyCache(0)
{
}

QmlCompiledData::~QmlCompiledData()
{
    for (int ii = 0; ii < types.count(); ++ii) {
        if (types.at(ii).ref)
            types.at(ii).ref->release();
    }

    for (int ii = 0; ii < propertyCaches.count(); ++ii) 
        propertyCaches.at(ii)->release();

    for (int ii = 0; ii < contextCaches.count(); ++ii)
        contextCaches.at(ii)->release();

    if (importCache)
        importCache->release();

    if (rootPropertyCache)
        rootPropertyCache->release();

    qDeleteAll(cachedPrograms);
    qDeleteAll(cachedClosures);
}

void QmlCompiledData::clear()
{
    qDeleteAll(cachedPrograms);
    qDeleteAll(cachedClosures);
    for (int ii = 0; ii < cachedClosures.count(); ++ii)
        cachedClosures[ii] = 0;
    for (int ii = 0; ii < cachedPrograms.count(); ++ii)
        cachedPrograms[ii] = 0;
}


QObject *QmlCompiledData::TypeReference::createInstance(QmlContext *ctxt, const QBitField &bindings) const
{
    if (type) {
        QObject *rv = type->create();
        if (rv)
            QmlEngine::setContextForObject(rv, ctxt);
        return rv;
    } else {
        Q_ASSERT(component);
        return QmlComponentPrivate::get(component)->create(ctxt, bindings);
    } 
}

const QMetaObject *QmlCompiledData::TypeReference::metaObject() const
{
    if (type) {
        return type->metaObject();
    } else {
        Q_ASSERT(component);
        return static_cast<QmlComponentPrivate *>(QObjectPrivate::get(component))->cc->root;
    }
}

void QmlCompiledData::dumpInstructions()
{
    if (!name.isEmpty())
        qWarning() << name;
    qWarning().nospace() << "Index\tLine\tOperation\t\tData1\tData2\tData3\tComments";
    qWarning().nospace() << "-------------------------------------------------------------------------------";
    for (int ii = 0; ii < bytecode.count(); ++ii) {
        dump(&bytecode[ii], ii);
    }
    qWarning().nospace() << "-------------------------------------------------------------------------------";
}


QT_END_NAMESPACE

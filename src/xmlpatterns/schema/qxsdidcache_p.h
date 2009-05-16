/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_XsdIdCache_H
#define Patternist_XsdIdCache_H

#include "qschemacomponent_p.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QReadWriteLock>
#include <QtCore/QSet>
#include <QtCore/QString>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Helper class for keeping track of all existing IDs in a schema.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdIdCache : public QSharedData
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdIdCache> Ptr;

            /**
             * Adds an @p id to the id cache.
             */
            void addId(const QString &id);

            /**
             * Returns whether the id cache contains the given @p id already.
             */
            bool hasId(const QString &id) const;

        private:
            QSet<QString>          m_ids;
            mutable QReadWriteLock m_lock;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

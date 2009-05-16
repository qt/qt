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

#ifndef Patternist_XsdSchemaMerger_H
#define Patternist_XsdSchemaMerger_H

#include "qxsdschema_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A helper class that merges two schemas into one.
     *
     * This class is used in XsdValidatingInstanceReader to merge the schema
     * given in the constructor with a schema defined in the instance document
     * via xsi:schemaLocation or xsi:noNamespaceSchema location.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdSchemaMerger : public QSharedData
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdSchemaMerger> Ptr;

            /**
             * Creates a new schema merger object that merges @p schema with @p otherSchema.
             */
            XsdSchemaMerger(const XsdSchema::Ptr &schema, const XsdSchema::Ptr &otherSchema);

            /**
             * Returns the merged schema.
             */
            XsdSchema::Ptr mergedSchema() const;

        private:
            void merge(const XsdSchema::Ptr &schema, const XsdSchema::Ptr &otherSchema);

            XsdSchema::Ptr m_mergedSchema;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

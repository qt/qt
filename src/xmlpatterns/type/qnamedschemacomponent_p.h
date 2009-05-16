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

#ifndef Patternist_NamedSchemaComponent_H
#define Patternist_NamedSchemaComponent_H

#include "qnamepool_p.h"
#include "qschemacomponent_p.h"
#include "qxmlname.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Base class for all named components that can appear in a W3C XML Schema.
     *
     * @ingroup Patternist_types
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class NamedSchemaComponent : public SchemaComponent
    {
        public:
            typedef QExplicitlySharedDataPointer<NamedSchemaComponent> Ptr;

            /**
             * Describes the blocking constraints that are given by the 'block' attributes.
             */
            enum BlockingConstraint
            {
                RestrictionConstraint = 1,
                ExtensionConstraint = 2,
                SubstitutionConstraint = 4
            };
            Q_DECLARE_FLAGS(BlockingConstraints, BlockingConstraint)

            /**
             * Creates a new named schema component.
             */
            NamedSchemaComponent();

            /**
             * Destroys the named schema component.
             */
            virtual ~NamedSchemaComponent();

            /**
             * Sets the @p name of the schema component.
             */
            void setName(const QXmlName &name);

            /**
             * Returns the name of the schema component.
             *
             * @param namePool The name pool the name belongs to.
             */
            virtual QXmlName name(const NamePool::Ptr &namePool) const;

            /**
             * Returns the display name of the schema component.
             *
             * @param namePool The name pool the name belongs to.
             */
            virtual QString displayName(const NamePool::Ptr &namePool) const;

        private:
            QXmlName m_name;
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(NamedSchemaComponent::BlockingConstraints)
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

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

#ifndef Patternist_XsdAttributeTerm_H
#define Patternist_XsdAttributeTerm_H

#include "qnamedschemacomponent_p.h"
#include "qxsdannotated_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A base class for all attribute types.
     *
     * For easy resolving of attribute references, we use this as
     * common base class for XsdAttribute and XsdAttributeReference.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdAttributeTerm : public NamedSchemaComponent, public XsdAnnotated
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdAttributeTerm> Ptr;

            /**
             * Returns @c true if the term is an attribute use, @c false otherwise.
             */
            virtual bool isAttributeUse() const;

            /**
             * Returns @c true if the term is an attribute use reference, @c false otherwise.
             *
             * @note The reference term is only used internally as helper during type resolving.
             */
            virtual bool isReference() const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

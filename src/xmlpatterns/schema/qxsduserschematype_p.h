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

#ifndef Patternist_XsdUserSchemaType_H
#define Patternist_XsdUserSchemaType_H

#include "qnamedschemacomponent_p.h"
#include "qschematype_p.h"
#include "qxsdannotated_p.h"

template<typename N, typename M> class QHash;
template<typename N> class QList;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A base class for all user defined simple and complex types.
     *
     * This class was introduced to combine the SchemaType class and the
     * NamedSchemaComponent class without explicit inheritance.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    template<typename TSuperClass>
    class XsdUserSchemaType : public TSuperClass, public NamedSchemaComponent, public XsdAnnotated
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdUserSchemaType> Ptr;

            /**
             * Sets the @p name of the type.
             */
            void setName(const QXmlName &name);

            /**
             * Returns the name of the type.
             *
             * @param namePool The pool the name belongs to.
             */
            virtual QXmlName name(const NamePool::Ptr &namePool) const;

            /**
             * Returns the display name of the type.
             *
             * @param namePool The pool the name belongs to.
             */
            virtual QString displayName(const NamePool::Ptr &namePool) const;

            /**
             * Sets the derivation @p constraints of the type.
             */
            void setDerivationConstraints(const SchemaType::DerivationConstraints &constraints);

            /**
             * Returns the derivation constraints of the type.
             */
            SchemaType::DerivationConstraints derivationConstraints() const;

        private:
            QXmlName                          m_name;
            SchemaType::DerivationConstraints m_derivationConstraints;
    };

    #include "qxsduserschematype.cpp"
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

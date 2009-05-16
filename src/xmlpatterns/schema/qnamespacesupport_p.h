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

#ifndef Patternist_NamespaceSupport_H
#define Patternist_NamespaceSupport_H

#include "qnamepool_p.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QStack>
#include <QtCore/QXmlStreamNamespaceDeclarations>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A helper class for handling nested namespace declarations.
     *
     * This class is mostly an adaption of QXmlNamespaceSupport to the NamePool
     * mechanism used in XmlPatterns.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class NamespaceSupport
    {
        public:
            /**
             * Describes whether the name to process is an attribute or element.
             */
            enum NameType
            {
                AttributeName,  ///< An attribute name to process.
                ElementName     ///< An element name to process.
            };

            /**
             * Creates an empty namespace support object.
             */
            NamespaceSupport();

            /**
             * Creates a new namespace support object.
             *
             * @param namePool The name pool where all processed names are stored in.
             */
            NamespaceSupport(const NamePool::Ptr &namePool);

            /**
             * Adds a new prefix-to-namespace binding.
             *
             * @param prefixCode The name pool code for the prefix.
             * @param namespaceCode The name pool code for the namespace.
             */
            void setPrefix(const QXmlName::PrefixCode prefixCode, const QXmlName::NamespaceCode namespaceCode);

            /**
             * Adds the prefix-to-namespace bindings from @p declarations to
             * the namespace support.
             */
            void setPrefixes(const QXmlStreamNamespaceDeclarations &declarations);

            /**
             * Sets the name pool code of the target namespace of the schema the
             * namespace support works on.
             */
            void setTargetNamespace(const QXmlName::NamespaceCode code);

            /**
             * Returns the prefix code for the given namespace @p code.
             */
            QXmlName::PrefixCode prefix(const QXmlName::NamespaceCode code) const;

            /**
             * Returns the namespace code for the given prefix @p code.
             */
            QXmlName::NamespaceCode uri(const QXmlName::PrefixCode code) const;

            /**
             * Converts the given @p qualifiedName to a resolved QXmlName @p name according
             * to the current namespace mapping.
             *
             * @param qualifiedName The full qualified name.
             * @param type The type of name processing.
             * @param name The resolved QXmlName.
             *
             * @returns @c true if the name could be processed correctly or @c false if the
             *          namespace prefix is unknown.
             */
            bool processName(const QString &qualifiedName, NameType type, QXmlName &name) const;

            /**
             * Pushes the current namespace mapping onto the stack.
             */
            void pushContext();

            /**
             * Pops the current namespace mapping from the stack.
             */
            void popContext();

            /**
             * Returns the list of namespace bindings.
             */
            QList<QXmlName> namespaceBindings() const;

        private:
            typedef QHash<QXmlName::PrefixCode, QXmlName::NamespaceCode> NamespaceHash;

            NamePool::Ptr         m_namePool;
            QStack<NamespaceHash> m_nsStack;
            NamespaceHash         m_ns;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

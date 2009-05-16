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

#ifndef Patternist_XsdInstanceReader_H
#define Patternist_XsdInstanceReader_H

#include "qabstractxmlnodemodel.h"
#include "qpullbridge_p.h"
#include "qxsdschemacontext_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short The schema instance reader.
     *
     * This class reads in a xml instance document from a QAbstractXmlNodeModel
     * and provides a QXmlStreamReader like interface with some additional context
     * information.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdInstanceReader
    {
        public:
            typedef QExplicitlySharedDataPointer<XsdInstanceReader> Ptr;

            /**
             * Creates a new instance reader that will read the data from
             * the given @p model.
             *
             * @param model The model the data are read from.
             * @param context The context that is used for error reporting etc.
             */
            XsdInstanceReader(const QAbstractXmlNodeModel *model, const XsdSchemaContext::Ptr &context);

        protected:
            /**
             * Returns @c true if the end of the document is reached, @c false otherwise.
             */
            bool atEnd() const;

            /**
             * Reads the next node from the document.
             */
            void readNext();

            /**
             * Returns whether the current node is a start element.
             */
            bool isStartElement() const;

            /**
             * Returns whether the current node is an end element.
             */
            bool isEndElement() const;

            /**
             * Returns whether the current node has a text node among its children.
             */
            bool hasChildText() const;

            /**
             * Returns whether the current node has an element node among its children.
             */
            bool hasChildElement() const;

            /**
             * Returns the name of the current node.
             */
            QXmlName name() const;

            /**
             * Returns whether the current node has an attribute with the given @p name.
             */
            bool hasAttribute(const QXmlName &name) const;

            /**
             * Returns the attribute with the given @p name of the current node.
             */
            QString attribute(const QXmlName &name) const;

            /**
             * Returns the list of attribute names of the current node.
             */
            QSet<QXmlName> attributeNames() const;

            /**
             * Returns the concatenated text of all direct child text nodes.
             */
            QString text() const;

            /**
             * Converts a qualified name into a QXmlName according to the namespace
             * mappings of the current node.
             */
            QXmlName convertToQName(const QString &name) const;

            /**
             * Returns a source location object for the current position.
             */
            QSourceLocation sourceLocation() const;

            /**
             * Returns the QXmlItem for the current position.
             */
            QXmlItem item() const;

            /**
             * Returns the QXmlItem for the attribute with the given @p name at the current position.
             */
            QXmlItem attributeItem(const QXmlName &name) const;

            /**
             * Returns the namespace bindings for the given node model @p index.
             */
            QVector<QXmlName> namespaceBindings(const QXmlNodeModelIndex &index) const;

            /**
             * The shared schema context.
             */
            XsdSchemaContext::Ptr     m_context;

        private:
            PullBridge                m_model;
            QHash<QXmlName, QString>  m_cachedAttributes;
            QHash<QXmlName, QXmlItem> m_cachedAttributeItems;
            QSourceLocation           m_cachedSourceLocation;
            QXmlItem                  m_cachedItem;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

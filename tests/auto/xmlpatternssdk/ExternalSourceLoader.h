/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#ifndef PatternistSDK_ExternalSourceLoader_H
#define PatternistSDK_ExternalSourceLoader_H

#include <QHash>
#include <QUrl>
#include <QXmlQuery>

#include "qdynamiccontext_p.h"
#include "qresourceloader_p.h"
#include "qexternalvariableloader_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternistSDK
{
    /**
     * @short Handles external variables in XQTS queries, such as <tt>$input-context</tt>,
     * by loading appropriate XML source files.
     *
     * @ingroup PatternistSDK
     * @author Frans Englich <frans.englich@nokia.com>
     */
    class Q_PATTERNISTSDK_EXPORT ExternalSourceLoader : public QPatternist::ExternalVariableLoader
    {
    public:
        enum TargetOfURI
        {
            /**
             * Identifies @c input-file.
             */
            Document,

            /**
             * Identifies @c input-URI.
             */
            URI,

            /**
             * Identifies @c input-query.
             */
            Query
        };

        /**
         * The first is the complete, absolute, final URI to the file to be loaded,
         * and the second is the type of source found at that URI.
         */
        typedef QPair<QUrl, TargetOfURI> VariableValue;

        /**
         * In the XQTSCatalog.xml each source file in each test is referred to
         * by a key, which can be fully looked up in the @c sources element. This QHash
         * maps the keys to absolute URIs pointing to the source file.
         */
        typedef QHash<QString, QUrl> SourceMap;

        /**
         * The first value is the variable name, and the second is the URI identifying
         * the XML source file that's supposed to be loaded as a document.
         *
         * This is one for every test case, except for @c rdb-queries-results-q5,
         * @c rdb-queries-results-q17 and @c rdb-queries-results-q18(at least in XQTS 1.0).
         */
        typedef QHash<QString, VariableValue> VariableMap;

        ExternalSourceLoader(const VariableMap &varMap,
                             const QPatternist::ResourceLoader::Ptr &resourceLoader);

        virtual QPatternist::SequenceType::Ptr
        announceExternalVariable(const QXmlName name,
                                 const QPatternist::SequenceType::Ptr &declaredType);

        virtual QPatternist::Item
        evaluateSingleton(const QXmlName name,
                          const QPatternist::DynamicContext::Ptr &context);

        VariableMap variableMap() const
        {
            return m_variableMap;
        }

    private:
        const VariableMap                       m_variableMap;
        const QPatternist::ResourceLoader::Ptr  m_resourceLoader;
        QXmlQuery                               m_query;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4

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

#ifndef PATTERNIST_PULLBRIDGE_P_H
#define PATTERNIST_PULLBRIDGE_P_H

#include <QtCore/QPair>
#include <QtCore/QStack>

#include "qabstractxmlforwarditerator_p.h"
#include "qabstractxmlpullprovider_p.h"
#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class PullBridge : public AbstractXmlPullProvider
    {
    public:
        inline PullBridge(const QXmlNodeModelIndex::Iterator::Ptr &it) : m_current(StartOfInput)
        {
            Q_ASSERT(it);
            m_iterators.push(qMakePair(StartOfInput, it));
        }

        virtual Event next();
        virtual Event current() const;
        virtual QXmlName name() const;
        /**
         * Returns always an empty QVariant.
         */
        virtual QVariant atomicValue() const;
        virtual QString stringValue() const;
        virtual QHash<QXmlName, QString> attributes();
        virtual QHash<QXmlName, QXmlItem> attributeItems();

        QXmlNodeModelIndex index() const;
        QSourceLocation sourceLocation() const;

    private:
        typedef QStack<QPair<Event, QXmlNodeModelIndex::Iterator::Ptr> > IteratorStack;
        IteratorStack      m_iterators;
        QXmlNodeModelIndex m_index;
        Event              m_current;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

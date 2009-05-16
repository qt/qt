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

#ifndef QABSTRACTXMLPULLPROVIDER_H
#define QABSTRACTXMLPULLPROVIDER_H

#include <QtCore/QtGlobal>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QXmlItem;
class QXmlName;
class QString;
class QVariant;
template<typename Key, typename Value> class QHash;

namespace QPatternist
{
    class AbstractXmlPullProviderPrivate;

    class AbstractXmlPullProvider
    {
    public:
        AbstractXmlPullProvider();
        virtual ~AbstractXmlPullProvider();

        enum Event
        {
            StartOfInput            = 1,
            AtomicValue             = 1 << 1,
            StartDocument           = 1 << 2,
            EndDocument             = 1 << 3,
            StartElement            = 1 << 4,
            EndElement              = 1 << 5,
            Text                    = 1 << 6,
            ProcessingInstruction   = 1 << 7,
            Comment                 = 1 << 8,
            Attribute               = 1 << 9,
            Namespace               = 1 << 10,
            EndOfInput              = 1 << 11
        };

        virtual Event next() = 0;
        virtual Event current() const = 0;
        virtual QXmlName name() const = 0;
        virtual QVariant atomicValue() const = 0;
        virtual QString stringValue() const = 0;

        virtual QHash<QXmlName, QString> attributes() = 0;
        virtual QHash<QXmlName, QXmlItem> attributeItems() = 0;

        /* *** The functions below are internal. */
    private:
        Q_DISABLE_COPY(AbstractXmlPullProvider)
        AbstractXmlPullProviderPrivate *d;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

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

#include "qxsdidcache_p.h"

#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdIdCache::addId(const QString &id)
{
    const QWriteLocker locker(&m_lock);
    Q_ASSERT(!m_ids.contains(id));

    m_ids.insert(id);
}

bool XsdIdCache::hasId(const QString &id) const
{
    const QReadLocker locker(&m_lock);

    return m_ids.contains(id);
}

QT_END_NAMESPACE

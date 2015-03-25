/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qinteger_p.h"

#include "qtocodepointsiterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ToCodepointsIterator::ToCodepointsIterator(const QString &string)
                             : m_string(string),
                               m_len(string.length()),
                               m_position(0)
{
    Q_ASSERT(!string.isEmpty());
}

Item ToCodepointsIterator::next()
{
    if(m_position == -1)
        return Item();

    ++m_position;
    if(m_position > m_len)
    {
        m_position = -1;
        m_current.reset();
        return m_current;
    }

    m_current = Integer::fromValue(m_string.at(m_position - 1).unicode());
    return m_current;
}

xsInteger ToCodepointsIterator::count()
{
    return m_len;
}

Item ToCodepointsIterator::current() const
{
    return m_current;
}

xsInteger ToCodepointsIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr ToCodepointsIterator::copy() const
{
    return Item::Iterator::Ptr(new ToCodepointsIterator(m_string));
}

QT_END_NAMESPACE

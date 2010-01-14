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

#include <QtDebug>

#include "TreeSortFilter.h"

using namespace QPatternistSDK;

TreeSortFilter::TreeSortFilter(QObject *p) : QSortFilterProxyModel(p)
{
    Q_ASSERT(p);
}

bool TreeSortFilter::lessThan(const QModelIndex &left,
                              const QModelIndex &right) const
{
    const QVariant leftData(sourceModel()->data(left));
    const QVariant rightData(sourceModel()->data(right));

    return numericLessThan(leftData.toString(), rightData.toString());
}

bool TreeSortFilter::numericLessThan(const QString &l, const QString &r) const
{
    QString ls(l);
    QString rs(r);
    const int len = (l.length() > r.length() ? r.length() : l.length());

    for(int i = 0;i < len; ++i)
    {
        const QChar li(l.at(i));
        const QChar ri(r.at(i));

        if(li >= QLatin1Char('0') &&
           li <= QLatin1Char('9') &&
           ri >= QLatin1Char('0') &&
           ri <= QLatin1Char('9'))
        {
            ls = l.mid(i);
            rs = r.mid(i);
            break;
        }
        else if(li != ri)
            break;
    }

    const int ld = ls.toInt();
    const int rd = rs.toInt();

    if(ld == rd)
        return ls.localeAwareCompare(rs) < 0;
    else
        return ld < rd;
}

bool TreeSortFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(filterRegExp().isEmpty())
        return true;

    QModelIndex current(sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent));

    if(sourceModel()->hasChildren(current))
    {
        bool atLeastOneValidChild = false;
        int i = 0;
        while(!atLeastOneValidChild)
        {
            const QModelIndex child(current.child(i, current.column()));
            if(!child.isValid())
                // No valid child
                break;

            atLeastOneValidChild = filterAcceptsRow(i, current);
            i++;
        }
        return atLeastOneValidChild;
    }

    return sourceModel()->data(current).toString().contains(filterRegExp());
}

// vim: et:ts=4:sw=4:sts=4

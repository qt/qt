/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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

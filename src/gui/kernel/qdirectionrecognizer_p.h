/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDIRECTIONRECOGNIZER_P_H
#define QDIRECTIONRECOGNIZER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qpoint.h"
#include "qlist.h"

QT_BEGIN_NAMESPACE

struct Direction
{
    Qt::DirectionType direction;
    QPoint point;

    Direction(Qt::DirectionType dir, const QPoint &pt)
        : direction(dir), point(pt) { }
    Direction()
        : direction(Qt::NoDirection) { }

    inline bool isEmpty() const { return direction == Qt::NoDirection; }
    inline bool isNull() const { return direction == Qt::NoDirection; }
};

typedef QList<Direction> DirectionList;

class QDirectionSimpleRecognizer
{
public:
    QDirectionSimpleRecognizer();
    Direction addPosition(const QPoint &pos);
    DirectionList getDirections() const;
    void reset();

private:
    QPoint lastPoint;
    DirectionList directions;
};

class QDirectionDiagonalRecognizer
{
public:
    QDirectionDiagonalRecognizer();
    Direction addPosition(const QPoint &pos);
    DirectionList getDirections() const;
    void reset();

private:
    QPoint lastPoint;
    DirectionList directions;
};

QT_END_NAMESPACE

#endif // QDIRECTIONRECOGNIZER_P_H

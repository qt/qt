/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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

#ifndef QDIRECTIONSIMPLIFICATOR_P_H
#define QDIRECTIONSIMPLIFICATOR_P_H

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

#include "private/qdirectionrecognizer_p.h"

QT_BEGIN_NAMESPACE

class QDirectionSimplificator
{
public:
    QDirectionSimplificator(const DirectionList &dir);

    bool simplify(DirectionList *result);

private:
    DirectionList directions;
    DirectionList lastResult;
    enum State {
        None,
        Trim, // remove first and last element
        AccidentalMoves, // 66866 => 6666
        ComplexAccidentalMoves, // 778788 => 777888 (swapping elements without changing direction)
        ShortMoves, // (moves of length 1)
    } state;

    struct SimplifyTrim
    {
        SimplifyTrim() : state(0) { }
        bool operator()(DirectionList &directions)
        {
            if (state == 0) {
                directions.removeFirst();
                state = 1;
            } else if (state == 1) {
                directions.removeLast();
                state = 2;
            } else if (state == 2 && directions.size() >= 2) {
                directions.removeFirst();
                directions.removeLast();
                state = 3;
            } else {
                return false;
            }
            return true;
        }
        int state;
    };
    struct SimplifyAccidentalMoves
    {
        SimplifyAccidentalMoves() : state(0) { }
        bool operator()(DirectionList &directions)
        {
            return false;
        }
        int state;
    };
    struct SimplifyComplexAccidentalMoves
    {
        SimplifyComplexAccidentalMoves() : state(0) { }
        bool operator()(DirectionList &directions)
        {
            return false;
        }
        int state;
    };

    SimplifyTrim trim;
    SimplifyAccidentalMoves accidentalMoves;
    SimplifyComplexAccidentalMoves complexAccidentalMoves;
    //SimplifyShortMoves shortMoves;
};

QDirectionSimplificator::QDirectionSimplificator(const DirectionList &dir)
    : directions(dir), state(None)
{
}

bool QDirectionSimplificator::simplify(DirectionList *result)
{
    if (directions.isEmpty() || !result)
        return false;
    *result = directions;
    switch(state) {
    case None:
        state = Trim;
        trim = SimplifyTrim();
    case Trim:
        if (trim(*result))
            break;
        *result = lastResult;
        state = AccidentalMoves;
        accidentalMoves = SimplifyAccidentalMoves();
    case AccidentalMoves:
        if (accidentalMoves(*result))
            break;
        *result = lastResult;
        state = ComplexAccidentalMoves;
        complexAccidentalMoves = SimplifyComplexAccidentalMoves();
    case ComplexAccidentalMoves:
        if (complexAccidentalMoves(*result))
            break;
        *result = lastResult;
    //     state = ShortMoves;
    //     shortMoves = SimplifyShortMoves();
    // case ShortMoves:
    //     if (shortMoves(*result))
    //         break;
    //     state = None;
    default:
        return false;
    }
    lastResult = *result;
    if (lastResult.isEmpty())
        return false;
    return true;
}

QT_END_NAMESPACE

#endif // QDIRECTIONSIMPLIFICATOR_P_H

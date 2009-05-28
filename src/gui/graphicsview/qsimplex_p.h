/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ $TROLLTECH$. All rights reserved.
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QSIMPLEX_P_H
#define QSIMPLEX_P_H

#include <QtCore/qhash.h>
#include <QtCore/qpair.h>

struct QSimplexVariable
{
    QSimplexVariable() : result(0), index(0) {};

    qreal result;
    uint index;
};


/*!
  \internal

  Representation of a LP constraint like:

    (c1 * X1) + (c2 * X2) + ...  =  K
                             or <=  K
                             or >=  K

    Where (ci, Xi) are the pairs in "variables" and K the real "constant".
*/
struct QSimplexConstraint
{
    QSimplexConstraint() : constant(0), ratio(Equal), artificial(0) {};

    enum Ratio {
        LessOrEqual = 0,
        Equal,
        MoreOrEqual
    };

    QHash<QSimplexVariable *, qreal> variables;
    qreal constant;
    Ratio ratio;

    QPair<QSimplexVariable *, qreal> helper;
    QSimplexVariable * artificial;
};


class QSimplex
{
public:
    QSimplex();
    virtual ~QSimplex();

    qreal solveMin();
    qreal solveMax();
    QList<QSimplexVariable *> constraintsVariables();

    void setConstraints(const QList<QSimplexConstraint *> constraints);
    void setObjective(QSimplexConstraint *objective);

    void dumpMatrix();

private:
    // Matrix handling
    qreal valueAt(int row, int column);
    void setValueAt(int row, int column, qreal value);
    void clearRow(int rowIndex);
    void clearColumns(int first, int last);
    void combineRows(int toIndex, int fromIndex, qreal factor);

    // Simplex
    int findPivotColumn();
    int pivotRowForColumn(int column);
    void reducedRowEchelon();
    bool iterate();

    // Helpers
    void clearDataStructures();
    void solveMaxHelper();
    void collectResults();

    QList<QSimplexConstraint *> constraints;
    QList<QSimplexVariable *> variables;
    QSimplexConstraint *objective;

    int rows;
    int columns;
    int firstArtificial;

    qreal *matrix;
};

inline QList<QSimplexVariable *> QSimplex::constraintsVariables()
{
    return variables;
}

inline qreal QSimplex::valueAt(int rowIndex, int columnIndex)
{
    return matrix[rowIndex * columns + columnIndex];
}

inline void QSimplex::setValueAt(int rowIndex, int columnIndex, qreal value)
{
    matrix[rowIndex * columns + columnIndex] = value;
}


#endif

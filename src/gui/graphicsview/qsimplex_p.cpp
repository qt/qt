#include "qsimplex_p.h"

#include <QtCore/qset.h>
#include <QtCore/qdebug.h>

#include <stdlib.h>

QT_BEGIN_NAMESPACE

QSimplex::QSimplex() : objective(0), rows(0), columns(0), firstArtificial(0), matrix(0)
{
}

QSimplex::~QSimplex()
{
    clearDataStructures();
}

void QSimplex::clearDataStructures()
{
    if (matrix == 0)
        return;

    // Matrix
    rows = 0;
    columns = 0;
    firstArtificial = 0;
    free(matrix);
    matrix = 0;

    // Constraints
    for (int i = 0; i < constraints.size(); ++i) {
        delete constraints[i]->helper.first;
        constraints[i]->helper.first = 0;
        constraints[i]->helper.second = 0.0;
        delete constraints[i]->artificial;
        constraints[i]->artificial = 0;
    }
    constraints.clear();

    // Other
    variables.clear();
    objective = 0;
}

void QSimplex::setConstraints(const QList<QSimplexConstraint *> newConstraints)
{
    clearDataStructures();

    if (newConstraints.isEmpty())
        return;
    constraints = newConstraints;

    // Set Variables direct mapping
    QSet<QSimplexVariable *> variablesSet;
    for (int i = 0; i < constraints.size(); ++i)
        variablesSet += \
            QSet<QSimplexVariable *>::fromList(constraints[i]->variables.keys());
    variables = variablesSet.toList();

    // Set Variables reverse mapping
    for (int i = 0; i < variables.size(); ++i) {
        // The variable "0" goes at the column "1", etc...
        variables[i]->index = i + 1;
    }

    // Normalize Constraints
    int variableIndex = variables.size();
    QList <QSimplexVariable *> artificialList;

    for (int i = 0; i < constraints.size(); ++i) {
        QSimplexVariable *slack;
        QSimplexVariable *surplus;
        QSimplexVariable *artificial;

        Q_ASSERT(constraints[i]->helper.first == 0);
        Q_ASSERT(constraints[i]->artificial == 0);

        switch(constraints[i]->ratio) {
        case QSimplexConstraint::LessOrEqual:
            slack = new QSimplexVariable;
            slack->index = ++variableIndex;
            constraints[i]->helper.first = slack;
            constraints[i]->helper.second = 1.0;
            break;
        case QSimplexConstraint::MoreOrEqual:
            surplus = new QSimplexVariable;
            surplus->index = ++variableIndex;
            constraints[i]->helper.first = surplus;
            constraints[i]->helper.second = -1.0;
            // fall through
        case QSimplexConstraint::Equal:
            artificial = new QSimplexVariable;
            constraints[i]->artificial = artificial;
            artificialList += constraints[i]->artificial;
            break;
        }
    }

    firstArtificial = variableIndex + 1;
    for (int i = 0; i < artificialList.size(); ++i)
        artificialList[i]->index = ++variableIndex;
    artificialList.clear();

    // Matrix

    // One for each variable plus the Basic and BFS columns (first and last)
    columns = variableIndex + 2;
    // One for each constraint plus the objective function
    rows = constraints.size() + 1;

    matrix = (qreal *)malloc(sizeof(qreal) * columns * rows);
    if (!matrix) {
        qWarning() << "QSimplex: Unable to allocate memory!";
        return;
    }
    for (int i = columns * rows - 1; i >= 0; --i)
        matrix[i] = 0.0;

    // Fill Matrix
    for (int i = 1; i <= constraints.size(); ++i) {
        QSimplexConstraint *c = constraints[i - 1];

        if (c->artificial) {
            // Will use artificial basic variable
            setValueAt(i, 0, c->artificial->index);
            setValueAt(i, c->artificial->index, 1.0);

            if (c->helper.second != 0.0) {
                // Surplus variable
                setValueAt(i, c->helper.first->index, c->helper.second);
            }
        } else {
            // Slack is used as the basic variable
            Q_ASSERT(c->helper.second == 1.0);
            setValueAt(i, 0, c->helper.first->index);
            setValueAt(i, c->helper.first->index, 1.0);
        }

        QHash<QSimplexVariable *, qreal>::const_iterator iter;
        for (iter = c->variables.constBegin();
             iter != c->variables.constEnd();
             ++iter) {
            setValueAt(i, iter.key()->index, iter.value());
        }

        setValueAt(i, columns - 1, c->constant);
    }

    // Set temporary objective: -1 * sum_of_artificial_vars
    for (int j = firstArtificial; j < columns - 1; ++j)
        setValueAt(0, j, 1.0);

    // Maximize our objective (artificial vars go to zero)
    solveMaxHelper();

    if (valueAt(0, columns - 1) != 0.0) {
        qWarning() << "QSimplex: No feasible solution!";
        clearDataStructures();
        return;
    }

    // Remove artificial variables
    clearColumns(firstArtificial, columns - 2);
}

void QSimplex::solveMaxHelper()
{
    reducedRowEchelon();
    while (iterate()) ;
}

void QSimplex::setObjective(QSimplexConstraint *newObjective)
{
    objective = newObjective;
}

void QSimplex::clearRow(int rowIndex)
{
    qreal *item = matrix + rowIndex * columns;
    for (int i = 0; i < columns; ++i)
        item[i] = 0.0;
}

void QSimplex::clearColumns(int first, int last)
{
    for (int i = 0; i < rows; ++i) {
        qreal *row = matrix + i * columns;
        for (int j = first; j <= last; ++j)
            row[j] = 0.0;
    }
}

void QSimplex::dumpMatrix()
{
    printf("---- Simplex Matrix ----\n");

    printf("       ");
    for (int j = 0; j < columns; ++j)
        printf("  <% 2d >", j);
    printf("\n");

    for (int i = 0; i < rows; ++i) {
        printf("Row %2d:", i);

        qreal *row = matrix + i * columns;
        for (int j = 0; j < columns; ++j) {
            printf("  % 2.2f", row[j]);
        }
        printf("\n");
    }
    printf("------------------------\n\n");
}

void QSimplex::combineRows(int toIndex, int fromIndex, qreal factor)
{
    if (!factor)
        return;

    qreal *from = matrix + fromIndex * columns;
    qreal *to = matrix + toIndex * columns;

    for (int j = 1; j < columns; ++j) {
        qreal value = from[j];

        // skip to[j] = to[j] + factor*0.0
        if (value == 0.0)
            continue;

        to[j] += factor * value;

        // ### Avoid Numerical errors
        if (qAbs(to[j]) < 0.0000000001)
            to[j] = 0.0;
    }
}

int QSimplex::findPivotColumn()
{
    qreal min = 0;
    int minIndex = -1;

    for (int j = 0; j < columns-1; ++j) {
        if (valueAt(0, j) < min) {
            min = valueAt(0, j);
            minIndex = j;
        }
    }

    return minIndex;
}

int QSimplex::pivotRowForColumn(int column)
{
    qreal min = 999999999999.0; // ###
    int minIndex = -1;

    for (int i = 1; i < rows; ++i) {
        qreal divisor = valueAt(i, column);
        if (divisor <= 0)
            continue;

        qreal quotient = valueAt(i, columns - 1) / divisor;
        if (quotient < min) {
            min = quotient;
            minIndex = i;
        }
    }

    return minIndex;
}

void QSimplex::reducedRowEchelon()
{
    for (int i = 1; i < rows; ++i) {
        int factorInObjectiveRow = valueAt(i, 0);
        combineRows(0, i, -1 * valueAt(0, factorInObjectiveRow));
    }
}

bool QSimplex::iterate()
{
    // Find Pivot column
    int pivotColumn = findPivotColumn();
    if (pivotColumn == -1)
        return false;

    // Find Pivot row for column
    int pivotRow = pivotRowForColumn(pivotColumn);
    if (pivotRow == -1) {
        qWarning() << "QSimplex: Unbounded problem!";
        return false;
    }

    // Normalize Pivot Row
    qreal pivot = valueAt(pivotRow, pivotColumn);
    if (pivot != 1.0)
        combineRows(pivotRow, pivotRow, (1.0 - pivot) / pivot);

    // Update other rows
    for (int row=0; row < rows; ++row) {
        if (row == pivotRow)
            continue;

        combineRows(row, pivotRow, -1 * valueAt(row, pivotColumn));
    }

    // Update first column
    setValueAt(pivotRow, 0, pivotColumn);

    //    dumpMatrix();
    //    printf("------------ end of iteration --------------\n");
    return true;
}

/*!
  \internal

  Both solveMin and solveMax are interfaces to this method.

  The enum solverFactor admits 2 values: Minimum (-1) and Maximum (+1).
 */
qreal QSimplex::solver(solverFactor factor)
{
    // Remove old objective
    clearRow(0);

    // Set new objective
    QHash<QSimplexVariable *, qreal>::const_iterator iter;
    for (iter = objective->variables.constBegin();
         iter != objective->variables.constEnd();
         ++iter) {
        setValueAt(0, iter.key()->index, -1 * factor * iter.value());
    }

    solveMaxHelper();
    collectResults();

    return factor * valueAt(0, columns - 1);
}

qreal QSimplex::solveMin()
{
    return solver(Minimum);
}

qreal QSimplex::solveMax()
{
    return solver(Maximum);
}

void QSimplex::collectResults()
{
    // All variables are zero unless overridden below.

    // ### Is this really needed? Is there any chance that an
    // important variable remains as non-basic at the end of simplex?
    for (int i = 0; i < variables.size(); ++i)
        variables[i]->result = 0;

    // Basic variables
    // Update the variable indicated in the first column with the value
    // in the last column.
    for (int i = 1; i < rows; ++i) {
        int index = valueAt(i, 0) - 1;
        if (index < variables.size())
            variables[index]->result = valueAt(i, columns - 1);
    }
}

QT_END_NAMESPACE

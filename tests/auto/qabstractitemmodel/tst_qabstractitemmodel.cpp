/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>
#include <QtCore/QtCore>

//TESTED_CLASS=QAbstractListModel QAbstractTableModel
//TESTED_FILES=

/*!
    Note that this doesn't test models, but any functionality that QAbstractItemModel shoudl provide
 */
class tst_QAbstractItemModel : public QObject
{
    Q_OBJECT

public:
    tst_QAbstractItemModel();
    virtual ~tst_QAbstractItemModel();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void index();
    void parent();
    void hasChildren();
    void _data();
    void headerData();
    void itemData();
    void itemFlags();
    void match();
    void dropMimeData_data();
    void dropMimeData();
    void changePersistentIndex();
    void movePersistentIndex();

    void insertRows();
    void insertColumns();
    void removeRows();
    void removeColumns();

    void reset();

    void complexChangesWithPersistent();

};

/*!
    Test model that impliments the pure vitual functions and anything else that is
    needed.

    It is a table implimented as a vector of vectors of strings.
 */
class QtTestModel: public QAbstractItemModel
{
public:
    QtTestModel(int rows, int columns, QObject *parent = 0);
    QtTestModel(const QVector<QVector<QString> > tbl, QObject *parent = 0);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool hasChildren(const QModelIndex &) const;
    QVariant data(const QModelIndex &idx, int) const;
    bool setData(const QModelIndex &idx, const QVariant &value, int);
    bool insertRows(int row, int count, const QModelIndex &parent= QModelIndex());
    bool insertColumns(int column, int count, const QModelIndex &parent= QModelIndex());
    void setPersistent(const QModelIndex &from, const QModelIndex &to);
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool removeColumns( int column, int count, const QModelIndex & parent = QModelIndex());
    void reset();

    int cCount, rCount;
    mutable bool wrongIndex;
    QVector<QVector<QString> > table;
};

QtTestModel::QtTestModel(int rows, int columns, QObject *parent)
    : QAbstractItemModel(parent), cCount(columns), rCount(rows), wrongIndex(false) {

    table.resize(rows);
    for (int r = 0; r < rows; ++r) {
        table[r].resize(columns);
        for (int c = 0; c < columns; ++c)
            table[r][c] = QString("%1/%2").arg(r).arg(c);
    }
}

QtTestModel::QtTestModel(const QVector<QVector<QString> > tbl, QObject *parent)
    : QAbstractItemModel(parent), wrongIndex(false) {
    table = tbl;
    rCount = tbl.count();
    cCount = tbl.at(0).count();
}

QModelIndex QtTestModel::index(int row, int column, const QModelIndex &parent) const
        { return hasIndex(row, column, parent) ? createIndex(row, column, 0) : QModelIndex(); }

QModelIndex QtTestModel::parent(const QModelIndex &) const { return QModelIndex(); }
int QtTestModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : rCount; }
int QtTestModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : cCount; }
bool QtTestModel::hasChildren(const QModelIndex &) const { return false; }

QVariant QtTestModel::data(const QModelIndex &idx, int) const
{
    if (idx.row() < 0 || idx.column() < 0 || idx.column() > cCount || idx.row() > rCount) {
        wrongIndex = true;
        qWarning("got invalid modelIndex %d/%d", idx.row(), idx.column());
        return QVariant();
    }
    return table.at(idx.row()).at(idx.column());
}

bool QtTestModel::setData(const QModelIndex &idx, const QVariant &value, int)
{
    table[idx.row()][idx.column()] = value.toString();
    return true;
}

bool QtTestModel::insertRows(int row, int count, const QModelIndex &parent)
{
    QAbstractItemModel::beginInsertRows(parent, row, row + count - 1);
    int cc = columnCount(parent);
    table.insert(row, count, QVector<QString>(cc));
    rCount = table.count();
    QAbstractItemModel::endInsertRows();
    return true;
}

bool QtTestModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    QAbstractItemModel::beginInsertColumns(parent, column, column + count - 1);
    int rc = rowCount(parent);
    for (int i = 0; i < rc; ++i)
        table[i].insert(column, 1, "");
    cCount = table.at(0).count();
    QAbstractItemModel::endInsertColumns();
    return true;
}

void QtTestModel::setPersistent(const QModelIndex &from, const QModelIndex &to)
{
    changePersistentIndex(from, to);
}

bool QtTestModel::removeRows( int row, int count, const QModelIndex & parent)
{
    QAbstractItemModel::beginRemoveRows(parent, row, row + count - 1);

    for (int r = row+count-1; r >= row; --r)
        table.remove(r);
    rCount = table.count();

    QAbstractItemModel::endRemoveRows();
    return true;
}

bool QtTestModel::removeColumns(int column, int count, const QModelIndex & parent)
{
    QAbstractItemModel::beginRemoveColumns(parent, column, column + count - 1);

    for (int c = column+count-1; c > column; --c)
        for (int r = 0; r < rCount; ++r)
            table[r].remove(c);

    cCount = table.at(0).count();

    QAbstractItemModel::endRemoveColumns();
    return true;
}

void QtTestModel::reset()
{
    QAbstractItemModel::reset();
}

tst_QAbstractItemModel::tst_QAbstractItemModel()
{
}

tst_QAbstractItemModel::~tst_QAbstractItemModel()
{
}

/**
 * The source Model *must* be initialized before the _data function, since the _data function uses QModelIndexes to reference the items in the tables.
 * Therefore, we must initialize it globally.
 */

void tst_QAbstractItemModel::initTestCase()
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
}

void tst_QAbstractItemModel::cleanupTestCase()
{

}

void tst_QAbstractItemModel::init()
{

}

void tst_QAbstractItemModel::cleanup()
{

}

/*
  tests
*/

void tst_QAbstractItemModel::index()
{
    QtTestModel model(1, 1);
    QModelIndex idx = model.index(0, 0, QModelIndex());
    QVERIFY(idx.isValid());
}

void tst_QAbstractItemModel::parent()
{
    QtTestModel model(1, 1);
    QModelIndex idx = model.index(0, 0, QModelIndex());
    QModelIndex par = model.parent(idx);
    QVERIFY(!par.isValid());
}

void tst_QAbstractItemModel::hasChildren()
{
    QtTestModel model(1, 1);
    QModelIndex idx = model.index(0, 0, QModelIndex());
    QVERIFY(model.hasChildren(idx) == false);
}

void tst_QAbstractItemModel::_data()
{
    QtTestModel model(1, 1);
    QModelIndex idx = model.index(0, 0, QModelIndex());
    QVERIFY(idx.isValid());
    QCOMPARE(model.data(idx, Qt::DisplayRole).toString(), QString("0/0"));

    // Default does nothing
    QCOMPARE(model.setHeaderData(0, Qt::Horizontal, QVariant(0), 0), false);
}

void tst_QAbstractItemModel::headerData()
{
    QtTestModel model(1, 1);
    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DisplayRole).toString(),
            QString("1"));

    // Default text alignment for header must be invalid
    QVERIFY( !model.headerData(0, Qt::Horizontal, Qt::TextAlignmentRole).isValid() );
}

void tst_QAbstractItemModel::itemData()
{
    QtTestModel model(1, 1);
    QModelIndex idx = model.index(0, 0, QModelIndex());
    QVERIFY(idx.isValid());
    QMap<int, QVariant> dat = model.itemData(idx);
    QCOMPARE(dat.count(Qt::DisplayRole), 1);
    QCOMPARE(dat.value(Qt::DisplayRole).toString(), QString("0/0"));
}

void tst_QAbstractItemModel::itemFlags()
{
    QtTestModel model(1, 1);
    QModelIndex idx = model.index(0, 0, QModelIndex());
    QVERIFY(idx.isValid());
    Qt::ItemFlags flags = model.flags(idx);
    QCOMPARE(Qt::ItemIsSelectable|Qt::ItemIsEnabled, flags);
}

void tst_QAbstractItemModel::match()
{
    QtTestModel model(4, 1);
    QModelIndex start = model.index(0, 0, QModelIndex());
    QVERIFY(start.isValid());
    QModelIndexList res = model.match(start, Qt::DisplayRole, QVariant("1"), 3);
    QCOMPARE(res.count(), 1);
    QModelIndex idx = model.index(1, 0, QModelIndex());
    bool areEqual = (idx == res.first());
    QVERIFY(areEqual);

    model.setData(model.index(0, 0, QModelIndex()), "bat", Qt::DisplayRole);
    model.setData(model.index(1, 0, QModelIndex()), "cat", Qt::DisplayRole);
    model.setData(model.index(2, 0, QModelIndex()), "dog", Qt::DisplayRole);
    model.setData(model.index(3, 0, QModelIndex()), "boar", Qt::DisplayRole);

    res = model.match(start, Qt::DisplayRole, QVariant("dog"), -1, Qt::MatchExactly);
    QCOMPARE(res.count(), 1);
    res = model.match(start, Qt::DisplayRole, QVariant("a"), -1, Qt::MatchContains);
    QCOMPARE(res.count(), 3);
    res = model.match(start, Qt::DisplayRole, QVariant("b"), -1, Qt::MatchStartsWith);
    QCOMPARE(res.count(), 2);
    res = model.match(start, Qt::DisplayRole, QVariant("t"), -1, Qt::MatchEndsWith);
    QCOMPARE(res.count(), 2);
    res = model.match(start, Qt::DisplayRole, QVariant("*a*"), -1, Qt::MatchWildcard);
    QCOMPARE(res.count(), 3);
    res = model.match(start, Qt::DisplayRole, QVariant(".*O.*"), -1, Qt::MatchRegExp);
    QCOMPARE(res.count(), 2);
    res = model.match(start, Qt::DisplayRole, QVariant(".*O.*"), -1, Qt::MatchRegExp | Qt::MatchCaseSensitive);
    QCOMPARE(res.count(), 0);
    res = model.match(start, Qt::DisplayRole, QVariant("BOAR"), -1, Qt::MatchFixedString);
    QCOMPARE(res.count(), 1);
    res = model.match(start, Qt::DisplayRole, QVariant("bat"), -1,
                      Qt::MatchFixedString | Qt::MatchCaseSensitive);
    QCOMPARE(res.count(), 1);
}

typedef QPair<int, int> Position;
typedef QVector<QPair<int, int> > Selection;
typedef QVector<QVector<QString> > StringTable;
typedef QVector<QString> StringTableRow;
Q_DECLARE_METATYPE(Position)
Q_DECLARE_METATYPE(Selection)
Q_DECLARE_METATYPE(StringTable)

static StringTableRow qStringTableRow(const QString &s1, const QString &s2, const QString &s3)
{
    StringTableRow row;
    row << s1 << s2 << s3;
    return row;
}

#ifdef Q_CC_MSVC
# define STRINGTABLE (StringTable())
#else
# define STRINGTABLE StringTable()
#endif

void tst_QAbstractItemModel::dropMimeData_data()
{
    QTest::addColumn<StringTable>("src_table"); // drag source
    QTest::addColumn<StringTable>("dst_table"); // drop target
    QTest::addColumn<Selection>("selection"); // dragged items
    QTest::addColumn<Position>("dst_position"); // drop position
    QTest::addColumn<StringTable>("res_table"); // expected result

    {
        QTest::newRow("2x2 dropped at [0, 0]")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection
                << Position(0, 0) << Position(0, 1)
                << Position(1, 0) << Position(1, 1))
            << Position(0, 0) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("A", "B", "" ))
                << (qStringTableRow("D", "E", "" ))
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")));
    }

    {
        QTest::newRow("2x2 dropped at [1, 0]")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection
                << Position(0, 0) << Position(0, 1)
                << Position(1, 0) << Position(1, 1))
            << Position(1, 0) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("A", "B", "" ))
                << (qStringTableRow("D", "E", "" ))
                << (qStringTableRow("3", "4", "5")));
    }

    {
        QTest::newRow("2x2 dropped at [3, 0]")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection
                << Position(0, 0) << Position(0, 1)
                << Position(1, 0) << Position(1, 1))
            << Position(3, 0) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5"))
                << (qStringTableRow("A", "B", "" ))
                << (qStringTableRow("D", "E", "" )));
    }

    {
        QTest::newRow("2x2 dropped at [0, 1]")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection
                << Position(0, 0) << Position(0, 1)
                << Position(1, 0) << Position(1, 1))
            << Position(0, 1) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("" , "A", "B"))
                << (qStringTableRow("" , "D", "E"))
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")));
    }

    {
        QTest::newRow("2x2 dropped at [0, 2] (line break)")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection
                << Position(0, 0) << Position(0, 1)
                << Position(1, 0) << Position(1, 1))
            << Position(0, 2) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("" , "" , "A"))
                << (qStringTableRow("" , "" , "D"))
                << (qStringTableRow("" , "" , "B"))
                << (qStringTableRow("" , "" , "E"))
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")));
    }

    {
        QTest::newRow("2x2 dropped at [3, 2] (line break)")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection
                << Position(0, 0) << Position(0, 1)
                << Position(1, 0) << Position(1, 1))
            << Position(3, 2) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5"))
                << (qStringTableRow("" , "" , "A"))
                << (qStringTableRow("" , "" , "D"))
                << (qStringTableRow("" , "" , "B"))
                << (qStringTableRow("" , "" , "E")));
    }

    {
        QTest::newRow("non-square dropped at [0, 0]")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection
                << Position(0, 0) << Position(0, 1)
                << Position(1, 0))
            << Position(0, 0) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("A", "B", "" ))
                << (qStringTableRow("D", "" , "" ))
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")));
    }

    {
        QTest::newRow("non-square dropped at [0, 2]")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection
                << Position(0, 0) << Position(0, 1)
                << Position(1, 0))
            << Position(0, 2) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("" , "" , "A"))
                << (qStringTableRow("" , "" , "D"))
                << (qStringTableRow("" , "" , "B"))
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")));
    }

    {
        QTest::newRow("2x 1x2 dropped at [0, 0] (duplicates)")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection; 2x the same row (to simulate selections in hierarchy)
                << Position(0, 0) << Position(0, 1)
                << Position(0, 0) << Position(0, 1))
            << Position(0, 0) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("A", "B", "" ))
                << (qStringTableRow("A", "" , "" ))
                << (qStringTableRow("" , "B", "" )) // ### FIXME: strange behavior, but rare case
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")));
    }

    {
        QTest::newRow("2x 1x2 dropped at [3, 2] (duplicates)")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection; 2x the same row (to simulate selections in hierarchy)
                << Position(0, 0) << Position(0, 1)
                << Position(0, 0) << Position(0, 1))
            << Position(3, 2) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5"))
                << (qStringTableRow("" , "" , "A"))
                << (qStringTableRow("" , "" , "B"))
                << (qStringTableRow("" , "" , "A"))
                << (qStringTableRow("" , "" , "B")));
    }
    {
        QTest::newRow("2x 1x2 dropped at [3, 2] (different rows)")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F"))
                << (qStringTableRow("G", "H", "I")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection; 2x the same row (to simulate selections in hierarchy)
                << Position(0, 0) << Position(0, 1)
                << Position(2, 0) << Position(2, 1))
            << Position(2, 1) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5"))
                << (qStringTableRow("" , "A" , "B"))
                << (qStringTableRow("" , "G" , "H")));
    }

    {
        QTest::newRow("2x 1x2 dropped at [3, 2] (different rows, over the edge)")
            << (STRINGTABLE // source table
                << (qStringTableRow("A", "B", "C"))
                << (qStringTableRow("D", "E", "F"))
                << (qStringTableRow("G", "H", "I")))
            << (STRINGTABLE // destination table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5")))
            << (Selection() // selection; 2x the same row (to simulate selections in hierarchy)
                << Position(0, 0) << Position(0, 1)
                << Position(2, 0) << Position(2, 1))
            << Position(3, 2) // drop position
            << (STRINGTABLE // resulting table
                << (qStringTableRow("0", "1", "2"))
                << (qStringTableRow("3", "4", "5"))
                << (qStringTableRow("" , "" , "A"))
                << (qStringTableRow("" , "" , "G"))
                << (qStringTableRow("" , "" , "B"))
                << (qStringTableRow("" , "" , "H")));
    }
}

void tst_QAbstractItemModel::dropMimeData()
{
    QFETCH(StringTable, src_table);
    QFETCH(StringTable, dst_table);
    QFETCH(Selection, selection);
    QFETCH(Position, dst_position);
    QFETCH(StringTable, res_table);

    QtTestModel src(src_table);
    QtTestModel dst(dst_table);
    QtTestModel res(res_table);

//     qDebug() << "src" << src.rowCount(QModelIndex()) << src.columnCount(QModelIndex());
//     qDebug() << "dst" << dst.rowCount(QModelIndex()) << dst.columnCount(QModelIndex());
//     qDebug() << "res" << res.rowCount(QModelIndex()) << res.columnCount(QModelIndex());

    // get the mimeData from the "selected" indexes
    QModelIndexList selectedIndexes;
    for (int i = 0; i < selection.count(); ++i)
        selectedIndexes << src.index(selection.at(i).first, selection.at(i).second, QModelIndex());
    QMimeData *md = src.mimeData(selectedIndexes);
    // do the drop
    dst.dropMimeData(md, Qt::CopyAction, dst_position.first, dst_position.second, QModelIndex());
    delete md;

    // compare to the expected results
    QCOMPARE(dst.rowCount(QModelIndex()), res.rowCount(QModelIndex()));
    QCOMPARE(dst.columnCount(QModelIndex()), res.columnCount(QModelIndex()));
    for (int r = 0; r < dst.rowCount(QModelIndex()); ++r) {
        for (int c = 0; c < dst.columnCount(QModelIndex()); ++c) {
            QModelIndex dst_idx = dst.index(r, c, QModelIndex());
            QModelIndex res_idx = res.index(r, c, QModelIndex());
            QMap<int, QVariant> dst_data = dst.itemData(dst_idx);
            QMap<int, QVariant> res_data = res.itemData(res_idx);
            //if(dst_data != res_data)
            //    qDebug() << r << c << dst_data.value(0).toString() << res_data.value(0).toString();
            QCOMPARE(dst_data , res_data);
        }
    }

}


void tst_QAbstractItemModel::changePersistentIndex()
{
    QtTestModel model(3, 3);
    QModelIndex a = model.index(1, 2, QModelIndex());
    QModelIndex b = model.index(2, 1, QModelIndex());
    QPersistentModelIndex p(a);
    QVERIFY(p == a);
    model.setPersistent(a, b);
    QVERIFY(p == b);
}

void tst_QAbstractItemModel::movePersistentIndex()
{
    QtTestModel model(3, 3);

    QPersistentModelIndex a = model.index(1, 1);
    QVERIFY(a.isValid());
    QCOMPARE(a.row(), 1);
    QCOMPARE(a.column(), 1);

    model.insertRow(0);
    QCOMPARE(a.row(), 2);

    model.insertRow(1);
    QCOMPARE(a.row(), 3);

    model.insertColumn(0);
    QCOMPARE(a.column(), 2);
}

void tst_QAbstractItemModel::removeRows()
{
    QtTestModel model(10, 10);

    QSignalSpy rowsAboutToBeRemovedSpy(&model, SIGNAL(rowsAboutToBeRemoved( const QModelIndex &, int , int )));
    QSignalSpy rowsRemovedSpy(&model, SIGNAL(rowsRemoved( const QModelIndex &, int, int )));

    QCOMPARE(model.removeRows(6, 4), true);
    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 1);
}

void tst_QAbstractItemModel::removeColumns()
{
    QtTestModel model(10, 10);

    QSignalSpy columnsAboutToBeRemovedSpy(&model, SIGNAL(columnsAboutToBeRemoved( const QModelIndex &, int , int )));
    QSignalSpy columnsRemovedSpy(&model, SIGNAL(columnsRemoved( const QModelIndex &, int, int )));

    QCOMPARE(model.removeColumns(6, 4), true);
    QCOMPARE(columnsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(columnsRemovedSpy.count(), 1);
}

void tst_QAbstractItemModel::insertRows()
{
    QtTestModel model(10, 10);

    QSignalSpy rowsAboutToBeInsertedSpy(&model, SIGNAL(rowsAboutToBeInserted( const QModelIndex &, int , int )));
    QSignalSpy rowsInsertedSpy(&model, SIGNAL(rowsInserted( const QModelIndex &, int, int )));

    QCOMPARE(model.insertRows(6, 4), true);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 1);
}

void tst_QAbstractItemModel::insertColumns()
{
    QtTestModel model(10, 10);

    QSignalSpy columnsAboutToBeInsertedSpy(&model, SIGNAL(columnsAboutToBeInserted( const QModelIndex &, int , int )));
    QSignalSpy columnsInsertedSpy(&model, SIGNAL(columnsInserted( const QModelIndex &, int, int )));

    QCOMPARE(model.insertColumns(6, 4), true);
    QCOMPARE(columnsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(columnsInsertedSpy.count(), 1);
}

void tst_QAbstractItemModel::reset()
{
    QtTestModel model(10, 10);

    QSignalSpy resetSpy(&model, SIGNAL(modelReset()));
    model.reset();
    QCOMPARE(resetSpy.count(), 1);
}

void tst_QAbstractItemModel::complexChangesWithPersistent()
{
    QtTestModel model(10, 10);
    QPersistentModelIndex a = model.index(1, 1, QModelIndex());
    QPersistentModelIndex b = model.index(9, 7, QModelIndex());
    QPersistentModelIndex c = model.index(5, 6, QModelIndex());
    QPersistentModelIndex d = model.index(3, 9, QModelIndex());
    QPersistentModelIndex e[10];
    for (int i=0; i <10 ; i++) {
        e[i] = model.index(2, i , QModelIndex());
    }

    QVERIFY(a == model.index(1, 1, QModelIndex()));
    QVERIFY(b == model.index(9, 7, QModelIndex()));
    QVERIFY(c == model.index(5, 6, QModelIndex()));
    QVERIFY(d == model.index(3, 9, QModelIndex()));
    for (int i=0; i <8 ; i++)
        QVERIFY(e[i] == model.index(2, i , QModelIndex()));

    //remove a bunch of columns
    model.removeColumns(2, 4);
    
    QVERIFY(a == model.index(1, 1, QModelIndex()));
    QVERIFY(b == model.index(9, 3, QModelIndex()));
    QVERIFY(c == model.index(5, 2, QModelIndex()));
    QVERIFY(d == model.index(3, 5, QModelIndex()));
    for (int i=0; i <2 ; i++)
        QVERIFY(e[i] == model.index(2, i , QModelIndex()));
    for (int i=2; i <6 ; i++)
        QVERIFY(!e[i].isValid());
    for (int i=6; i <10 ; i++)
        QVERIFY(e[i] == model.index(2, i-4 , QModelIndex()));
    
    //move some indexes around
    model.setPersistent(model.index(1, 1 , QModelIndex()), model.index(9, 3 , QModelIndex()));
    model.setPersistent(model.index(9, 3 , QModelIndex()), model.index(8, 4 , QModelIndex()));

    QVERIFY(a == model.index(9, 3, QModelIndex()));
    QVERIFY(b == model.index(8, 4, QModelIndex()));
    QVERIFY(c == model.index(5, 2, QModelIndex()));
    QVERIFY(d == model.index(3, 5, QModelIndex()));
    for (int i=0; i <2 ; i++)
        QVERIFY(e[i] == model.index(2, i , QModelIndex()));
    for (int i=2; i <6 ; i++)
        QVERIFY(!e[i].isValid());
    for (int i=6; i <10 ; i++)
        QVERIFY(e[i] == model.index(2, i-4 , QModelIndex()));

    //inserting a bunch of columns
    model.insertColumns(2, 2);
    QVERIFY(a == model.index(9, 5, QModelIndex()));
    QVERIFY(b == model.index(8, 6, QModelIndex()));
    QVERIFY(c == model.index(5, 4, QModelIndex()));
    QVERIFY(d == model.index(3, 7, QModelIndex()));
    for (int i=0; i <2 ; i++)
        QVERIFY(e[i] == model.index(2, i , QModelIndex()));
    for (int i=2; i <6 ; i++)
        QVERIFY(!e[i].isValid());
    for (int i=6; i <10 ; i++)
        QVERIFY(e[i] == model.index(2, i-2 , QModelIndex()));

}

QTEST_MAIN(tst_QAbstractItemModel)
#include "tst_qabstractitemmodel.moc"

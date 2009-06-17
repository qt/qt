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
#include <QtSql/QtSql>

#include "../qsqldatabase/tst_databases.h"



//TESTED_CLASS=
//TESTED_FILES=

class tst_QSqlRelationalTableModel : public QObject
{
    Q_OBJECT

public:
    void recreateTestTables(QSqlDatabase);

    tst_Databases dbs;

public slots:
    void initTestCase_data();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void data();
    void setData();
    void multipleRelation();
    void insertRecord();
    void setRecord();
    void insertWithStrategies();
    void removeColumn();
    void filter();
    void sort();
    void revert();

    void clearDisplayValuesCache();
    void insertRecordDuplicateFieldNames();
    void invalidData();
    void relationModel();
};


void tst_QSqlRelationalTableModel::initTestCase_data()
{
    dbs.open();
    if (dbs.fillTestTable() == 0) {
        qWarning("NO DATABASES");
        QSKIP("No database drivers are available in this Qt configuration", SkipAll);
    }
}

void tst_QSqlRelationalTableModel::recreateTestTables(QSqlDatabase db)
{
    QSqlQuery q(db);

    QStringList tableNames;
    tableNames << qTableName( "reltest1" )
            << qTableName( "reltest2" )
            << qTableName( "reltest3" )
            << qTableName( "reltest4" )
            << qTableName( "reltest5" );
    tst_Databases::safeDropTables( db, tableNames );

    QVERIFY_SQL( q, exec("create table " + qTableName("reltest1") +
            " (id int not null primary key, name varchar(20), title_key int, another_title_key int)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest1") + " values(1, 'harry', 1, 2)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest1") + " values(2, 'trond', 2, 1)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest1") + " values(3, 'vohi', 1, 2)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest1") + " values(4, 'boris', 2, 2)"));

    QVERIFY_SQL( q, exec("create table " + qTableName("reltest2") + " (tid int not null primary key, title varchar(20))"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest2") + " values(1, 'herr')"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest2") + " values(2, 'mister')"));

    QVERIFY_SQL( q, exec("create table " + qTableName("reltest3") + " (id int not null primary key, name varchar(20), city_key int)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest3") + " values(1, 'Gustav', 1)"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest3") + " values(2, 'Heidi', 2)"));

    QVERIFY_SQL( q, exec("create table " + qTableName("reltest4") + " (id int not null primary key, name varchar(20))"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest4") + " values(1, 'Oslo')"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest4") + " values(2, 'Trondheim')"));

    QVERIFY_SQL( q, exec("create table " + qTableName("reltest5") + " (title varchar(20) not null primary key, abbrev varchar(20))"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest5") + " values('herr', 'Hr')"));
    QVERIFY_SQL( q, exec("insert into " + qTableName("reltest5") + " values('mister', 'Mr')"));
}

void tst_QSqlRelationalTableModel::initTestCase()
{
    foreach (const QString &dbname, dbs.dbNames)
        recreateTestTables(QSqlDatabase::database(dbname));
}

void tst_QSqlRelationalTableModel::cleanupTestCase()
{
    QStringList tableNames;
    tableNames << qTableName( "reltest1" )
            << qTableName( "reltest2" )
            << qTableName( "reltest3" )
            << qTableName( "reltest4" );
    foreach (const QString &dbName, dbs.dbNames) {
        QSqlDatabase db = QSqlDatabase::database(dbName);
        tst_Databases::safeDropTables( db, tableNames );
    }
    dbs.close();
}

void tst_QSqlRelationalTableModel::init()
{
}

void tst_QSqlRelationalTableModel::cleanup()
{
}

void tst_QSqlRelationalTableModel::data()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    QVERIFY_SQL(model, select());

    QCOMPARE(model.columnCount(), 4);
    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("herr"));

    //try a non-existant index
    QVERIFY2(model.data(model.index(0,4)).isValid() == false,"Invalid index returned valid QVariant");

    //check data retrieval when relational key is a non-integer type
    //in this case a string
    QSqlRelationalTableModel model2(0,db);
    model2.setTable(qTableName("reltest2"));
    model2.setRelation(1, QSqlRelation(qTableName("reltest5"),"title","abbrev"));
    QVERIFY_SQL(model2, select());

    QCOMPARE(model2.data(model2.index(0, 1)).toString(), QString("Hr"));
    QCOMPARE(model2.data(model2.index(1, 1)).toString(), QString("Mr"));
}

void tst_QSqlRelationalTableModel::setData()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    // set the values using OnRowChange Strategy
    {
        QSqlRelationalTableModel model(0, db);

        model.setTable(qTableName("reltest1"));
        model.setSort(0, Qt::AscendingOrder);
        model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
        QVERIFY_SQL(model, select());

        QVERIFY(model.setData(model.index(0, 1), QString("harry2")));
        QVERIFY(model.setData(model.index(0, 2), 2));

        QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry2"));
        QCOMPARE(model.data(model.index(0, 2)).toString(), QString("mister"));

        model.submit();

        QVERIFY(model.setData(model.index(3,1), QString("boris2")));
        QVERIFY(model.setData(model.index(3, 2), 1));

        QCOMPARE(model.data(model.index(3,1)).toString(), QString("boris2"));
        QCOMPARE(model.data(model.index(3, 2)).toString(), QString("herr"));

        model.submit();
    }
    { //verify values
        QSqlRelationalTableModel model(0, db);
        model.setTable(qTableName("reltest1"));
        model.setSort(0, Qt::AscendingOrder);
        QVERIFY_SQL(model, select());

        QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry2"));
        QCOMPARE(model.data(model.index(0, 2)).toInt(), 2);
        QCOMPARE(model.data(model.index(3, 1)).toString(), QString("boris2"));
        QCOMPARE(model.data(model.index(3, 2)).toInt(), 1);

        model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
        QVERIFY_SQL(model, select());
        QCOMPARE(model.data(model.index(0, 2)).toString(), QString("mister"));
        QCOMPARE(model.data(model.index(3,2)).toString(), QString("herr"));

    }

    //set the values using OnFieldChange strategy
    {
        QSqlRelationalTableModel model(0, db);
        model.setTable(qTableName("reltest1"));
        model.setEditStrategy(QSqlTableModel::OnFieldChange);
        model.setSort(0, Qt::AscendingOrder);
        model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
        QVERIFY_SQL(model, select());

        QVERIFY(model.setData(model.index(1,1), QString("trond2")));
        QVERIFY(model.setData(model.index(2,2), 2));

        QCOMPARE(model.data(model.index(1,1)).toString(), QString("trond2"));
        QCOMPARE(model.data(model.index(2,2)).toString(), QString("mister"));
    }
    { //verify values
        QSqlRelationalTableModel model(0, db);
        model.setTable(qTableName("reltest1"));
        model.setSort(0, Qt::AscendingOrder);
        QVERIFY_SQL(model, select());

        QCOMPARE(model.data(model.index(1, 1)).toString(), QString("trond2"));
        QCOMPARE(model.data(model.index(2, 2)).toInt(), 2);

        model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
        QVERIFY_SQL(model, select());
        QCOMPARE(model.data(model.index(2, 2)).toString(), QString("mister"));
    }

    //set values using OnManualSubmit strategy
    {
        QSqlRelationalTableModel model(0, db);

        model.setTable(qTableName("reltest1"));
        model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
        model.setRelation(3, QSqlRelation(qTableName("reltest2"), "tid", "title"));
        model.setEditStrategy(QSqlTableModel::OnManualSubmit);
        model.setSort(0, Qt::AscendingOrder);
        QVERIFY_SQL(model, select());

        QVERIFY(model.setData(model.index(2, 1), QString("vohi2")));
        QVERIFY(model.setData(model.index(3, 2), 1));
        QVERIFY(model.setData(model.index(0, 3), 1));

        QCOMPARE(model.data(model.index(2, 1)).toString(), QString("vohi2"));
        QCOMPARE(model.data(model.index(3, 2)).toString(), QString("herr"));
        QCOMPARE(model.data(model.index(0, 3)).toString(), QString("herr"));

        QVERIFY_SQL(model, submitAll());
    }
    { //verify values
        QSqlRelationalTableModel model(0, db);
        model.setTable(qTableName("reltest1"));
        model.setSort(0, Qt::AscendingOrder);
        QVERIFY_SQL(model, select());

        QCOMPARE(model.data(model.index(2, 1)).toString(), QString("vohi2"));
        QCOMPARE(model.data(model.index(3, 2)).toInt(), 1);
        QCOMPARE(model.data(model.index(0, 3)).toInt(), 1);

        model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
        model.setRelation(3, QSqlRelation(qTableName("reltest2"), "tid", "title"));
        QVERIFY_SQL(model, select());
        QCOMPARE(model.data(model.index(3, 2)).toString(), QString("herr"));
        QCOMPARE(model.data(model.index(0, 3)).toString(), QString("herr"));
    }

    //check setting of data when the relational key is a non-integer type
    //in this case a string.
    {
        QSqlRelationalTableModel model(0, db);

        model.setTable(qTableName("reltest2"));
        model.setRelation(1, QSqlRelation(qTableName("reltest5"), "title", "abbrev"));
        model.setEditStrategy(QSqlTableModel::OnManualSubmit);
        QVERIFY_SQL(model, select());

        QCOMPARE(model.data(model.index(0,1)).toString(), QString("Hr"));
        QVERIFY(model.setData(model.index(0,1), QString("mister")));
        QCOMPARE(model.data(model.index(0,1)).toString(), QString("Mr"));
        QVERIFY_SQL(model, submitAll());

        QCOMPARE(model.data(model.index(0,1)).toString(), QString("Mr"));
    }

}

void tst_QSqlRelationalTableModel::multipleRelation()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);
    recreateTestTables(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    model.setRelation(3, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    QVERIFY_SQL(model, select());

    QCOMPARE(model.data(model.index(2, 0)).toInt(), 3);

    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(0, 3)).toString(), QString("mister"));
}

void tst_QSqlRelationalTableModel::insertRecord()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    QVERIFY_SQL(model, select());

    QSqlRecord rec;
    QSqlField f1("id", QVariant::Int);
    QSqlField f2("name", QVariant::String);
    QSqlField f3("title_key", QVariant::Int);
    QSqlField f4("another_title_key", QVariant::Int);

    f1.setValue(5);
    f2.setValue("test");
    f3.setValue(1);
    f4.setValue(2);

    f1.setGenerated(true);
    f2.setGenerated(true);
    f3.setGenerated(true);
    f4.setGenerated(true);

    rec.append(f1);
    rec.append(f2);
    rec.append(f3);
    rec.append(f4);

    QVERIFY_SQL(model, insertRecord(-1, rec));

    QCOMPARE(model.data(model.index(4, 0)).toInt(), 5);
    QCOMPARE(model.data(model.index(4, 1)).toString(), QString("test"));
    QCOMPARE(model.data(model.index(4, 2)).toString(), QString("herr"));
}

void tst_QSqlRelationalTableModel::setRecord()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);
    recreateTestTables(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    QVERIFY_SQL(model, select());

    QSqlRecord rec;
    QSqlField f1("id", QVariant::Int);
    QSqlField f2("name", QVariant::String);
    QSqlField f3("title_key", QVariant::Int);
    QSqlField f4("another_title_key", QVariant::Int);

    f1.setValue(5);
    f2.setValue("tester");
    f3.setValue(1);
    f4.setValue(2);

    f1.setGenerated(true);
    f2.setGenerated(true);
    f3.setGenerated(true);
    f4.setGenerated(true);

    rec.append(f1);
    rec.append(f2);
    rec.append(f3);
    rec.append(f4);

    QCOMPARE(model.data(model.index(1, 0)).toInt(), 2);
    QCOMPARE(model.data(model.index(1, 1)).toString(), QString("trond"));
    QCOMPARE(model.data(model.index(1, 2)).toString(), QString("mister"));

    QVERIFY_SQL(model, setRecord(1, rec));

    QCOMPARE(model.data(model.index(1, 0)).toInt(), 5);
    QCOMPARE(model.data(model.index(1, 1)).toString(), QString("tester"));
    QCOMPARE(model.data(model.index(1, 2)).toString(), QString("herr"));

    model.setSort(0, Qt::AscendingOrder);
    QVERIFY_SQL(model, submit());

    QCOMPARE(model.data(model.index(3, 0)).toInt(), 5);
    QCOMPARE(model.data(model.index(3, 1)).toString(), QString("tester"));
    QCOMPARE(model.data(model.index(3, 2)).toString(), QString("herr"));

}

void tst_QSqlRelationalTableModel::insertWithStrategies()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    model.setRelation(3, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    QVERIFY_SQL(model, select());

    QCOMPARE(model.data(model.index(0,0)).toInt(), 1);
    QCOMPARE(model.data(model.index(0,1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(0,2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(0,3)).toString(), QString("mister"));

    model.insertRows(0, 1);
    model.setData(model.index(0, 0), 1011);
    model.setData(model.index(0, 1), "test");
    model.setData(model.index(0, 2), 2);
    model.setData(model.index(0, 3), 1);

    QCOMPARE(model.data(model.index(0,0)).toInt(), 1011);
    QCOMPARE(model.data(model.index(0,1)).toString(), QString("test"));
    QCOMPARE(model.data(model.index(0,2)).toString(), QString("mister"));
    QCOMPARE(model.data(model.index(0,3)).toString(), QString("herr"));

    QCOMPARE(model.data(model.index(1,0)).toInt(), 1);
    QCOMPARE(model.data(model.index(1,1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(1,2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(1,3)).toString(), QString("mister"));

    QVERIFY_SQL(model, submitAll());

    model.setEditStrategy(QSqlTableModel::OnManualSubmit);

    QCOMPARE(model.data(model.index(0,0)).toInt(), 1);
    QCOMPARE(model.data(model.index(0,1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(0,2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(0,3)).toString(), QString("mister"));
    model.setData(model.index(0,3),1);
    QCOMPARE(model.data(model.index(0,3)).toString(), QString("herr"));

    model.insertRows(0, 2);
    model.setData(model.index(0, 0), 1012);
    model.setData(model.index(0, 1), "george");
    model.setData(model.index(0, 2), 2);
    model.setData(model.index(0, 3), 2);

    model.setData(model.index(1, 0), 1013);
    model.setData(model.index(1, 1), "kramer");
    model.setData(model.index(1, 2), 2);
    model.setData(model.index(1, 3), 1);

    QCOMPARE(model.data(model.index(0,0)).toInt(),1012);
    QCOMPARE(model.data(model.index(0,1)).toString(), QString("george"));
    QCOMPARE(model.data(model.index(0,2)).toString(), QString("mister"));
    QCOMPARE(model.data(model.index(0,3)).toString(), QString("mister"));

    QCOMPARE(model.data(model.index(1,0)).toInt(),1013);
    QCOMPARE(model.data(model.index(1,1)).toString(), QString("kramer"));
    QCOMPARE(model.data(model.index(1,2)).toString(), QString("mister"));
    QCOMPARE(model.data(model.index(1,3)).toString(), QString("herr"));

    QCOMPARE(model.data(model.index(2,0)).toInt(), 1);
    QCOMPARE(model.data(model.index(2,1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(2,2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(2,3)).toString(), QString("herr"));

    QVERIFY_SQL(model, submitAll());
}

void tst_QSqlRelationalTableModel::removeColumn()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);
    recreateTestTables(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    QVERIFY_SQL(model, select());

    QVERIFY_SQL(model, removeColumn(3));
    QVERIFY_SQL(model, select());

    QCOMPARE(model.columnCount(), 3);

    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("harry"));
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(0, 3)), QVariant());

    // try removing more than one column
    QVERIFY_SQL(model, removeColumns(1, 2));
    QCOMPARE(model.columnCount(), 1);
    QCOMPARE(model.data(model.index(0, 0)).toInt(), 1);
    QCOMPARE(model.data(model.index(0, 1)), QVariant());

}

void tst_QSqlRelationalTableModel::filter()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);
    recreateTestTables(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    model.setFilter("title = 'herr'");

    QVERIFY_SQL(model, select());
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(1, 2)).toString(), QString("herr"));
}

void tst_QSqlRelationalTableModel::sort()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    model.setRelation(3, QSqlRelation(qTableName("reltest2"), "tid", "title"));

    model.setSort(2, Qt::DescendingOrder);
    QVERIFY_SQL(model, select());

    QCOMPARE(model.rowCount(), 4);
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("mister"));
    QCOMPARE(model.data(model.index(1, 2)).toString(), QString("mister"));
    QCOMPARE(model.data(model.index(2, 2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(3, 2)).toString(), QString("herr"));


    model.setSort(3, Qt::AscendingOrder);
    QVERIFY_SQL(model, select());

    QCOMPARE(model.rowCount(), 4);
    QCOMPARE(model.data(model.index(0, 3)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(1, 3)).toString(), QString("mister"));
    QCOMPARE(model.data(model.index(2, 3)).toString(), QString("mister"));
    QCOMPARE(model.data(model.index(3, 3)).toString(), QString("mister"));
}

static void testRevert(QSqlRelationalTableModel &model)
{
    /* revert single row */
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("herr"));
    QVERIFY(model.setData(model.index(0, 2), 2, Qt::EditRole));

    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("mister"));
    model.revertRow(0);
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("herr"));

    /* revert all */
    QVERIFY(model.setData(model.index(0, 2), 2, Qt::EditRole));

    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("mister"));
    model.revertAll();
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("herr"));

    // the following only works for OnManualSubmit
    if (model.editStrategy() != QSqlTableModel::OnManualSubmit)
        return;

    /* revert inserted rows */
    int initialRowCount = model.rowCount();
    QVERIFY(model.insertRows(4, 4));
    QVERIFY(model.rowCount() == (initialRowCount + 4));

    /* make sure the new rows are initialized to nothing */
    QVERIFY(model.data(model.index(4, 2)).toString().isEmpty());
    QVERIFY(model.data(model.index(5, 2)).toString().isEmpty());
    QVERIFY(model.data(model.index(6, 2)).toString().isEmpty());
    QVERIFY(model.data(model.index(7, 2)).toString().isEmpty());

    /* Set some values */
    QVERIFY(model.setData(model.index(4, 0), 42, Qt::EditRole));
    QVERIFY(model.setData(model.index(5, 0), 43, Qt::EditRole));
    QVERIFY(model.setData(model.index(6, 0), 44, Qt::EditRole));
    QVERIFY(model.setData(model.index(7, 0), 45, Qt::EditRole));

    QVERIFY(model.setData(model.index(4, 2), 2, Qt::EditRole));
    QVERIFY(model.setData(model.index(5, 2), 2, Qt::EditRole));
    QVERIFY(model.setData(model.index(6, 2), 1, Qt::EditRole));
    QVERIFY(model.setData(model.index(7, 2), 2, Qt::EditRole));

    /* Now revert the newly inserted rows */
    model.revertAll();
    QVERIFY(model.rowCount() == initialRowCount);

    /* Insert rows again */
    QVERIFY(model.insertRows(4, 4));

    /* make sure the new rows are initialized to nothing */
    QVERIFY(model.data(model.index(4, 2)).toString().isEmpty());
    QVERIFY(model.data(model.index(5, 2)).toString().isEmpty());
    QVERIFY(model.data(model.index(6, 2)).toString().isEmpty());
    QVERIFY(model.data(model.index(7, 2)).toString().isEmpty());
}

void tst_QSqlRelationalTableModel::revert()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    model.setRelation(3, QSqlRelation(qTableName("reltest2"), "tid", "title"));

    model.setSort(0, Qt::AscendingOrder);

    QVERIFY_SQL(model, select());
    QCOMPARE(model.data(model.index(0, 0)).toString(), QString("1"));

    testRevert(model);
    if (QTest::currentTestFailed())
        return;

    /* and again with OnManualSubmit */
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    testRevert(model);
}

void tst_QSqlRelationalTableModel::clearDisplayValuesCache()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlRelationalTableModel model(0, db);

    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    model.setRelation(3, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    model.setSort(1, Qt::AscendingOrder);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);

    QVERIFY_SQL(model, select());

    QCOMPARE(model.data(model.index(3, 0)).toInt(), 3);
    QCOMPARE(model.data(model.index(3, 1)).toString(), QString("vohi"));
    QCOMPARE(model.data(model.index(3, 2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(3, 3)).toString(), QString("mister"));

    model.insertRow(model.rowCount());
    QVERIFY(model.setData(model.index(4, 0), 5, Qt::EditRole));
    QVERIFY(model.setData(model.index(4, 1), "anders", Qt::EditRole));
    QVERIFY(model.setData(model.index(4, 2), 1, Qt::EditRole));
    QVERIFY(model.setData(model.index(4, 3), 1, Qt::EditRole));
    model.submitAll();

    QCOMPARE(model.data(model.index(0, 0)).toInt(), 5);
    QCOMPARE(model.data(model.index(0, 1)).toString(), QString("anders"));
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(0, 3)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(4, 0)).toInt(), 3);
    QCOMPARE(model.data(model.index(4, 1)).toString(), QString("vohi"));
    QCOMPARE(model.data(model.index(4, 2)).toString(), QString("herr"));
    QCOMPARE(model.data(model.index(4, 3)).toString(), QString("mister"));
}

// For task 140782 and 176374: If the main table and the the related tables uses the same
// name for a column or display column then insertRecord() would return true though it
// actually failed.
void tst_QSqlRelationalTableModel::insertRecordDuplicateFieldNames()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlRelationalTableModel model(0, db);
    model.setTable(qTableName("reltest3"));
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);

    // Duplication of "name", used in both reltest3 and reltest4.
    model.setRelation(2, QSqlRelation(qTableName("reltest4"), "id", "name"));
    QVERIFY_SQL(model, select());

    QCOMPARE(model.record(1).value(qTableName("reltest4").append(QLatin1String("_name"))).toString(),
            QString("Trondheim"));

    QSqlRecord rec = model.record();
    rec.setValue(0, 3);
    rec.setValue(1, "Berge");
    rec.setValue(2, 1); // Must insert the key value

    QCOMPARE(rec.fieldName(0), QLatin1String("id"));
    QCOMPARE(rec.fieldName(1), QLatin1String("name")); // This comes from main table

    // The duplicate field names is aliased because it's comes from the relation's display column.
    if(!db.driverName().startsWith("QIBASE"))
        QCOMPARE(rec.fieldName(2), qTableName("reltest4").append(QLatin1String("_name")));
    else
        QCOMPARE(rec.fieldName(2), (qTableName("reltest4").append(QLatin1String("_name"))).toUpper());

    QVERIFY(model.insertRecord(-1, rec));
    QCOMPARE(model.data(model.index(2, 2)).toString(), QString("Oslo"));
    QVERIFY(model.submitAll());
    QCOMPARE(model.data(model.index(2, 2)).toString(), QString("Oslo"));
}

void tst_QSqlRelationalTableModel::invalidData()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlRelationalTableModel model(0, db);
    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    QVERIFY_SQL(model, select());

    //try set a non-existent relational key
    QVERIFY(model.setData(model.index(0, 2), 3) == false);
    QCOMPARE(model.data(model.index(0, 2)).toString(), QString("herr"));

    //try to set data in non valid index
    QVERIFY(model.setData(model.index(0,10),5) == false);
}

void tst_QSqlRelationalTableModel::relationModel()
{
    QFETCH_GLOBAL(QString, dbName);
    QSqlDatabase db = QSqlDatabase::database(dbName);
    CHECK_DATABASE(db);

    QSqlRelationalTableModel model(0, db);
    model.setTable(qTableName("reltest1"));
    model.setRelation(2, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    QVERIFY_SQL(model, select());

    QVERIFY(model.relationModel(0) == NULL);
    QVERIFY(model.relationModel(1) == NULL);
    QVERIFY(model.relationModel(2) != NULL);
    QVERIFY(model.relationModel(3) == NULL);
    QVERIFY(model.relationModel(4) == NULL);

    model.setRelation(3, QSqlRelation(qTableName("reltest2"), "tid", "title"));
    QVERIFY_SQL(model, select());

    QVERIFY(model.relationModel(0) == NULL);
    QVERIFY(model.relationModel(1) == NULL);
    QVERIFY(model.relationModel(2) != NULL);
    QVERIFY(model.relationModel(3) != NULL);
    QVERIFY(model.relationModel(4) == NULL);

    QSqlTableModel *rel_model = model.relationModel(2);
    QCOMPARE(rel_model->data(rel_model->index(0,1)).toString(), QString("herr"));
}

QTEST_MAIN(tst_QSqlRelationalTableModel)
#include "tst_qsqlrelationaltablemodel.moc"

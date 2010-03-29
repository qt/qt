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


#include <QtTest/QtTest>
#include <q3sqlcursor.h>
#include <qsqlfield.h>
#include <qsqldriver.h>
#include <QSet>

#include "../qsqldatabase/tst_databases.h"

const QString qtest(qTableName( "qtest", __FILE__ ));

//TESTED_FILES=

QT_FORWARD_DECLARE_CLASS(QSqlDatabase)

class tst_Q3SqlCursor : public QObject
{
Q_OBJECT

public:
    tst_Q3SqlCursor();
    virtual ~tst_Q3SqlCursor();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void copyConstructor_data() { generic_data(); }
    void copyConstructor();

    void value_data() { generic_data(); }
    void value();
    void primaryIndex_data() { generic_data(); }
    void primaryIndex();
    void insert_data() { generic_data(); }
    void insert();
    void select_data() { generic_data(); }
    void select();
    void setFilter_data() { generic_data(); }
    void setFilter();
    void setName_data() { generic_data(); }
    void setName();

    // problem specific tests
    void unicode_data() { generic_data(); }
    void unicode();
    void precision_data() { generic_data(); }
    void precision();
    void insertORA_data() { generic_data("QOCI"); }
    void insertORA();
    void batchInsert_data() { generic_data(); }
    void batchInsert();
    void insertSpecial_data() { generic_data(); }
    void insertSpecial();
    void updateNoPK_data() { generic_data(); }
    void updateNoPK();
    void insertFieldNameContainsWS_data() { generic_data(); }
    void insertFieldNameContainsWS(); // For task 117996

private:
    void generic_data(const QString &engine=QString());
    void createTestTables( QSqlDatabase db );
    void dropTestTables( QSqlDatabase db );
    void populateTestTables( QSqlDatabase db );

    tst_Databases dbs;
};

tst_Q3SqlCursor::tst_Q3SqlCursor()
{
}

tst_Q3SqlCursor::~tst_Q3SqlCursor()
{
}

void tst_Q3SqlCursor::generic_data(const QString &engine)
{
    if ( dbs.fillTestTable(engine) == 0 ) {
        if(engine.isEmpty())
           QSKIP( "No database drivers are available in this Qt configuration", SkipAll );
        else
           QSKIP( (QString("No database drivers of type %1 are available in this Qt configuration").arg(engine)).toLocal8Bit(), SkipAll );
    }
}

void tst_Q3SqlCursor::createTestTables( QSqlDatabase db )
{
    if ( !db.isValid() )
        return;
    QSqlQuery q( db );

    if (tst_Databases::isSqlServer(db)) {
        QVERIFY_SQL(q, exec("SET ANSI_DEFAULTS ON"));
        QVERIFY_SQL(q, exec("SET IMPLICIT_TRANSACTIONS OFF"));
    }
    else if(tst_Databases::isPostgreSQL(db))
        QVERIFY_SQL( q, exec("set client_min_messages='warning'"));

    // please never ever change this table; otherwise fix all tests ;)
    if ( tst_Databases::isMSAccess( db ) ) {
        QVERIFY_SQL(q, exec( "create table " + qtest + " ( id int not null, t_varchar varchar(40) not null,"
                         "t_char char(40), t_numeric number, primary key (id, t_varchar) )" ));
    } else {
        QVERIFY_SQL(q, exec( "create table " + qtest + " ( id int not null, t_varchar varchar(40) not null,"
                         "t_char char(40), t_numeric numeric(6, 3), primary key (id, t_varchar) )" ));
    }

    if ( tst_Databases::isSqlServer( db ) ) {
        //workaround for SQL SERVER since he can store unicode only in nvarchar fields
        QVERIFY_SQL(q, exec("create table " + qTableName("qtest_unicode", __FILE__) + " (id int not null, "
                       "t_varchar nvarchar(80) not null, t_char nchar(80) )" ));
    } else {
        QVERIFY_SQL(q, exec("create table " + qTableName("qtest_unicode", __FILE__) + " (id int not null, "
                       "t_varchar varchar(100) not null," "t_char char(100))" ));
    }

    if (tst_Databases::isMSAccess(db)) {
        QVERIFY_SQL(q, exec("create table " + qTableName("qtest_precision", __FILE__) + " (col1 number)"));
    } else {
        QVERIFY_SQL(q, exec("create table " + qTableName("qtest_precision", __FILE__) + " (col1 numeric(15, 14))"));
    }
}

void tst_Q3SqlCursor::dropTestTables( QSqlDatabase db )
{
    if ( !db.isValid() )
        return;
    QStringList tableNames;
    tableNames << qtest
            << qTableName( "qtest_unicode", __FILE__ )
            << qTableName( "qtest_precision", __FILE__ )
            << qTableName( "qtest_ovchar", __FILE__ )
            << qTableName( "qtest_onvchar", __FILE__ )
            << qTableName( "qtestPK", __FILE__ );
    tst_Databases::safeDropTables( db, tableNames );
}

void tst_Q3SqlCursor::populateTestTables( QSqlDatabase db )
{
    if (!db.isValid())
        return;
    QSqlQuery q( db );

    q.exec( "delete from " + qtest ); //not fatal
    QVERIFY_SQL(q, prepare("insert into " + qtest + " (id, t_varchar, t_char, t_numeric) values (?, ?, ?, ?)"));
    q.addBindValue(QVariantList() << 0 << 1 << 2 << 3);
    q.addBindValue(QVariantList() << "VarChar0" << "VarChar1" << "VarChar2" << "VarChar3");
    q.addBindValue(QVariantList() << "Char0" << "Char1" << "Char2" << "Char3");
    q.addBindValue(QVariantList() << 1.1 << 2.2 << 3.3 << 4.4);
    QVERIFY_SQL(q, execBatch());
}

void tst_Q3SqlCursor::initTestCase()
{
    dbs.open();

    for ( QStringList::ConstIterator it = dbs.dbNames.begin(); it != dbs.dbNames.end(); ++it ) {
        QSqlDatabase db = QSqlDatabase::database( (*it) );
        CHECK_DATABASE( db );

        dropTestTables( db ); //in case of leftovers
        createTestTables( db );
        populateTestTables( db );
    }
}

void tst_Q3SqlCursor::cleanupTestCase()
{
    for ( QStringList::ConstIterator it = dbs.dbNames.begin(); it != dbs.dbNames.end(); ++it ) {
        QSqlDatabase db = QSqlDatabase::database( (*it) );
        CHECK_DATABASE( db );
        dropTestTables( db );
    }

    dbs.close();
}

void tst_Q3SqlCursor::init()
{
}

void tst_Q3SqlCursor::cleanup()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );
    if ( QTest::currentTestFailed() ) {
        //since Oracle ODBC totally craps out on error, we init again
        db.close();
        db.open();
    }
}

void tst_Q3SqlCursor::copyConstructor()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    Q3SqlCursor cur2;
    {
        Q3SqlCursor cur( qtest, true, db );
        QVERIFY_SQL(cur, select( cur.index( QString("id") ) ));
        cur2 = Q3SqlCursor( cur );
        // let "cur" run out of scope...
    }

    QSqlRecord* rec = cur2.primeUpdate();
    Q_ASSERT( rec );
    QCOMPARE( (int)rec->count(), 4 );

    int i = 0;
    while ( cur2.next() ) {
        QVERIFY( cur2.value("id").toInt() == i );
        i++;
    }
}

void tst_Q3SqlCursor::value()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    Q3SqlCursor cur( qtest, true, db );
    QVERIFY_SQL(cur, select( cur.index( QString("id") ) ));
    int i = 0;
    while ( cur.next() ) {
        QCOMPARE(cur.value("id").toInt(), i);
        i++;
    }
}

void tst_Q3SqlCursor::primaryIndex()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    Q3SqlCursor cur( qtest, true, db );
    QSqlIndex index = cur.primaryIndex();
    if ( tst_Databases::isMSAccess( db ) ) {
        QCOMPARE( index.fieldName(1).upper(), QString( "ID" ) );
        QCOMPARE( index.fieldName(0).upper(), QString( "T_VARCHAR" ) );
    } else {
        QCOMPARE( index.fieldName(0).upper(), QString( "ID" ) );
        QCOMPARE( index.fieldName(1).upper(), QString( "T_VARCHAR" ) );
    }
    QVERIFY(!index.isDescending(0));
    QVERIFY(!index.isDescending(1));
}

void tst_Q3SqlCursor::insert()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    Q3SqlCursor cur( qtest, true, db );
    QSqlRecord* irec = cur.primeInsert();
    QVERIFY( irec != 0 );

    // check that primeInsert returns a valid QSqlRecord
    QCOMPARE( (int)irec->count(), 4 );
    if ( ( irec->field( 0 ).type() != QVariant::Int ) &&
         ( irec->field( 0 ).type() != QVariant::String ) &&
         ( irec->field( 0 ).type() != QVariant::Double ) ) {
        QFAIL( QString( "Wrong datatype %1 for field 'ID'"
            " (expected Int or String)" ).arg( QVariant::typeToName( irec->field( 0 ).type() ) ) );
    }
    QCOMPARE( QVariant::typeToName( irec->field( 1 ).type() ), QVariant::typeToName( QVariant::String ) );
    QCOMPARE( QVariant::typeToName( irec->field( 2 ).type() ), QVariant::typeToName( QVariant::String ) );
    QVERIFY((QVariant::typeToName(irec->field(3).type()) == QVariant::typeToName(QVariant::Double)) ||
            (QVariant::typeToName(irec->field(3).type()) == QVariant::typeToName(QVariant::String)));
    QCOMPARE( irec->field( 0 ).name().upper(), QString( "ID" ) );
    QCOMPARE( irec->field( 1 ).name().upper(), QString( "T_VARCHAR" ) );
    QCOMPARE( irec->field( 2 ).name().upper(), QString( "T_CHAR" ) );
    QCOMPARE( irec->field( 3 ).name().upper(), QString( "T_NUMERIC" ) );

    irec->setValue( "id", 400 );
    irec->setValue( "t_varchar", "SomeVarChar" );
    irec->setValue( "t_char", "SomeChar" );
    irec->setValue( "t_numeric", 400.400 );

    QSet<int> validReturns(QSet<int>() << -1 << 1);

    QVERIFY( validReturns.contains(cur.insert()) );

    // restore old test-tables
    populateTestTables( db );
}

void tst_Q3SqlCursor::insertSpecial()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );
    QSet<int> validReturns(QSet<int>() << -1 << 1);

    Q3SqlCursor cur( qtest, true, db );
    QSqlRecord* irec = cur.primeInsert();
    QVERIFY( irec != 0 );

    QStringList strings;
    strings << "StringWith'ATick" << "StringWith\"Doublequote" << "StringWith\\Backslash" << "StringWith~Tilde";
    strings << "StringWith%Percent" << "StringWith_Underscore" << "StringWith[SquareBracket" << "StringWith{Brace";
    strings << "StringWith''DoubleTick" << "StringWith\\Lot\\of\\Backslash" << "StringWith\"lot\"of\"quotes\"";
    strings << "'StartsAndEndsWithTick'" << "\"StartsAndEndsWithQuote\"";
    strings << "StringWith\nCR" << "StringWith\n\rCRLF";

    int i = 800;

    // INSERT the strings
    QStringList::Iterator it;
    for ( it = strings.begin(); it != strings.end(); ++it ) {
        QSqlRecord* irec = cur.primeInsert();
        QVERIFY( irec != 0 );
        irec->setValue( "id", i );
        irec->setValue( "t_varchar", (*it) );
        irec->setValue( "t_char", (*it) );
        irec->setValue( "t_numeric", (double)i );
        ++i;
        QVERIFY( validReturns.contains(cur.insert()) );
    }

    QVERIFY( cur.select( "id >= 800 and id < 900" ) );

    int i2 = 800;
    while( cur.next() ) {
        QCOMPARE( cur.value( "id" ).toInt(), i2 );
        QCOMPARE( cur.value( "t_varchar" ).toString().stripWhiteSpace(), strings.at( i2 - 800 ) );
        QCOMPARE( cur.value( "t_char" ).toString().stripWhiteSpace(), strings.at( i2 - 800 ) );
        QCOMPARE( cur.value( "t_numeric" ).toDouble(), (double)i2 );
        ++i2;
    }
    QCOMPARE( i, i2 );

    populateTestTables( db );
}

void tst_Q3SqlCursor::batchInsert()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );
    QSet<int> validReturns(QSet<int>() << -1 << 1);

    QSqlQuery q( db );
    q.exec( "delete from " + qtest );

    Q3SqlCursor cur( qtest, true, db );

    int i = 0;
    for ( ; i < 100; ++i ) {
        QSqlRecord* irec = cur.primeInsert();
        Q_ASSERT( irec );
        irec->setValue( "id", i );
        irec->setValue( "t_varchar", "blah" );
        irec->setValue( "t_char", "blah" );
        irec->setValue( "t_numeric", 1.1 );
        if ( db.driverName().startsWith( "QSQLITE" ) ) {
            QVERIFY( cur.insert( true ) );
        } else {
            QVERIFY( validReturns.contains(cur.insert( true )) );
        }
    }

    for ( ; i < 200; ++i ) {
        QSqlRecord* irec = cur.primeInsert();
        Q_ASSERT( irec );
        irec->setValue( "id", i );
        irec->setValue( "t_varchar", "blah" );
        irec->setValue( "t_char", "blah" );
        irec->setValue( "t_numeric", 1.1 );
        if ( db.driverName().startsWith( "QSQLITE" ) ) {
            QVERIFY( cur.insert( false ) );
        } else {
            QVERIFY( validReturns.contains(cur.insert( false )) );
        }
    }

    i = 0;
    QVERIFY_SQL(q, exec( "select * from " + qtest + " order by id" ));
    while ( q.next() ) {
        QCOMPARE( q.value( 0 ).toInt(), i );
        i++;
    }

    QCOMPARE( i, 200 );

    populateTestTables( db );
}

static QString dumpUtf8( const QString& str )
{
    QString res;
    for ( int i = 0; i < (int)str.length(); ++i ) {
        res += "0x" + QString::number( str[ i ].unicode(), 16 ) + ' ';
    }
    return res;
}

void tst_Q3SqlCursor::insertORA()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    if (tst_Databases::getOraVersion(db) < 9)
        QSKIP("Need Oracle >= 9", SkipSingle);

    /****** CHARSET TEST ******/

    QSqlQuery q( db );
    QVERIFY_SQL(q, exec( "create table " + qTableName( "qtest_ovchar", __FILE__ ) + " ( id int primary key, t_char varchar(40) )" ));

    static const QString val1( "blah1" );

    Q3SqlCursor cur ( qTableName( "qtest_ovchar", __FILE__ ), true, db );
    QSqlRecord* irec = cur.primeInsert();
    irec->setValue( "id", 1 );
    irec->setValue( "t_char", val1 );
    QVERIFY( cur.insert() );

    QVERIFY_SQL(cur, select());
    QVERIFY( cur.next() );
    if ( cur.value( "t_char" ).toString() != val1 )
        qDebug( QString( "Wrong value for t_char: expected '%1', got '%2'" ).arg( val1 ).arg(
                cur.value( "t_char" ).toString() ) );

    static const unsigned short utf8arr[] = { 0xd792,0xd79c,0xd792,0xd79c,0xd799,0x00 };
    static const QString utf8str = QString::fromUcs2( utf8arr );

    irec = cur.primeInsert();
    irec->setValue( "id", 2 );
    irec->setValue( "t_char", utf8str );
    QVERIFY( cur.insert() );

    QVERIFY_SQL(cur, select( "id=2" ));
    QVERIFY( cur.next() );

    // until qtest knows non-fatal errors we use qDebug instead
    if ( cur.value( "t_char" ).toString() != utf8str )
        qDebug( QString( "Wrong value for t_char: expected '%1', got '%2'" ).arg( dumpUtf8 ( utf8str ) ).arg(
                dumpUtf8( cur.value( "t_char" ).toString() ) ) );

    /****** NCHARSET TEST ********/

    QVERIFY_SQL(q, exec( "create table " + qTableName( "qtest_onvchar", __FILE__ ) + " ( id int primary key, t_nchar nvarchar2(40) )" ));

    Q3SqlCursor cur2 ( qTableName( "qtest_onvchar", __FILE__ ), true, db );
    irec = cur2.primeInsert();
    irec->setValue( "id", 1 );
    irec->setValue( "t_nchar", val1 );
    QVERIFY( cur2.insert() );

    QVERIFY_SQL(cur2, select());
    QVERIFY( cur2.next() );
    if ( cur2.value( "t_nchar" ).toString() != val1 )
        qDebug( QString( "Wrong value for t_nchar: expected '%1', got '%2'" ).arg( val1 ).arg(
                cur2.value( "t_nchar" ).toString() ) );

    irec = cur2.primeInsert();
    irec->setValue( "id", 2 );
    irec->setValue( "t_nchar", utf8str );
    QVERIFY( cur2.insert() );

    QVERIFY_SQL(cur2, select( "id=2" ));
    QVERIFY( cur2.next() );

    // until qtest knows non-fatal errors we use qDebug instead
    if ( cur2.value( "t_nchar" ).toString() != utf8str )
        qDebug( QString( "Wrong value for t_nchar: expected '%1', got '%2'" ).arg( dumpUtf8( utf8str ) ).arg(
                dumpUtf8( cur2.value( "t_nchar" ).toString() ) ) );

}

void tst_Q3SqlCursor::unicode()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    static const QString utf8str = QString::fromUtf8( "ὕαλον ϕαγεῖν δύναμαι· τοῦτο οὔ με βλάπτει." );
    if ( !db.driver()->hasFeature( QSqlDriver::Unicode ) ) {
         QSKIP( "DBMS not Unicode capable", SkipSingle );
    }
    // ascii in the data storage, can't transliterate properly. invalid test.
    if(db.driverName().startsWith("QIBASE") && (db.databaseName() == "silence.nokia.troll.no:c:\\ibase\\testdb_ascii" || db.databaseName() == "/opt/interbase/qttest.gdb"))
        QSKIP("Can't transliterate extended unicode to ascii", SkipSingle);

    Q3SqlCursor cur( qTableName( "qtest_unicode", __FILE__ ), true, db );
    QSqlRecord* irec = cur.primeInsert();
    irec->setValue( 0, 500 );
    irec->setValue( 1, utf8str );
    irec->setValue( 2, utf8str );
    QVERIFY_SQL(cur, insert());
    QVERIFY_SQL(cur, select( "id=500" ));
    QVERIFY_SQL(cur, next());
    QString res = cur.value( 1 ).asString();
    cur.primeDelete();
    cur.del();

    if ( res != utf8str ) {
        int i;
        for ( i = 0; i < (int)res.length(); ++i ) {
            if ( res[ i ] != utf8str[ i ] )
                break;
        }
        if(db.driverName().startsWith("QMYSQL") || db.driverName().startsWith("QDB2"))
            qWarning() << "Needs someone with more Unicode knowledge than I have to fix:" << QString( "Strings differ at position %1: orig: %2, db: %3" ).arg( i ).arg( utf8str[ i ].unicode(), 0, 16 ).arg( res[ i ].unicode(), 0, 16 );
        else
            QFAIL( QString( "Strings differ at position %1: orig: %2, db: %3" ).arg( i ).arg( utf8str[ i ].unicode(), 0, 16 ).arg( res[ i ].unicode(), 0, 16 ) );
    }
    if((db.driverName().startsWith("QMYSQL") || db.driverName().startsWith("QDB2")) && res != utf8str)
        QEXPECT_FAIL("", "See above message", Continue);
    QVERIFY( res == utf8str );
}

void tst_Q3SqlCursor::precision()
{
    static const QString precStr = QLatin1String("1.23456789012345");
    static const double precDbl = 2.23456789012345;

    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    Q3SqlCursor cur( qTableName( "qtest_precision", __FILE__ ), true, db );
    cur.setTrimmed( "col1", true );
    QSqlRecord* irec = cur.primeInsert();
    irec->setValue( 0, precStr );
    QVERIFY( cur.insert() );

    irec = cur.primeInsert();
    irec->setValue( 0, precDbl );
    QVERIFY( cur.insert() );

    QVERIFY_SQL(cur, select());
    QVERIFY( cur.next() );
    QCOMPARE( cur.value( 0 ).asString(), precStr );
    QVERIFY( cur.next() );
    QCOMPARE( cur.value( 0 ).asDouble(), precDbl );
}

void tst_Q3SqlCursor::setFilter()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    Q3SqlCursor cur( qtest, true, db );
    cur.setFilter( "id = 2" );

    QVERIFY_SQL(cur, select());
    QVERIFY( cur.next() );
    QCOMPARE( cur.value( "id" ).toInt(), 2 );
    QVERIFY( !cur.next() );

    QVERIFY_SQL(cur, select());
    QVERIFY( cur.next() );
    QCOMPARE( cur.value( "id" ).toInt(), 2 );
    QVERIFY( !cur.next() );

    QVERIFY_SQL(cur, select( "id = 3" ));
    QVERIFY( cur.next() );
    QCOMPARE( cur.value( "id" ).toInt(), 3 );
    QVERIFY( !cur.next() );
    
    QVERIFY_SQL(cur, select());
    QVERIFY( cur.next() );
    QCOMPARE( cur.value( "id" ).toInt(), 3 );
    QVERIFY( !cur.next() );
}

void tst_Q3SqlCursor::select()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    Q3SqlCursor cur( qtest, true, db );
    QVERIFY_SQL(cur, select());
    QVERIFY( cur.next() );
    QVERIFY( cur.next() );

    Q3SqlCursor cur2( qtest, true, db );
    QVERIFY_SQL(cur2, select( "id = 1" ));
    QVERIFY( cur2.next() );
    QCOMPARE( cur2.value( 0 ).toInt(), 1 );

    Q3SqlCursor cur3( qtest, true, db );
    QVERIFY_SQL(cur3, select( cur3.primaryIndex( false ) ));
    QVERIFY( cur3.next() );
    QVERIFY( cur3.next() );
    QCOMPARE( cur3.value( 0 ).toInt(), 1 );

    Q3SqlCursor cur4( qtest, true, db );
    QSqlIndex idx = cur4.primaryIndex( false );
    QCOMPARE( (int)idx.count(), 2 );
    if ( tst_Databases::isMSAccess( db ) ) {
        QCOMPARE( idx.field( 1 ).name().upper(), QString("ID") );
        QCOMPARE( idx.field( 0 ).name().upper(), QString("T_VARCHAR") );
    } else {
        QCOMPARE( idx.field( 0 ).name().upper(), QString("ID") );
        QCOMPARE( idx.field( 1 ).name().upper(), QString("T_VARCHAR") );
    }

#ifdef QT_DEBUG
    // Ignore debugging message advising users of a potential pitfall.
    QTest::ignoreMessage(QtDebugMsg, "Q3SqlCursor::setValue(): This will not affect actual database values. Use primeInsert(), primeUpdate() or primeDelete().");
#endif
    cur4.setValue( "id", 1 );
#ifdef QT_DEBUG
    QTest::ignoreMessage(QtDebugMsg, "Q3SqlCursor::setValue(): This will not affect actual database values. Use primeInsert(), primeUpdate() or primeDelete().");
#endif
    cur4.setValue( "t_varchar", "VarChar1" );

    QVERIFY_SQL(cur4, select( idx, cur4.primaryIndex( false ) ));
    QVERIFY( cur4.next() );
    QCOMPARE( cur4.value( 0 ).toInt(), 1 );
}

void tst_Q3SqlCursor::setName()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );

    Q3SqlCursor c2( qtest, true, db );
    QCOMPARE( c2.name(), qtest );
    QCOMPARE( c2.fieldName( 0 ).lower(), QString( "id" ) );

    Q3SqlCursor c( QString(), true, db );
    c.setName( qtest );
    QCOMPARE( c.name(), qtest );
    QCOMPARE( c.fieldName( 0 ).lower(), QString( "id" ) );

    c.setName( qTableName( "qtest_precision", __FILE__ ) );
    QCOMPARE( c.name(), qTableName( "qtest_precision", __FILE__ ) );
    QCOMPARE( c.fieldName( 0 ).lower(), QString( "col1" ) );
}

/* Database independent test */
void tst_Q3SqlCursor::updateNoPK()
{
    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );
    QSet<int> validReturns(QSet<int>() << -1 << 1);

    QSqlQuery q(db);
    QVERIFY_SQL(q, exec("create table " + qTableName( "qtestPK", __FILE__ ) + " (id int, name varchar(20), num numeric)"));

    Q3SqlCursor cur(qTableName("qtestPK", __FILE__), true, db);
    QSqlRecord* rec = cur.primeInsert();
    Q_ASSERT(rec);
    rec->setNull(0);
    rec->setNull(1);
    rec->setNull(2);
    QVERIFY(validReturns.contains(cur.insert()));
    if (!db.driver()->hasFeature(QSqlDriver::PreparedQueries)) {

        // Only QPSQL, QMYSQL, QODBC and QOCI drivers currently use escape identifiers for column names
        if (db.driverName().startsWith("QPSQL") ||
                db.driverName().startsWith("QMYSQL") ||
                db.driverName().startsWith("QODBC") ||  
                db.driverName().startsWith("QOCI")) {
            QString query = QString::fromLatin1("insert into " + qTableName("qtestPK", __FILE__) +
                                                " (" + db.driver()->escapeIdentifier("id", QSqlDriver::FieldName) + ','
                                                + db.driver()->escapeIdentifier("name", QSqlDriver::FieldName) + ','
                                                + db.driver()->escapeIdentifier("num", QSqlDriver::FieldName) + ')'
                                                + " values (NULL,NULL,NULL)");
            QCOMPARE(cur.lastQuery(), query);
        } else {
            QCOMPARE(cur.lastQuery(), QString::fromLatin1("insert into " + qTableName("qtestPK", __FILE__) +
                                                         " (\"id\",\"name\",\"num\") values (NULL,NULL,NULL)"));
        }
    }

    rec = cur.primeUpdate();
    Q_ASSERT(rec);
    rec->setValue(0, 1);
    rec->setNull(1);
    rec->setNull(2);
    // Sqlite returns 2, don't ask why.
    QVERIFY(cur.update() != 0);
    QString expect = "update " + qTableName("qtestPK", __FILE__) +
            " set "+db.driver()->escapeIdentifier("id", QSqlDriver::FieldName)+" = 1 , "
            +db.driver()->escapeIdentifier("name", QSqlDriver::FieldName)+" = NULL , "
            +db.driver()->escapeIdentifier("num", QSqlDriver::FieldName)+" = NULL  where " + qTableName("qtestPK", __FILE__) + ".id"
            " IS NULL and " + qTableName("qtestPK", __FILE__) + ".name IS NULL and " +
            qTableName("qtestPK", __FILE__) + ".num IS NULL";
    if (!db.driver()->hasFeature(QSqlDriver::PreparedQueries)) {
        if (!db.driverName().startsWith("QSQLITE")) {
            QCOMPARE(cur.lastQuery(), expect);
        }
    }
    QVERIFY(cur.select(cur.index(QString("id"))));
    QVERIFY(cur.next());
    QCOMPARE(cur.value("id").toInt(), 1);
    QVERIFY(cur.isNull("name"));
    QVERIFY(cur.isNull("num")); 
}

// For task 117996: Q3SqlCursor::insert() should not fail even if field names 
// contain white spaces.
void tst_Q3SqlCursor::insertFieldNameContainsWS() {

    QFETCH( QString, dbName );
    QSqlDatabase db = QSqlDatabase::database( dbName );
    CHECK_DATABASE( db );
    QSet<int> validReturns(QSet<int>() << -1 << 1);

    // The bugfix (and this test) depends on QSqlDriver::escapeIdentifier(...) 
    // to be implemented, which is currently only the case for the 
    // QPSQL, QODBC and QOCI drivers.
    if (!db.driverName().startsWith("QPSQL") && 
            !db.driverName().startsWith("QODBC") &&  
            !db.driverName().startsWith("QOCI")) {
       QSKIP("PSQL, QODBC or QOCI specific test", SkipSingle);
       return;
    }

    QString tableName = qTableName("qtestws", __FILE__);

    QSqlQuery q(db);
    tst_Databases::safeDropTable(db, tableName);
    QString query = "CREATE TABLE %1 (id int, " 
        + db.driver()->escapeIdentifier("first Name", QSqlDriver::FieldName) 
        + " varchar(20), lastName varchar(20))";
    QVERIFY_SQL(q, exec(query.arg(tableName)));

    Q3SqlCursor cur(tableName, true, db);
    cur.select();

    QSqlRecord *r = cur.primeInsert();
    r->setValue("id", 1);
    r->setValue("firsT NaMe", "Kong");
    r->setValue("lastNaMe", "Harald");

    QVERIFY(validReturns.contains(cur.insert()));

    cur.select();
    cur.next();

    QVERIFY(cur.value(0) == 1);
    QCOMPARE(cur.value(1).toString(), QString("Kong"));
    QCOMPARE(cur.value(2).toString(), QString("Harald"));

    tst_Databases::safeDropTable(db, tableName);

}

QTEST_MAIN(tst_Q3SqlCursor)
#include "tst_q3sqlcursor.moc"

function test() {
    var db = openDatabaseSync("QmlTestDB-selection", "", "Test database from Qt autotests", 1000000);
    var r="transaction_not_finished";

    db.transaction(
        function(tx) {
            tx.executeSql('CREATE TABLE IF NOT EXISTS Greeting(salutation TEXT, salutee TEXT)');
            tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ]);
            tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ]);
        }
    );

    db.transaction(
        function(tx) {
            tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ]);
            tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ]);
            var rs = tx.executeSql('SELECT * FROM Greeting');
            if ( rs.rows.length != 4 )
                r = "SELECT RETURNED WRONG VALUE "+rs.rows.length+rs.rows[0]+rs.rows[1]
            else
                r = "passed";
        }
    );

    return r;
}

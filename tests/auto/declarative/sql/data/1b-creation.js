function test() {
    var db = openDatabaseSync("QmlTestDB-b", "", "Test B database from Qt autotests", 1000000,
            function(db) {
                db.transaction(function(tx){
                    tx.executeSql('CREATE TABLE Greeting(salutation TEXT, salutee TEXT)');
                })
            });
    var r;

    db.transaction(
        function(tx) {
            var rs = tx.executeSql('SELECT * FROM Greeting');
            if (rs.rows.length == 0)
                r = "passed";
            else
                r = "FAILED: got results:" + rs.rows.length
        }
    );

    return r;
}


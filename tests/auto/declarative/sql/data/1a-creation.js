function test() {
    var r=0;

    var db = openDatabaseSync("QmlTestDB", "", "Test database from Qt autotests", 1000000,
            function(db) {
                r = "FAILED: should have already been created";
                db.transaction(function(tx){
                    tx.executeSql('CREATE TABLE Greeting(salutation TEXT, salutee TEXT)');
                })
            });

    db.transaction(
        function(tx) {
            var rs = tx.executeSql('SELECT * FROM Greeting');
            if (r==0) {
                if (rs.rows.length==1) // created in 1-creation
                    r = "passed";
                else
                    r = "FAILED";
            }
        }
    );

    return r;
}

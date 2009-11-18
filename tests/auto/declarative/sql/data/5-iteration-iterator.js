function test() {
    var db = openDatabaseSync("QmlTestDB", "", "Test database from Qt autotests", 1000000);
    var r="transaction_not_finished";

    db.transaction(
        function(tx) {
            var rs = tx.executeSql('SELECT * FROM Greeting')
            var r1=""
            for(var i in rs.rows)
                r1 += rs.rows[i].salutation + ", " + rs.rows[i].salutee + ";"
            if (r1 != "hello, world;hello, world;hello, world;hello, world;")
                r = "SELECTED DATA WRONG: "+r1;
            else
                r = "passed";
        }
    );

    return r;
}

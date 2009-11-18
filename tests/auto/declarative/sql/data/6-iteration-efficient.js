function test() {
    var db = openDatabaseSync("QmlTestDB", "", "Test database from Qt autotests", 1000000);
    var r="transaction_not_finished";

    db.transaction(
        function(tx) {
            var rs = tx.executeSql('SELECT * FROM Greeting');
            var r1=""
            var fbefiore = rs.rows.forwardOnly;
            rs.rows.forwardOnly = true;
            var fafter = rs.rows.forwardOnly;
            if (fbefore)
                r = "forward wrong before";
            else if (!fafter)
                r = "forward wrong after";
            else {
                for(var i=0; rs.rows[i]; ++i) {
                    r1 += rs.rows[i].salutation + ", " + rs.rows[i].salutee + ";"
                }
                if (r1 != "hello, world;hello, world;hello, world;hello, world;")
                    r = "SELECTED DATA WRONG: "+r1;
                else
                    r = "passed";
            }
        }
    );

    return r;
}

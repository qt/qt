function test() {
    var db = openDatabaseSync("QmlTestDB", "", "Test database from Qt autotests", 1000000);
    var r="transaction_not_finished";

    try {
        db.transaction(
            function(tx) {
                var rs = tx.executeSql('SELECT * FROM NotExists');
                r = "SHOULD NOT SUCCEED";
            }
        );
    } catch (err) {
        if (err.message == "no such table: NotExists Unable to execute statement")
            r = "passed";
        else
            r = "WRONG ERROR="+err.message;
    }

    return r;
}

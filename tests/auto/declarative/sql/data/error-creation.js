function test() {
    var r="transaction_not_finished";
    try {
        var db = openDatabaseSync("QmlTestDB-creation", "2.0", "Test database from Qt autotests", 1000000);
    } catch (err) {
        if (err.message == "SQL: database version mismatch")
            r = "passed";
        else
            r = "WRONG ERROR="+err.message;
    }
    return r;
}

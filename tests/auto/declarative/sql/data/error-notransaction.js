function test() {
    var db = openDatabaseSync("QmlTestDB-data/error-notransaction", "1.0", "Test database from Qt autotests", 1000000);
    var r="transaction_not_finished";

    try {
        db.transaction();
    } catch (err) {
        if (err.message == "transaction: missing callback")
            r = "passed";
        else
            r = "WRONG ERROR="+err.message;
    }

    return r;
}

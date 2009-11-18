function test() {
    var db = openDatabaseSync("QmlTestDB", "", "Test database from Qt autotests", 1000000);
    var r="transaction_not_finished";

    // Asynchronous in WebKit, so must wait before calling test()  
    db.transaction(
        function(tx) {
            tx.executeSql('CREATE TABLE IF NOT EXISTS Greeting(salutation TEXT, salutee TEXT)');
            tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ]);
            r = "passed";
        }
    );

    return r;
}

var db;
db = openDatabase("QmlTestDB", "", "Test database from Qt autotests", 1000000);
var r="testerror";

// Asynchronous in WebKit, so must wait before calling test()  
db.transaction(function(tx) {
    r = "passed";
    tx.executeSql('CREATE TABLE IF NOT EXISTS Greeting(salutation TEXT, salutee TEXT)', [],
        function(tx, rs) { }, function(tx, error) { r="FAILED: "+error.message });
    tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ],
        function(tx, rs) { }, function(tx, error) { r="FAILED: "+error.message });
}, function(tx, error) { r="TXFAILED: "+error.message }, function(tx, result) { if (r=="testerror") r="passed" });


function test()
{
    return r;
}

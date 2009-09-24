var db = openDatabase("QmlTestDB", "", "Test database from Qt autotests", 1000000);
var r="transaction_not_finished";

// Asynchronous in WebKit, so must wait before calling test()  
db.transaction(
    function(tx) {
        tx.executeSql('CREATE TABLE IF NOT EXISTS Greeting(salutation TEXT, salutee TEXT)', [],
            function(tx, rs) { }, function(tx, error) { r="CREATE FAILED: "+error.message });
        tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ],
            function(tx, rs) { }, function(tx, error) { r="INSERT FAILED: "+error.message });
    },
    function(tx, error) { r="TRANSACTION FAILED: "+error.message },
    function(tx, result) { if (r=="transaction_not_finished") r="passed" }
);


function test()
{
    return r;
}

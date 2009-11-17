var db = openDatabase("QmlTestDB", "", "Test database from Qt autotests", 1000000);
var r=0;

db.transaction(
    function(tx) {
        tx.executeSql('SELECT * FROM NotExists', [],
            function(tx, rs) {
                r = "SHOULD NOT SUCCEED";
            },
            function(tx, error) {
                if (error.message == "no such table: NotExists Unable to execute statement" // QML
                 || error.message == "no such table: NotExists") r="passed" // WebKit
                else r="WRONG ERROR:"+error.message
            }
        );
    },
    function(tx, error) { if (r==0) r="TRANSACTION FAILED: "+error.message },
    function(tx, result) { if (r==0) r="SHOULD NOT SUCCEED2" }
);


function test()
{
    if (r == 0) r = "transaction_not_finished";
    return r;
}

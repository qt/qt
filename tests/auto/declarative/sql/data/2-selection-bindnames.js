var db = openDatabase("QmlTestDB", "", "Test database from Qt autotests", 1000000);
var r=0;

db.transaction(
    function(tx) {
        tx.executeSql('SELECT * FROM Greeting WHERE salutation=:p2 AND salutee=:p1', {':p1':'world', ':p2':'hello'},
            function(tx, rs) {
                if ( rs.rows.length != 4 ) {
                    if (r==0) r = "SELECT RETURNED WRONG VALUE "+rs.rows.length+rs.rows.item(0)+rs.rows.item(1)
                }
            },
            function(tx, error) { if (r==0) r="SELECT FAILED: "+error.message }
        );
    },
    function(tx, error) { if (r==0) r="TRANSACTION FAILED: "+error.message },
    function(tx, result) { if (r==0) r="passed" }
);


function test()
{
    if (r == 0) r = "transaction_not_finished";
    return r;
}

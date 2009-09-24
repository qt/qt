var db = openDatabase("QmlTestDB", "", "Test database from Qt autotests", 1000000);
var r=0;

db.transaction(
    function(tx) {
        tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ],
            function(tx, rs) { }, function(tx, error) { if (r==0) r="INSERT 1 FAILED: "+error.message });
        tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ],
            function(tx, rs) { }, function(tx, error) { if (r==0) r="INSERT 2 FAILED: "+error.message });
        tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ],
            function(tx, rs) { }, function(tx, error) { if (r==0) r="INSERT 3 FAILED: "+error.message });
        tx.executeSql('SELECT * FROM Greeting', [],
            function(tx, rs) {
                if ( rs.rows.length != 4 ) { // 1 from test1, 3 from this test.
                    if (r==0) r = "SELECT RETURNED WRONG VALUE "+rs.rows.length+rs.rows[0]+rs.rows[1]
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

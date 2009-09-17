import Qt 4.6

Text {
    Script {
        function allGreetings()
        {
            var db = openDatabase("QmlExampleDB", "", "The Example QML SQL!", 1000000);
            var r = ""

            db.transaction(function(tx) {
                tx.executeSql('CREATE TABLE IF NOT EXISTS Greeting(salutation TEXT, salutee TEXT)', []);
                tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ]);
                tx.executeSql('SELECT * FROM Greeting', [],
                    function(tx, rs) {
                        /* Inefficient HTML5-compatible way
                        for(var i = 0; i < rs.rows.length; i++) {
                            r += rs.rows[i][0] + ", " + rs.rows[i][1] + "\n"
                        }
                        */
                        /* Efficient way: forward only, not "length" query */
                        rs.rows.forwardOnly = true;
                        for(var i = 0; rs.rows[i]; i++) {
                            r += rs.rows[i][0] + ", " + rs.rows[i][1] + "\n"
                        }
                    },
                    function(tx, error) {
                        print("ERROR:", error.message)
                    }
                );
            })

            return r
        }
    }
    text: allGreetings()
}

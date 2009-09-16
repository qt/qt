import Qt 4.6

Text {
    Script {
        function test()
        {
            var db = openDatabase("QmlExampleDB", "", "The Example QML SQL!", 1000000);
            print(db)
            var r = ""

            db.transaction(function(tx) {
                tx.executeSql('CREATE TABLE IF NOT EXISTS Greeting(salutation TEXT, salutee TEXT)', []);
                tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ]);
                tx.executeSql('SELECT * FROM Greeting', [],
                    function(tx, rs) {
                        for(var i = 0; i < rs.rows.length; i++) {
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
    text: test()
}

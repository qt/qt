import Qt 4.6

Text {
    Script {
        function findGreetings() {
            var db = openDatabase("QmlExampleDB", "", "The Example QML SQL!", 1000000);

            db.transaction(
                function(tx) {
                    tx.executeSql('CREATE TABLE IF NOT EXISTS Greeting(salutation TEXT, salutee TEXT)', []);
                    tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ]);
                    tx.executeSql('SELECT * FROM Greeting', [],
                        function(tx, rs) {
                            var r = ""
                            for(var i = 0; i < rs.rows.length; i++) {
                                r += rs.rows.item(i).salutation + ", " + rs.rows.item(i).salutee + "\n"
                            }
                            text = r
                        },
                        function(tx, error) {
                            print("ERROR:", error.message)
                        }
                    );
                },
                function() {
                    print("ERROR in transaction");
                },
                function() {
                    print("Transaction successful");
                }
            )
        }
    }
    text: "?"
    Component.onCompleted: findGreetings()
}


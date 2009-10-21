import Qt 4.6

Text {
    Script {
        function findGreetings() {
            var db = openDatabase("QmlExampleDB", "1.0", "The Example QML SQL!", 1000000);

            db.transaction(
                function(tx) {
                    // Create the database if it doesn't already exist
                    tx.executeSql('CREATE TABLE IF NOT EXISTS Greeting(salutation TEXT, salutee TEXT)', []);

                    // Add (another) greeting row
                    tx.executeSql('INSERT INTO Greeting VALUES(?, ?)', [ 'hello', 'world' ]);

                    // Show all added greetings
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


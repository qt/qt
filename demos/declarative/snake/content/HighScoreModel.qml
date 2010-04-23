import Qt 4.7

// Models a high score table.
//
// Use this component like this:
//
//  HighScoreModel {
//      id: highScores
//      game: "MyCoolGame"
//  }
//
// Then use either use the top-score properties:
//
//  Text { text: "HI: " + highScores.topScore }
//
// or, use the model in a view:
//
//  ListView {
//      model: highScore
//      delegate: Component {
//                    ... player ... score ...
//                }
//  }
//
// Add new scores via:
//
//  saveScore(newScore)
//
// or:
//
//  savePlayerScore(playerName,newScore)
//
// The best maxScore scores added by this method will be retained in an SQL database,
// and presented in the model and in the topScore/topPlayer properties.
//

ListModel {
    id: model
    property string game: ""
    property int topScore: 0
    property string topPlayer: ""
    property int maxScores: 10

    function __db()
    {
        return openDatabaseSync("HighScoreModel", "1.0", "Generic High Score Functionality for QML", 1000000);
    }
    function __ensureTables(tx)
    {
        tx.executeSql('CREATE TABLE IF NOT EXISTS HighScores(game TEXT, score INT, player TEXT)', []);
    }

    function fillModel() {
        __db().transaction(
            function(tx) {
                __ensureTables(tx);
                var rs = tx.executeSql("SELECT score,player FROM HighScores WHERE game=? ORDER BY score DESC", [game]);
                model.clear();
                if (rs.rows.length > 0) {
                    topScore = rs.rows.item(0).score
                    topPlayer = rs.rows.item(0).player
                    for (var i=0; i<rs.rows.length; ++i) {
                        if (i < maxScores)
                            model.append(rs.rows.item(i))
                    }
                    if (rs.rows.length > maxScores)
                        tx.executeSql("DELETE FROM HighScores WHERE game=? AND score <= ?",
                                            [rs.rows.item(maxScores).score]);
                }
            }
        )
    }

    function savePlayerScore(player,score) {
        __db().transaction(
            function(tx) {
                __ensureTables(tx);
                tx.executeSql("INSERT INTO HighScores VALUES(?,?,?)", [game,score,player]);
                fillModel();
            }
        )
    }

    function saveScore(score) {
        savePlayerScore("player",score);
    }

    function clearScores() {
        __db().transaction(
            function(tx) {
                tx.executeSql("DELETE FROM HighScores WHERE game=?", [game]);
                fillModel();
            }
        )
    }

    Component.onCompleted: { fillModel() }
}

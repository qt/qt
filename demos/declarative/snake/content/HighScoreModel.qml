/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0

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
                                            [game, rs.rows.item(maxScores).score]);
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

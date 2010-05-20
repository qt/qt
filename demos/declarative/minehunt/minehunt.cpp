/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include <stdlib.h>
#include <qdeclarativeextensionplugin.h>
#include <qdeclarativecontext.h>
#include <qdeclarativeengine.h>
#include <qdeclarative.h>

#include <QTime>
#include <QTimer>

class TileData : public QObject
{
    Q_OBJECT
public:
    TileData() : _hasFlag(false), _hasMine(false), _hint(-1), _flipped(false) {}

    Q_PROPERTY(bool hasFlag READ hasFlag WRITE setHasFlag NOTIFY hasFlagChanged)
    bool hasFlag() const { return _hasFlag; }

    Q_PROPERTY(bool hasMine READ hasMine NOTIFY hasMineChanged)
    bool hasMine() const { return _hasMine; }

    Q_PROPERTY(int hint READ hint NOTIFY hintChanged)
    int hint() const { return _hint; }

    Q_PROPERTY(bool flipped READ flipped NOTIFY flippedChanged())
    bool flipped() const { return _flipped; }

    void setHasFlag(bool flag) {if(flag==_hasFlag) return; _hasFlag = flag; emit hasFlagChanged();}
    void setHasMine(bool mine) {if(mine==_hasMine) return; _hasMine = mine; emit hasMineChanged();}
    void setHint(int hint) { if(hint == _hint) return; _hint = hint; emit hintChanged(); }
    void flip() { if (_flipped) return; _flipped = true; emit flippedChanged(); }
    void unflip() { if(!_flipped) return; _flipped = false; emit flippedChanged(); }

signals:
    void flippedChanged();
    void hasFlagChanged();
    void hintChanged();
    void hasMineChanged();

private:
    bool _hasFlag;
    bool _hasMine;
    int _hint;
    bool _flipped;
};

class MinehuntGame : public QObject
{
    Q_OBJECT
public:
    MinehuntGame();

    Q_PROPERTY(QDeclarativeListProperty<TileData> tiles READ tiles CONSTANT)
    QDeclarativeListProperty<TileData> tiles();

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    bool isPlaying() {return playing;}

    Q_PROPERTY(bool hasWon READ hasWon NOTIFY hasWonChanged)
    bool hasWon() {return won;}

    Q_PROPERTY(int numMines READ numMines NOTIFY numMinesChanged)
    int numMines() const{return nMines;}

    Q_PROPERTY(int numFlags READ numFlags NOTIFY numFlagsChanged)
    int numFlags() const{return nFlags;}

public slots:
    Q_INVOKABLE bool flip(int row, int col);
    Q_INVOKABLE bool flag(int row, int col);
    void setBoard();
    void reset();

signals:
    void isPlayingChanged();
    void hasWonChanged();
    void numMinesChanged();
    void numFlagsChanged();

private:
    bool onBoard( int r, int c ) const { return r >= 0 && r < numRows && c >= 0 && c < numCols; }
    TileData *tile( int row, int col ) { return onBoard(row, col) ? _tiles[col+numRows*row] : 0; }
    int getHint(int row, int col);
    void setPlaying(bool b){if(b==playing) return; playing=b; emit isPlayingChanged();}

    QList<TileData *> _tiles;
    int numCols;
    int numRows;
    bool playing;
    bool won;
    int remaining;
    int nMines;
    int nFlags;
};

void tilesPropAppend(QDeclarativeListProperty<TileData>* prop, TileData* value)
{
    Q_UNUSED(prop);
    Q_UNUSED(value);
    return; //Append not supported
}

int tilesPropCount(QDeclarativeListProperty<TileData>* prop)
{
    return static_cast<QList<TileData*>*>(prop->data)->count();
}

TileData* tilesPropAt(QDeclarativeListProperty<TileData>* prop, int index)
{
    return static_cast<QList<TileData*>*>(prop->data)->at(index);
}

QDeclarativeListProperty<TileData> MinehuntGame::tiles(){
    return QDeclarativeListProperty<TileData>(this, &_tiles, &tilesPropAppend,
            &tilesPropCount, &tilesPropAt, 0);
}

MinehuntGame::MinehuntGame()
: numCols(9), numRows(9), playing(true), won(false)
{
    setObjectName("mainObject");
    srand(QTime(0,0,0).secsTo(QTime::currentTime()));

    //initialize array
    for(int ii = 0; ii < numRows * numCols; ++ii) {
        _tiles << new TileData;
    }
    reset();

}

void MinehuntGame::setBoard()
{
    foreach(TileData* t, _tiles){
        t->setHasMine(false);
        t->setHint(-1);
    }
    //place mines
    int mines = nMines;
    remaining = numRows*numCols-mines;
    while ( mines ) {
        int col = int((double(rand()) / double(RAND_MAX)) * numCols);
        int row = int((double(rand()) / double(RAND_MAX)) * numRows);

        TileData* t = tile( row, col );

        if (t && !t->hasMine()) {
            t->setHasMine( true );
            mines--;
        }
    }

    //set hints
    for (int r = 0; r < numRows; r++)
        for (int c = 0; c < numCols; c++) {
            TileData* t = tile(r, c);
            if (t && !t->hasMine()) {
                int hint = getHint(r,c);
                t->setHint(hint);
            }
        }

    setPlaying(true);
}

void MinehuntGame::reset()
{
    foreach(TileData* t, _tiles){
        t->unflip();
        t->setHasFlag(false);
    }
    nMines = 12;
    nFlags = 0;
    emit numMinesChanged();
    emit numFlagsChanged();
    setPlaying(false);
    QTimer::singleShot(600,this, SLOT(setBoard()));
}

int MinehuntGame::getHint(int row, int col)
{
    int hint = 0;
    for (int c = col-1; c <= col+1; c++)
        for (int r = row-1; r <= row+1; r++) {
            TileData* t = tile(r, c);
            if (t && t->hasMine())
                hint++;
        }
    return hint;
}

bool MinehuntGame::flip(int row, int col)
{
    if(!playing)
        return false;

    TileData *t = tile(row, col);
    if (!t || t->hasFlag())
        return false;

    if(t->flipped()){
        int flags = 0;
        for (int c = col-1; c <= col+1; c++)
            for (int r = row-1; r <= row+1; r++) {
                TileData *nearT = tile(r, c);
                if(!nearT || nearT == t)
                    continue;
                if(nearT->hasFlag())
                    flags++;
            }
        if(!t->hint() || t->hint() != flags)
            return false;
        for (int c = col-1; c <= col+1; c++)
            for (int r = row-1; r <= row+1; r++) {
                TileData *nearT = tile(r, c);
                if (nearT && !nearT->flipped() && !nearT->hasFlag()) {
                    flip( r, c );
                }
            }
        return true;
    }

    t->flip();

    if (t->hint() == 0) {
        for (int c = col-1; c <= col+1; c++)
            for (int r = row-1; r <= row+1; r++) {
                TileData* t = tile(r, c);
                if (t && !t->flipped()) {
                    flip( r, c );
                }
            }
    }

    if(t->hasMine()){
        for (int r = 0; r < numRows; r++)//Flip all other mines
            for (int c = 0; c < numCols; c++) {
                TileData* t = tile(r, c);
                if (t && t->hasMine()) {
                    flip(r, c);
                }
            }
        won = false;
        hasWonChanged();
        setPlaying(false);
    }

    remaining--;
    if(!remaining){
        won = true;
        hasWonChanged();
        setPlaying(false);
    }
    return true;
}

bool MinehuntGame::flag(int row, int col)
{
    TileData *t = tile(row, col);
    if(!t)
        return false;

    t->setHasFlag(!t->hasFlag());
    nFlags += (t->hasFlag()?1:-1);
    emit numFlagsChanged();
    return true;
}

class MinehuntExtensionPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT

    public:
    void registerTypes(const char *uri) {
        Q_UNUSED(uri);
        qmlRegisterType<TileData>();
    }

    void initializeEngine(QDeclarativeEngine *engine, const char *uri) {
        Q_UNUSED(uri);

        srand(QTime(0,0,0).secsTo(QTime::currentTime()));

        MinehuntGame* game = new MinehuntGame();

        engine->rootContext()->setContextObject(game);
    }
};

#include "minehunt.moc"

Q_EXPORT_PLUGIN(MinehuntExtensionPlugin);


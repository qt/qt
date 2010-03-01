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
#include "qdeclarativeengine.h"
#include "qdeclarativecontext.h"
#include "qdeclarative.h"
#include <qdeclarativeitem.h>
#include <qdeclarativeview.h>

#include <QWidget>
#include <QApplication>
#include <QFile>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <QFileInfo>

QString fileName = "minehunt.qml";

class Tile : public QObject
{
    Q_OBJECT
public:
    Tile() : _hasFlag(false), _hasMine(false), _hint(-1), _flipped(false) {}

    Q_PROPERTY(bool hasFlag READ hasFlag WRITE setHasFlag NOTIFY hasFlagChanged);
    bool hasFlag() const { return _hasFlag; }

    Q_PROPERTY(bool hasMine READ hasMine NOTIFY hasMineChanged);
    bool hasMine() const { return _hasMine; }

    Q_PROPERTY(int hint READ hint NOTIFY hintChanged);
    int hint() const { return _hint; }

    Q_PROPERTY(bool flipped READ flipped NOTIFY flippedChanged());
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

QML_DECLARE_TYPE(Tile);

class MyWidget : public QWidget
{
Q_OBJECT
public:
    MyWidget(int = 370, int = 480, QWidget *parent=0, Qt::WindowFlags flags=0);
    ~MyWidget();

    Q_PROPERTY(QDeclarativeListProperty<Tile> tiles READ tiles CONSTANT);
    QDeclarativeListProperty<Tile> tiles() { return _tilesProperty; }

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged);
    bool isPlaying() {return playing;}

    Q_PROPERTY(bool hasWon READ hasWon NOTIFY hasWonChanged);
    bool hasWon() {return won;}

    Q_PROPERTY(int numMines READ numMines NOTIFY numMinesChanged);
    int numMines() const{return nMines;}

    Q_PROPERTY(int numFlags READ numFlags NOTIFY numFlagsChanged);
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
    Tile *tile( int row, int col ) { return onBoard(row, col) ? _tiles[col+numRows*row] : 0; }
    int getHint(int row, int col);
    void setPlaying(bool b){if(b==playing) return; playing=b; emit isPlayingChanged();}

    QDeclarativeView *canvas;

    QList<Tile *> _tiles;
    QDeclarativeListProperty<Tile> _tilesProperty;
    int numCols;
    int numRows;
    bool playing;
    bool won;
    int remaining;
    int nMines;
    int nFlags;
};

Q_DECLARE_METATYPE(QList<Tile*>)
MyWidget::MyWidget(int width, int height, QWidget *parent, Qt::WindowFlags flags)
: QWidget(parent, flags), canvas(0), numCols(9), numRows(9), playing(true), won(false)
{
    setObjectName("mainWidget");
    srand(QTime(0,0,0).secsTo(QTime::currentTime()));

    //initialize array
    for(int ii = 0; ii < numRows * numCols; ++ii) {
        _tiles << new Tile;
    }
    reset();

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setMargin(0);
    setLayout(vbox);

    canvas = new QDeclarativeView(this);
    canvas->setFixedSize(width, height);
    vbox->addWidget(canvas);

    _tilesProperty = QDeclarativeListProperty<Tile>(this, _tiles);

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->addDefaultObject(this);

    canvas->setSource(QUrl::fromLocalFile(fileName));
}

MyWidget::~MyWidget()
{
}

void MyWidget::setBoard()
{
    foreach(Tile* t, _tiles){
        t->setHasMine(false);
        t->setHint(-1);
    }
    //place mines
    int mines = nMines;
    remaining = numRows*numCols-mines;
    while ( mines ) {
        int col = int((double(rand()) / double(RAND_MAX)) * numCols);
        int row = int((double(rand()) / double(RAND_MAX)) * numRows);

        Tile* t = tile( row, col );

        if (t && !t->hasMine()) {
            t->setHasMine( true );
            mines--;
        }
    }

    //set hints
    for (int r = 0; r < numRows; r++)
        for (int c = 0; c < numCols; c++) {
            Tile* t = tile(r, c);
            if (t && !t->hasMine()) {
                int hint = getHint(r,c);
                t->setHint(hint);
            }
        }

    setPlaying(true);
}

void MyWidget::reset()
{
    foreach(Tile* t, _tiles){
        t->unflip();
        t->setHasFlag(false);
    }
    nMines = 12;
    nFlags = 0;
    setPlaying(false);
    QTimer::singleShot(600,this, SLOT(setBoard()));
}

int MyWidget::getHint(int row, int col)
{
    int hint = 0;
    for (int c = col-1; c <= col+1; c++)
        for (int r = row-1; r <= row+1; r++) {
            Tile* t = tile(r, c);
            if (t && t->hasMine())
                hint++;
        }
    return hint;
}

bool MyWidget::flip(int row, int col)
{
    if(!playing)
        return false;

    Tile *t = tile(row, col);
    if (!t || t->hasFlag())
        return false;

    if(t->flipped()){
        int flags = 0;
        for (int c = col-1; c <= col+1; c++)
            for (int r = row-1; r <= row+1; r++) {
                Tile *nearT = tile(r, c);
                if(!nearT || nearT == t)
                    continue;
                if(nearT->hasFlag())
                    flags++;
            }
        if(!t->hint() || t->hint() != flags)
            return false;
        for (int c = col-1; c <= col+1; c++)
            for (int r = row-1; r <= row+1; r++) {
                Tile *nearT = tile(r, c);
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
                Tile* t = tile(r, c);
                if (t && !t->flipped()) {
                    flip( r, c );
                }
            }
    }

    if(t->hasMine()){
        for (int r = 0; r < numRows; r++)//Flip all other mines
            for (int c = 0; c < numCols; c++) {
                Tile* t = tile(r, c);
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

bool MyWidget::flag(int row, int col)
{
    Tile *t = tile(row, col);
    if(!t)
        return false;

    t->setHasFlag(!t->hasFlag());
    nFlags += (t->hasFlag()?1:-1);
    emit numFlagsChanged();
    return true;
}
/////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
#ifdef Q_WS_X11
    // native on X11 is terrible for this demo.
    QApplication::setGraphicsSystem("raster");
#endif
    QApplication app(argc, argv);

    bool frameless = false;

    int width = 370;
    int height = 480;

    QML_REGISTER_TYPE(0,0,0,Tile,Tile);

    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "-frameless") {
            frameless = true;
        } else if(arg == "-width" && i < (argc - 1)) {
            ++i;
            width = ::atoi(argv[i]);
        } else if(arg == "-height" && i < (argc - 1)) {
            ++i;
            height = ::atoi(argv[i]);
        } else if (arg[0] != '-') {
            fileName = arg;
        }
    }

    MyWidget wid(width, height, 0, frameless ? Qt::FramelessWindowHint : Qt::Widget);
    wid.show();

    return app.exec();
}

#include "main.moc"

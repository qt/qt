/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TETRIXBOARD_H
#define TETRIXBOARD_H

#include <QTimer>
#include <QFrame>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class TetrixBoard : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QObject* timer READ getTimer)
    Q_PROPERTY(QWidget* nextPieceLabel WRITE setNextPieceLabel)

public:
    TetrixBoard(QWidget *parent = 0);

    void setNextPieceLabel(QWidget *label);
    void setBoardWidth(int width);
    void setBoardHeight(int height);
    QSize minimumSizeHint() const;

    QObject *getTimer();

signals:
    void scoreChanged(int score);
    void levelChanged(int level);
    void linesRemovedChanged(int numLines);

#if !defined(Q_MOC_RUN)
private: // can only be emitted by TetrixBoard
#endif
    void keyPressed(int key);
    void paintRequested(QPainter *painter);
    void paintNextPieceRequested(QPainter *painter);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

protected slots:
    void showNextPiece(int width, int height);
    void drawPauseScreen(QPainter *painter);
    void drawSquare(QPainter *painter, int x, int y, int shape);
    void update();

private:
    enum { BoardWidth = 10, BoardHeight = 22 };

    int squareWidth() { return contentsRect().width() / BoardWidth; }
    int squareHeight() { return contentsRect().height() / BoardHeight; }

    QTimer *timer;
    QPointer<QLabel> nextPieceLabel;
    QImage image;
};

#endif

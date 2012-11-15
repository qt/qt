/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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

/********************************************************************************
** Form generated from reading ui file 'tetrixwindow.ui'
**
** Created: Thu Jul 10 09:47:35 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef TETRIXWINDOW_H
#define TETRIXWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "tetrixboard.h"

QT_BEGIN_NAMESPACE

class Ui_TetrixWindow
{
public:
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QPushButton *startButton;
    QLCDNumber *linesLcd;
    QLabel *linesRemovedLabel;
    QPushButton *pauseButton;
    QLCDNumber *scoreLcd;
    TetrixBoard *board;
    QLabel *levelLabel;
    QLabel *nextLabel;
    QLCDNumber *levelLcd;
    QLabel *scoreLabel;
    QLabel *nextPieceLabel;
    QPushButton *quitButton;

    void setupUi(QWidget *TetrixWindow)
    {
        if (TetrixWindow->objectName().isEmpty())
            TetrixWindow->setObjectName(QString::fromUtf8("TetrixWindow"));
        TetrixWindow->resize(537, 475);
        vboxLayout = new QVBoxLayout(TetrixWindow);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setMargin(9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        gridLayout = new QGridLayout();
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setMargin(0);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        startButton = new QPushButton(TetrixWindow);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        startButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(startButton, 4, 0, 1, 1);

        linesLcd = new QLCDNumber(TetrixWindow);
        linesLcd->setObjectName(QString::fromUtf8("linesLcd"));
        linesLcd->setSegmentStyle(QLCDNumber::Filled);

        gridLayout->addWidget(linesLcd, 3, 2, 1, 1);

        linesRemovedLabel = new QLabel(TetrixWindow);
        linesRemovedLabel->setObjectName(QString::fromUtf8("linesRemovedLabel"));
        linesRemovedLabel->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(linesRemovedLabel, 2, 2, 1, 1);

        pauseButton = new QPushButton(TetrixWindow);
        pauseButton->setObjectName(QString::fromUtf8("pauseButton"));
        pauseButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pauseButton, 5, 2, 1, 1);

        scoreLcd = new QLCDNumber(TetrixWindow);
        scoreLcd->setObjectName(QString::fromUtf8("scoreLcd"));
        scoreLcd->setSegmentStyle(QLCDNumber::Filled);

        gridLayout->addWidget(scoreLcd, 1, 2, 1, 1);

        board = new TetrixBoard(TetrixWindow);
        board->setObjectName(QString::fromUtf8("board"));
        board->setFocusPolicy(Qt::StrongFocus);
        board->setFrameShape(QFrame::Panel);
        board->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(board, 0, 1, 6, 1);

        levelLabel = new QLabel(TetrixWindow);
        levelLabel->setObjectName(QString::fromUtf8("levelLabel"));
        levelLabel->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(levelLabel, 2, 0, 1, 1);

        nextLabel = new QLabel(TetrixWindow);
        nextLabel->setObjectName(QString::fromUtf8("nextLabel"));
        nextLabel->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(nextLabel, 0, 0, 1, 1);

        levelLcd = new QLCDNumber(TetrixWindow);
        levelLcd->setObjectName(QString::fromUtf8("levelLcd"));
        levelLcd->setSegmentStyle(QLCDNumber::Filled);

        gridLayout->addWidget(levelLcd, 3, 0, 1, 1);

        scoreLabel = new QLabel(TetrixWindow);
        scoreLabel->setObjectName(QString::fromUtf8("scoreLabel"));
        scoreLabel->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(scoreLabel, 0, 2, 1, 1);

        nextPieceLabel = new QLabel(TetrixWindow);
        nextPieceLabel->setObjectName(QString::fromUtf8("nextPieceLabel"));
        nextPieceLabel->setFrameShape(QFrame::Box);
        nextPieceLabel->setFrameShadow(QFrame::Raised);
        nextPieceLabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(nextPieceLabel, 1, 0, 1, 1);

        quitButton = new QPushButton(TetrixWindow);
        quitButton->setObjectName(QString::fromUtf8("quitButton"));
        quitButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(quitButton, 4, 2, 1, 1);


        vboxLayout->addLayout(gridLayout);


        retranslateUi(TetrixWindow);

        QMetaObject::connectSlotsByName(TetrixWindow);
    } // setupUi

    void retranslateUi(QWidget *TetrixWindow)
    {
        TetrixWindow->setWindowTitle(QApplication::translate("TetrixWindow", "Tetrix", 0, QApplication::UnicodeUTF8));
        startButton->setText(QApplication::translate("TetrixWindow", "&Start", 0, QApplication::UnicodeUTF8));
        linesRemovedLabel->setText(QApplication::translate("TetrixWindow", "LINES REMOVED", 0, QApplication::UnicodeUTF8));
        pauseButton->setText(QApplication::translate("TetrixWindow", "&Pause", 0, QApplication::UnicodeUTF8));
        levelLabel->setText(QApplication::translate("TetrixWindow", "LEVEL", 0, QApplication::UnicodeUTF8));
        nextLabel->setText(QApplication::translate("TetrixWindow", "NEXT", 0, QApplication::UnicodeUTF8));
        scoreLabel->setText(QApplication::translate("TetrixWindow", "SCORE", 0, QApplication::UnicodeUTF8));
        nextPieceLabel->setText(QString());
        quitButton->setText(QApplication::translate("TetrixWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(TetrixWindow);
    } // retranslateUi

};

namespace Ui {
    class TetrixWindow: public Ui_TetrixWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // TETRIXWINDOW_H

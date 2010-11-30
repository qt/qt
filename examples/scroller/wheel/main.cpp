/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <QtGui>
#include <qmath.h>

#include "wheelwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(bool touch)
        : QMainWindow()
    {
        makeSlotMachine(touch);
        setCentralWidget(m_slotMachine);
    }

    void makeSlotMachine(bool touch)
    {
        if (QApplication::desktop()->width() > 1000) {
            QFont f = font();
            f.setPointSize(f.pointSize() * 2);
            setFont(f);
        }

        m_slotMachine = new QWidget(this);
        QGridLayout *grid = new QGridLayout(m_slotMachine);
        grid->setSpacing(20);

        QStringList colors;
        colors << "Red" << "Magenta" << "Peach" << "Orange" << "Yellow" << "Citro" << "Green" << "Cyan" << "Blue" << "Violet";

        m_wheel1 = new StringWheelWidget(touch);
        m_wheel1->setItems( colors );
        grid->addWidget( m_wheel1, 0, 0 );

        m_wheel2 = new StringWheelWidget(touch);
        m_wheel2->setItems( colors );
        grid->addWidget( m_wheel2, 0, 1 );

        m_wheel3 = new StringWheelWidget(touch);
        m_wheel3->setItems( colors );
        grid->addWidget( m_wheel3, 0, 2 );

        QPushButton *shakeButton = new QPushButton(tr("Shake"));
        connect(shakeButton, SIGNAL(clicked()), this, SLOT(rotateRandom()));

        grid->addWidget( shakeButton, 1, 0, 1, 3 );
    }

private slots:
    void rotateRandom()
    {
        m_wheel1->scrollTo(m_wheel1->currentIndex() + (qrand() % 200));
        m_wheel2->scrollTo(m_wheel2->currentIndex() + (qrand() % 200));
        m_wheel3->scrollTo(m_wheel3->currentIndex() + (qrand() % 200));
    }

private:
    QWidget *m_slotMachine;

    StringWheelWidget *m_wheel1;
    StringWheelWidget *m_wheel2;
    StringWheelWidget *m_wheel3;
};

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    bool touch = a.arguments().contains(QLatin1String("--touch"));

    MainWindow *mw = new MainWindow(touch);
    mw->show();

    return a.exec();
}

#include "main.moc"

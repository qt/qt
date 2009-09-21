/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
#include "ui_keypadnavigation.h"

class KeypadNavigation : public QMainWindow
{
    Q_OBJECT

public:
    KeypadNavigation(QWidget *parent = 0)
        : QMainWindow(parent)
        , ui(new Ui_KeypadNavigation)
    {
        ui->setupUi(this);

        connect(ui->m_actionLayoutVerticalSimple, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutVerticalSimple, ui->m_pageVerticalSimple);
        connect(ui->m_actionLayoutVerticalComplex, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutVerticalComplex, ui->m_pageVerticalComplex);
        connect(ui->m_actionLayoutTwoDimensional, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutTwoDimensional, ui->m_pageTwoDimensional);
        connect(ui->m_actionLayoutSliderMagic, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutSliderMagic, ui->m_pageSliderMagic);
        connect(ui->m_actionLayoutChaos, SIGNAL(triggered()), &m_layoutSignalMapper, SLOT(map()));
        m_layoutSignalMapper.setMapping(ui->m_actionLayoutChaos, ui->m_pageChaos);
        connect(&m_layoutSignalMapper, SIGNAL(mapped(QWidget*)), ui->m_stackWidget, SLOT(setCurrentWidget(QWidget*)));

        connect(ui->m_actionModeNone, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeNone, int(Qt::NavigationModeNone));
        connect(ui->m_actionModeKeypadTabOrder, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeKeypadTabOrder, int(Qt::NavigationModeKeypadTabOrder));
        connect(ui->m_actionModeKeypadDirectional, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeKeypadDirectional, int(Qt::NavigationModeKeypadDirectional));
        connect(ui->m_actionModeCursorAuto, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeCursorAuto, int(Qt::NavigationModeCursorAuto));
        connect(ui->m_actionModeCursorForceVisible, SIGNAL(triggered()), &m_modeSignalMapper, SLOT(map()));
        m_modeSignalMapper.setMapping(ui->m_actionModeCursorForceVisible, int(Qt::NavigationModeCursorForceVisible));
        connect(&m_modeSignalMapper, SIGNAL(mapped(int)), SLOT(setNavigationMode(int)));
    }

    ~KeypadNavigation()
    {
        delete ui;
    }

public slots:
    void setNavigationMode(int mode)
    {
        QApplication::setNavigationMode(Qt::NavigationMode(mode));
    }

private:
    Ui_KeypadNavigation *ui;
    QSignalMapper m_layoutSignalMapper;
    QSignalMapper m_modeSignalMapper;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KeypadNavigation w;
#ifdef Q_WS_S60
    w.showMaximized();
#else
    w.show();
#endif
    return a.exec();
}

#include "main.moc"

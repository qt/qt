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


#include <QtTest/QtTest>

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <Q3GroupBox>
#include <Q3ScrollView>
#include <Q3PopupMenu>
#include <QMenuBar>
#include <QLabel>
#include <QLineEdit>
#include <Q3ProgressBar>
#include <QSplitter>
#include <QToolBox>
#include <QLCDNumber>

template <typename WidgetType>
void callAllQt3FrameFunctions(WidgetType *widget)
{
    int e;
    e = WidgetType::Plain;
    e = WidgetType::Raised;
    e = WidgetType::Sunken;
    e = WidgetType::MShadow;
    e = WidgetType::NoFrame;
    e = WidgetType::Box;
    e = WidgetType::Panel;
    e = WidgetType::StyledPanel;
    e = WidgetType::HLine;
    e = WidgetType::VLine;
    e = WidgetType::GroupBoxPanel;
    e = WidgetType::WinPanel;
    e = WidgetType::ToolBarPanel;
    e = WidgetType::MenuBarPanel;
    e = WidgetType::PopupPanel;
    e = WidgetType::LineEditPanel;
    e = WidgetType::TabWidgetPanel;
    e = WidgetType::MShape;
    
    widget->contentsRect();
    widget->setFrameRect(widget->frameRect());
    widget->setFrameShape(widget->frameShape());
    widget->setFrameStyle(widget->frameStyle());
    widget->frameWidth();
    widget->setLineWidth(widget->lineWidth());
    widget->setMidLineWidth(widget->midLineWidth());
    
    const int margin = 10;
    widget->setMargin(margin);
    if (widget->margin() != 10)
        qFatal("Error: margin() did not return the value set width setMargin()");
}

template <typename WidgetType>
void callConstQt3FrameFunctions(WidgetType const *widget)
{
    widget->contentsRect();
    widget->frameRect();
    widget->frameShape();
    widget->frameStyle();
    widget->frameWidth();
    widget->lineWidth();
    widget->margin();
    widget->midLineWidth();
}

template <typename WidgetType>
void callQt3FrameFunctions(WidgetType *widget)
{
    callAllQt3FrameFunctions(widget);
    callConstQt3FrameFunctions(widget);
    delete widget;
}

class tst_Q3Frame : public QObject
{
Q_OBJECT

private slots:
    void frameFunctions();
};

/*
    A few classes inherit QFrame in Qt 3 but no longer does
    in Qt 4. Test that those classes have dummy implementations 
    for the missing functions.
*/
void tst_Q3Frame::frameFunctions()
{
    callQt3FrameFunctions(new Q3ScrollView());
    callQt3FrameFunctions(new Q3GroupBox());
    callQt3FrameFunctions(new Q3PopupMenu());
    callQt3FrameFunctions(new QMenuBar());
    callQt3FrameFunctions(new QLabel());
    callQt3FrameFunctions(new QLineEdit());
    callQt3FrameFunctions(new Q3ProgressBar());
    callQt3FrameFunctions(new QSplitter());
    callQt3FrameFunctions(new QToolBox());
    callQt3FrameFunctions(new QLCDNumber());
}

QTEST_MAIN(tst_Q3Frame)
#include "tst_q3frame.moc"

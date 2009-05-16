/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_schema.h"

//! [0]
class MainWindow : public QMainWindow,
                   private Ui::SchemaMainWindow
{
    Q_OBJECT

  public:
    MainWindow();

  private Q_SLOTS:
    void schemaSelected(int index);
    void instanceSelected(int index);
    void validate();
    void textChanged();

  private:
    void moveCursor(int line, int column);
};
//! [0]
#endif

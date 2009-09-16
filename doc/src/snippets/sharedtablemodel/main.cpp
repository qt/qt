/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

/*
  main.cpp

  A simple example that shows how a single model can be shared between
  multiple views.
*/

#include <QApplication>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QTableView>

#include "model.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TableModel *model = new TableModel(4, 2, &app);

//! [0]
    QTableView *firstTableView = new QTableView;
    QTableView *secondTableView = new QTableView;
//! [0]

//! [1]
    firstTableView->setModel(model);
    secondTableView->setModel(model);
//! [1]

    firstTableView->horizontalHeader()->setModel(model);

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 2; ++column) {
            QModelIndex index = model->index(row, column, QModelIndex());
            model->setData(index, QVariant(QString("(%1, %2)").arg(row).arg(column)));
        }
    }

//! [2]
    secondTableView->setSelectionModel(firstTableView->selectionModel());
//! [2]

    firstTableView->setWindowTitle("First table view");
    secondTableView->setWindowTitle("Second table view");
    firstTableView->show();
    secondTableView->show();
    return app.exec();
}

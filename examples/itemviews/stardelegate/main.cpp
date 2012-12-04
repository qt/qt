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

#include <QtGui>

#include "stardelegate.h"
#include "stareditor.h"
#include "starrating.h"

//! [0]
void populateTableWidget(QTableWidget *tableWidget)
{
    static const struct {
        const char *title;
        const char *genre;
        const char *artist;
        int rating;
    } staticData[] = {
//! [0] //! [1]
        { "Mass in B-Minor", "Baroque", "J.S. Bach", 5 },
//! [1]
        { "Three More Foxes", "Jazz", "Maynard Ferguson", 4 },
        { "Sex Bomb", "Pop", "Tom Jones", 3 },
        { "Barbie Girl", "Pop", "Aqua", 5 },
//! [2]
        { 0, 0, 0, 0 }
//! [2] //! [3]
    };
//! [3] //! [4]

    for (int row = 0; staticData[row].title != 0; ++row) {
        QTableWidgetItem *item0 = new QTableWidgetItem(staticData[row].title);
        QTableWidgetItem *item1 = new QTableWidgetItem(staticData[row].genre);
        QTableWidgetItem *item2 = new QTableWidgetItem(staticData[row].artist);
        QTableWidgetItem *item3 = new QTableWidgetItem;
        item3->setData(0,
                       qVariantFromValue(StarRating(staticData[row].rating)));

        tableWidget->setItem(row, 0, item0);
        tableWidget->setItem(row, 1, item1);
        tableWidget->setItem(row, 2, item2);
        tableWidget->setItem(row, 3, item3);
    }
}
//! [4]

//! [5]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTableWidget tableWidget(4, 4);
    tableWidget.setItemDelegate(new StarDelegate);
    tableWidget.setEditTriggers(QAbstractItemView::DoubleClicked
                                | QAbstractItemView::SelectedClicked);
    tableWidget.setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList headerLabels;
    headerLabels << "Title" << "Genre" << "Artist" << "Rating";
    tableWidget.setHorizontalHeaderLabels(headerLabels);

    populateTableWidget(&tableWidget);

    tableWidget.resizeColumnsToContents();
    tableWidget.resize(500, 300);
    tableWidget.show();

    return app.exec();
}
//! [5]

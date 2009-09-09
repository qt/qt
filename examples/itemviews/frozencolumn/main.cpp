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

#include <QApplication>
#include <QStandardItemModel>
#include <QFile>

#include "freezetablewidget.h"

int main( int argc, char** argv )
{

      Q_INIT_RESOURCE(grades);


      QApplication app( argc, argv );
      QStandardItemModel *model=new QStandardItemModel();


      QFile file(":/grades.txt");
      QString line;
      QStringList list;
      if (file.open(QFile::ReadOnly)) {
            line = file.readLine(200);
            list= line.simplified().split(",");
            model->setHorizontalHeaderLabels(list);

            int row=0;
            QStandardItem *newItem=0;
            while(file.canReadLine()){
                  line = file.readLine(200);
                  if(!line.startsWith("#") && line.contains(",")){
                        list= line.simplified().split(",");
                        for(int col=0; col<list.length(); col++){
                              newItem = new QStandardItem(list.at(col));
                              model->setItem(row ,col, newItem);
                        }
                        row++;
                  }
            }
      }
      file.close();

      FreezeTableWidget *tableView = new FreezeTableWidget(model);

      tableView->setWindowTitle(QObject::tr("Frozen Column Example"));
      tableView->resize(560,680);
      tableView->show();
      return app.exec();
}


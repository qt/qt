/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [0]
fileName = QFileDialog::getOpenFileName(this,
    tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
//! [0]


//! [1]
"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
//! [1]


//! [2]
QFileDialog dialog(this);
dialog.setFileMode(QFileDialog::AnyFile);
//! [2]


//! [3]
dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
//! [3]


//! [4]
dialog.setViewMode(QFileDialog::Detail);
//! [4]


//! [5]
QStringList fileNames;
if (dialog.exec())
    fileNames = dialog.selectedFiles();
//! [5]


//! [6]
dialog.setNameFilter("All C++ files (*.cpp *.cc *.C *.cxx *.c++)");
dialog.setNameFilter("*.cpp *.cc *.C *.cxx *.c++");
//! [6]


//! [7]
QStringList filters;
filters << "Image files (*.png *.xpm *.jpg)"
        << "Text files (*.txt)"
        << "Any files (*)";

QFileDialog dialog(this);
dialog.setNameFilters(filters);
dialog.exec();
//! [7]


//! [8]
QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                "/home",
                                                tr("Images (*.png *.xpm *.jpg)"));
//! [8]


//! [9]
QStringList files = QFileDialog::getOpenFileNames(
                        this,
                        "Select one or more files to open",
                        "/home",
                        "Images (*.png *.xpm *.jpg)");
//! [9]


//! [10]
QStringList list = files;
QStringList::Iterator it = list.begin();
while(it != list.end()) {
    myProcessing(*it);
    ++it;
}
//! [10]


//! [11]
QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                           "/home/jana/untitled.png",
                           tr("Images (*.png *.xpm *.jpg)"));
//! [11]


//! [12]
QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
//! [12]

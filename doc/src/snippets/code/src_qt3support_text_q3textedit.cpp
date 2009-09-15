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

//! [0]
QFile file(fileName); // Read the text from a file
if (file.open(IO_ReadOnly)) {
    QTextStream stream(&file);
    textEdit->setText(stream.read());
}

QFile file(fileName); // Write the text to a file
if (file.open(IO_WriteOnly)) {
    QTextStream stream(&file);
    stream << textEdit->text();
    textEdit->setModified(false);
}
//! [0]


//! [1]
This is <font color=red>red</font> while <b>this</b> is <font color=blue>blue</font>.
<font color=green><font color=yellow>Yellow,</font> and <u>green</u>.
//! [1]


//! [2]
Q3TextEdit * log = new Q3TextEdit(this);
log->setTextFormat(Qt::LogText);
Q3StyleSheetItem * item = new Q3StyleSheetItem(log->styleSheet(), "mytag");
item->setColor("red");
item->setFontWeight(QFont::Bold);
item->setFontUnderline(true);
log->append("This is a <mytag>custom tag</mytag>!");
//! [2]

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
QFile data("output.txt");
if (data.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&data);
    out << "Result: " << qSetFieldWidth(10) << left << 3.14 << 2.7;
    // writes "Result: 3.14      2.7       "
}
//! [0]


//! [1]
QTextStream stream(stdin);
QString line;
do {
    line = stream.readLine();
} while (!line.isNull());
//! [1]


//! [2]
QTextStream in("0x50 0x20");
int firstNumber, secondNumber;

in >> firstNumber;             // firstNumber == 80
in >> dec >> secondNumber;     // secondNumber == 0

char ch;
in >> ch;                      // ch == 'x'
//! [2]


//! [3]
int main(int argc, char *argv[])
{
    // read numeric arguments (123, 0x20, 4.5...)
    for (int i = 1; i < argc; ++i) {
          int number;
          QTextStream in(argv[i]);
          in >> number;
          ...
    }
}
//! [3]


//! [4]
QString str;
QTextStream in(stdin);
in >> str;
//! [4]


//! [5]
QString s;
QTextStream out(&s);
out.setFieldWidth(10);
out.setFieldAlignment(QTextStream::AlignCenter);
out.setPadChar('-');
out << "Qt" << "rocks!";
//! [5]


//! [6]
----Qt------rocks!--
//! [6]


//! [7]
QTextStream in(file);
QChar ch1, ch2, ch3;
in >> ch1 >> ch2 >> ch3;
//! [7]


//! [8]
QTextStream out(stdout);
out << "Qt rocks!" << endl;
//! [8]


//! [9]
stream << '\n' << flush;
//! [9]


//! [10]
QTextStream out(&file);
out.setCodec("UTF-8");
//! [10]

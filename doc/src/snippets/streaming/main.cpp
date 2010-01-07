/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtGui>
#include <QByteArray>
#include <QDataStream>

//! [0]
struct Movie
{
    int id;
    QString title;
    QDate releaseDate;
};
//! [0]

//! [1]
QDataStream &operator<<(QDataStream &out, const Movie &movie)
{
    out << (quint32)movie.id << movie.title
        << movie.releaseDate;
    return out;
}
//! [1]

//! [2]
QDataStream &operator>>(QDataStream &in, Movie &movie)
{
    quint32 id;
    QDate date;

    in >> id >> movie.title >> date;
    movie.id = (int)id;
    movie.releaseDate = date;
    return in;
}
//! [2]

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Movie m;
    m.id = 0001;
    m.title = "Happy Meal";
    m.releaseDate = QDate(1995, 5, 17);

    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << m;
    
    // display
    qDebug() << m.id << m.releaseDate << m.title;

    Movie m2;
    
    int id2;
    QString title2;
    QDate date2;

    QDataStream stream2(byteArray);
    stream2 >> id2 >> title2 >> date2;

    m2.id = id2;
    m2.title = title2;
    m2.releaseDate = date2;

    qDebug() << id2 << " " << date2 << " " << title2;

    return 0;
}

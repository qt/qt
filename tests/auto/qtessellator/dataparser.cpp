/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/
#include "dataparser.h"
#include <QString>
#include <QVector>
#include <QFile>
#include <QTextStream>

#include <QtDebug>

static QList<qreal> parseNumbersList(QByteArray::const_iterator &itr)
{
    QList<qreal> points;
    QByteArray temp;
    while ((*itr) == ' ')
        ++itr;
    while (((*itr) >= '0' && (*itr) <= '9') ||
           (*itr) == '-' || (*itr) == '+') {
        temp = QByteArray();

        if ((*itr) == '-')
            temp += *itr++;
        else if ((*itr) == '+')
            temp += *itr++;
        while ((*itr) >= '0' && (*itr) <= '9')
            temp += *itr++;
        if ((*itr) == '.')
            temp += *itr++;
        while ((*itr) >= '0' && (*itr) <= '9')
            temp += *itr++;
        if (( *itr) == 'e') {
            temp += *itr++;
            if ((*itr) == '-' ||
                (*itr) == '+')
                temp += *itr++;
        }
        while ((*itr) >= '0' && (*itr) <= '9')
            temp += *itr++;
        while ((*itr) == ' ')
            ++itr;
        if ((*itr) == ',')
            ++itr;
        points.append(temp.toDouble());
        //eat the rest of space
        while ((*itr) == ' ')
            ++itr;
    }

    return points;
}

static QList<QPointF> parsePoints(const QByteArray &line)
{
    QList<QPointF> res;

    QByteArray::const_iterator it = line.constBegin();
    if (*it == ',')
        ++it;
    
    QList<qreal> nums = parseNumbersList(it);
    QList<qreal>::const_iterator nitr;
    for (nitr = nums.begin(); nitr != nums.end(); ++nitr) {
        qreal x = *nitr;
        ++nitr;
        if (nitr == nums.end()) {
            qWarning() << "parsePoints: Even number of co-ordinates required, odd number found: skipping last point";
            break;
        }
        qreal y = *nitr;
        res.append(QPointF(x, y));
    }

    return res;
}

QList< QVector<QPointF> > parseData(const QByteArray &contents)
{
    QList<QByteArray> lines = contents.split('\n');
    QList<QByteArray>::const_iterator itr;

    QList< QVector<QPointF> > res;
    QVector<QPointF> current;

    for (itr = lines.begin(); itr != lines.end(); ++itr) {
        QByteArray line = (*itr).trimmed();
        if (line.isEmpty() || line.startsWith('/')) {
            if (!current.isEmpty()) {
                res.append(current);
                current = QVector<QPointF>();
            }
            continue;
        } else {
            QList<QPointF> lst = parsePoints(line);
            current << lst.toVector();
        }
    }
    return res;
}

QList< QVector<QPointF> > parseFile(const QString &fileName)
{
    QList< QVector<QPointF> > res;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"couldn't open "<<fileName;
        return res;
    }

    QVector<QPointF> current;

    while (!file.atEnd()) {
        QByteArray line = file.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('/')) {
            if (!current.isEmpty()) {
                res.append(current);
                current = QVector<QPointF>();
            }
            continue;
        } else {
            QList<QPointF> lst = parsePoints(line);
            current << lst.toVector();
        }
    }

    return res;
}

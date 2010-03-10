/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the FOO module of the Qt Toolkit.
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
class StaticWidget : public QWidget
{
Q_OBJECT
public:
    int hue;
    StaticWidget(QWidget *parent = 0)
    :QWidget(parent)
    {
        setAttribute(Qt::WA_StaticContents);
        setAttribute(Qt::WA_OpaquePaintEvent);
           hue = 200;
    }

    void resizeEvent(QResizeEvent *)
    {
  //      qDebug() << "static widget resize from" << e->oldSize() << "to" << e->size();
    }

    void paintEvent(QPaintEvent *e)
    {
        QPainter p(this);
        static int color = 200;
        color = (color + 41) % 205 + 50;
//        color = ((color + 45) %150) + 100;
        qDebug() << "static widget repaint" << e->rect();
        p.fillRect(e->rect(), QColor::fromHsv(hue, 255, color));
        p.setPen(QPen(QColor(Qt::white)));

        for (int y = e->rect().top(); y <= e->rect().bottom() + 1; ++y) {
            if (y % 20 == 0)
                p.drawLine(e->rect().left(), y, e->rect().right(), y);
        }

        for (int x = e->rect().left(); x <= e->rect().right() +1 ; ++x) {
            if (x % 20 == 0)
                p.drawLine(x, e->rect().top(), x, e->rect().bottom());
        }
    }
};

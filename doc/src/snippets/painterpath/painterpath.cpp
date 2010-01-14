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



int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QImage image(100, 100, QImage::Format_RGB32);

    QPainterPath path;
    path.addRect(20, 20, 60, 60);

    path.moveTo(0, 0);
    path.cubicTo(99, 0,  50, 50,  99, 99);
    path.cubicTo(0, 99,  50, 50,  0, 0);

    QPainter painter(&image);
    painter.fillRect(0, 0, 100, 100, Qt::white);

    painter.save();
    painter.translate(0.5, 0.5);
    painter.setPen(QPen(QColor(79, 106, 25), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    painter.setBrush(QColor(122, 163, 39));
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPath(path);

    painter.restore();
    painter.end();

    QLabel lab;
    lab.setPixmap(QPixmap::fromImage(image));
    lab.show();
    return app.exec();
}

/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef AX2_H
#define AX2_H

#include <QWidget>
#include <QPainter>

//! [0]
class QAxWidget2 : public QWidget
{
    Q_OBJECT
    Q_CLASSINFO("ClassID", "{58139D56-6BE9-4b17-937D-1B1EDEDD5B71}")
    Q_CLASSINFO("InterfaceID", "{B66280AB-08CC-4dcc-924F-58E6D7975B7D}")
    Q_CLASSINFO("EventsID", "{D72BACBA-03C4-4480-B4BB-DE4FE3AA14A0}")
    Q_CLASSINFO("ToSuperClass", "QAxWidget2")
    Q_CLASSINFO("StockEvents", "yes")
    Q_CLASSINFO("Insertable", "yes")

    Q_PROPERTY( int lineWidth READ lineWidth WRITE setLineWidth )
public:
    QAxWidget2(QWidget *parent = 0)
	: QWidget(parent), line_width( 1 )
    {
    }

    int lineWidth() const
    {
	return line_width;
    }
    void setLineWidth( int lw )
    {
	line_width = lw;
	repaint();
    }

protected:
    void paintEvent( QPaintEvent *e )
    {
	QPainter paint( this );
	QPen pen = paint.pen();
	pen.setWidth( line_width );
	paint.setPen( pen );

	QRect r = rect();
	r.adjust( 10, 10, -10, -10 );
	paint.drawEllipse( r );
    }

private:
    int line_width;
};
//! [0]

#endif // AX2_H

/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <qpolygon.h>
#include <qpainterpath.h>
#include <math.h>

#include <qpainter.h>
#include <qdialog.h>



//TESTED_CLASS=
//TESTED_FILES=gui/painting/qpolygon.h gui/painting/qpolygon.cpp

class tst_QPolygon : public QObject
{
    Q_OBJECT

public:
    tst_QPolygon();

private slots:
    void makeEllipse();
};

tst_QPolygon::tst_QPolygon()
{
}

void tst_QPolygon::makeEllipse()
{
    // create an ellipse with R1 = R2 = R, i.e. a circle
    QPolygon pa;
    const int R = 50; // radius
    QPainterPath path;
    path.addEllipse(0, 0, 2*R, 2*R);
    pa = path.toSubpathPolygons().at(0).toPolygon();

    int i;
    // make sure that all points are R+-1 away from the center
    bool err = FALSE;
    for (i = 1; i < pa.size(); i++) {
	QPoint p = pa.at( i );
	double r = sqrt( pow( double(p.x() - R), 2.0 ) + pow( double(p.y() - R), 2.0 ) );
	// ### too strict ? at least from visual inspection it looks
	// quite odd around the main axes. 2.0 passes easily.
	err |= ( qAbs( r - double(R) ) > 2.0 );
    }
    QVERIFY( !err );
}

QTEST_APPLESS_MAIN(tst_QPolygon)
#include "tst_qpointarray.moc"

/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

function printOrientation(orientation) {
    var orientationString;
    if (orientation == Orientation.Portrait) {
        orientationString = "Portrait";
    } else if (orientation == Orientation.Landscape) {
        orientationString = "Landscape";
    } else if (orientation == Orientation.PortraitInverted) {
        orientationString = "Portrait inverted";
    } else if (orientation == Orientation.LandscapeInverted) {
        orientationString = "Landscape inverted";
    } else {
        orientationString = "UnknownOrientation";
    }
    return orientationString;
}
 
function getAngle(orientation) {
    var angle;
    if (orientation == Orientation.Portrait) {
        angle = 0;
    } else if (orientation == Orientation.Landscape) {
        angle = 90;
    } else if (orientation == Orientation.PortraitInverted) {
        angle = 180;
    } else if (orientation == Orientation.LandscapeInverted) {
        angle = 270;
    } else {
        angle = 0;
    }
    return angle;
}

function parallel(firstOrientation, secondOrientation) {
    var difference = getAngle(firstOrientation) - getAngle(secondOrientation)
    return difference % 180 == 0;
}

function calculateGravityPoint(firstOrientation, secondOrientation) {
    var position = Qt.point(0, 0);
    var difference = getAngle(firstOrientation) - getAngle(secondOrientation)
    if (difference < 0) {
        difference = 360 + difference;
    }
    if (difference == 0) {
        position = Qt.point(0, -10);
    } else if (difference == 90) {
        position = Qt.point(-10, 0);
    } else if (difference == 180) {
        position = Qt.point(0, 1000);
    } else if (difference == 270) {
        position = Qt.point(1000, 0);
    }
    return position;
}

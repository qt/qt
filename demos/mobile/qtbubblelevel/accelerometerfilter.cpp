/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <math.h>
#include "accelerometerfilter.h"

#define RADIANS_TO_DEGREES 57.2957795

AccelerometerFilter::AccelerometerFilter()
    : x(0), y(0), z(0)
{
}

//! [0]
bool AccelerometerFilter::filter(QAccelerometerReading *reading)
{
    qreal rx = reading->x();
    qreal ry = reading->y();
    qreal rz = reading->z();

    qreal divider = sqrt(rx * rx + ry * ry + rz * rz);

    // Lowpass factor
#ifdef Q_OS_SYMBIAN
    float lowPassFactor = 0.10;
#else
    float lowPassFactor = 0.05;
#endif

    // Calculate the axis angles in degrees and reduce the noise in sensor
    // readings.
    x += (acos(rx / divider) * RADIANS_TO_DEGREES - 90 - x) * lowPassFactor;
    y += (acos(ry / divider) * RADIANS_TO_DEGREES - 90 - y) * lowPassFactor;
    z += (acos(rz / divider) * RADIANS_TO_DEGREES - 90 - z) * lowPassFactor;

    // The orientations of the accelerometers are different between
    // Symbian and Maemo devices so we use the different axes
    // depending on the platform.
#if defined(Q_OS_SYMBIAN)
    emit rotationChanged(-y);
#else
    emit rotationChanged(x);
#endif

    // Don't store the reading in the sensor.
    return false;
}
//! [0]

/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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

/* JavaScript functions for creating, handling and
 * destroying the bars of the adjuster component. */

var maxBars = 10;
var firstBarHeight = 5;
var bars = new Array(maxBars);
var colors = new Array(maxBars);
var barWidthFactor = 2.3;

function createBars() {
    var barWidth = adjuster.width/maxBars/barWidthFactor;
    for (var i = 1; i <= maxBars; i++) {
        //Create, configure and store the bars.
        var bar = Qt.createQmlObject('import QtQuick 1.0; Rectangle {smooth: true}', adjuster, "dynamicBar");
        bar.width = barWidth;
        bar.height = firstBarHeight+(i/maxBars)*(adjuster.height*0.8-firstBarHeight);
        bar.radius = bar.width/2;
        bar.x = (i-1)*(barWidthFactor*barWidth);
        bar.y = adjuster.height/2 - bar.height/2;
        bars[i-1] = bar;

        //Calculate and store the colors
        if (i < maxBars/2) {
            colors[i-1] = Qt.rgba(1-i/(maxBars/2), 1, 1-i/(maxBars/2), i);
        }
        else {
            colors[i-1] = Qt.rgba((i-(maxBars/2))/(maxBars/2), 1-((i-(maxBars/2))/(maxBars/2)), 0, i);
        }
    }
}

function destroyBars() {
    for (var i = 0; i < maxBars; i++) {
        bars[i].color = "transparent" //Colors must be set to transparent or otherwise the bars will stay appeared.
        bars[i].destroy();
    }
}

function fillBars(barNumber) {
    //Set the color for the bars (transparent from the selected bar to the end).
    for (var i = 0; i < maxBars; i++) {
        if (i < barNumber) {
            bars[i].color = colors[i];
        }
        else {
            bars[i].color = "black";
        }
    }
}

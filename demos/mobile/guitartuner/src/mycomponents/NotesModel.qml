/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0

/* The model component for the NoteButtonView. */
ListModel {
    ListElement {
        note: "E"
        offSource: "./images/tuner_e.png"
        onSource: "./images/tuner_e_on.png"
        bigSource: "./mycomponents/images/big_e.png"
        glowSource: "./mycomponents/images/glowing_e.png"
        frequency: "82.407"
        interval: "5"
    }
    ListElement {
        note: "A"
        offSource: "./images/tuner_a.png"
        onSource: "./images/tuner_a_on.png"
        bigSource: "./mycomponents/images/big_a.png"
        glowSource: "./mycomponents/images/glowing_a.png"
        frequency: "110.00"
        interval: "5"
    }
    ListElement {
        note: "D"
        offSource: "./images/tuner_d.png"
        onSource: "./images/tuner_d_on.png"
        bigSource: "./mycomponents/images/big_d.png"
        glowSource: "./mycomponents/images/glowing_d.png"
        frequency: "146.83"
        interval: "5"
    }
    ListElement {
        note: "G"
        offSource: "./images/tuner_g.png"
        onSource: "./images/tuner_g_on.png"
        bigSource: "./mycomponents/images/big_g.png"
        glowSource: "./mycomponents/images/glowing_g.png"
        frequency: "196.00"
        interval: "4"
    }
    ListElement {
        note: "B"
        offSource: "./images/tuner_b.png"
        onSource: "./images/tuner_b_on.png"
        bigSource: "./mycomponents/images/big_b.png"
        glowSource: "./mycomponents/images/glowing_b.png"
        frequency: "246.94"
        interval: "5"
    }
    ListElement {
        note: "e"
        offSource: "./images/tuner_e.png"
        onSource: "./images/tuner_e_on.png"
        bigSource: "./mycomponents/images/big_e.png"
        glowSource: "./mycomponents/images/glowing_e.png"
        frequency: "329.63"
        interval: "9999" //Big enough that we can't move over this note
    }
    ListElement {
        note: "Auto"
        offSource: "./images/tuner_auto.png"
        onSource: "./images/tuner_auto_on.png"
        frequency: "82.407"
    }
}

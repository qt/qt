/**
 * Copyright (c) 2011 Nokia Corporation. All rights reserved.
 *
 * Nokia and Nokia Connecting People are registered trademarks of Nokia
 * Corporation. Oracle and Java are registered trademarks of Oracle and/or its
 * affiliates. Other product and company names mentioned herein may be
 * trademarks or trade names of their respective owners.
 *
 *
 * Subject to the conditions below, you may, without charge:
 *
 *  *  Use, copy, modify and/or merge copies of this software and associated
 *     documentation files (the "Software")
 *
 *  *  Publish, distribute, sub-licence and/or sell new software derived from
 *     or incorporating the Software.
 *
 *
 *
 * This file, unmodified, shall be included with all copies or substantial
 * portions of the Software that are distributed in source code form.
 *
 * The Software cannot constitute the primary value of any new software derived
 * from or incorporating the Software.
 *
 * Any person dealing with the Software shall not misrepresent the source of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

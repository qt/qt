/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PHONON_MMF_DEFS_H
#define PHONON_MMF_DEFS_H

#include <QtGlobal>

// The following macros are for switching on / off various bits of code, 
// in order to debug the current problems with video visibility.

// If this is defined, then VideoOutput is essentially just a typedef for
// QWidget.  This is to allow us to test whether the QWidget function 
// overrides present in VideoOutput (e.g. sizeHint, paintEvent etc) may
// be the cause of the visibility problems.
//#define PHONON_MMF_VIDEOOUTPUT_IS_QWIDGET

// Use hard-coded rectangle coordinates, rather than using CCoeControl
// rect, i.e. QWidget::winId()->Rect()
#define PHONON_MMF_HARD_CODE_VIDEO_RECT

namespace Phonon
{
    namespace MMF
    {
        static const qint32	DefaultTickInterval = 10;
        static const qreal	InitialVolume = 0.5;
        
        enum MediaType
        {
            MediaTypeUnknown,
            MediaTypeAudio,
            MediaTypeVideo
        };
    }
}

#endif // PHONON_MMF_DEFS_H

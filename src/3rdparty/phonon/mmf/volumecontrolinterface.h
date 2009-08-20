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

#ifndef PHONON_MMF_VOLUMECONTROLINTERFACE_H
#define PHONON_MMF_VOLUMECONTROLINTERFACE_H

#include <QtGlobal>

namespace Phonon
{
    namespace MMF
    {
        /**
         * Interface used by AudioOutput to pass volume control commands
         * back along the audio path to the MediaObject.
         */
        class VolumeControlInterface
        {
        public:
			virtual qreal volume() const = 0;
			
			/**
			 * Returns true if the volume of the underlying audio stack is
			 * changed as a result of this call.  The return value is used
			 * by the AudioDevice to determine whether to emit a 
			 * volumeChanged signal.
			 */
			virtual bool setVolume(qreal volume) = 0;
        };
    }
}

#endif

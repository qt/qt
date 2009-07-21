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

#ifndef PHONON_MMF_AUDIOOUTPUT_H
#define PHONON_MMF_AUDIOOUTPUT_H

#include <Phonon/audiooutputinterface.h>

namespace Phonon
{
    namespace MMF
    {
        class Backend;
        class MediaObject;

        /**
         * @short AudioOutputInterface implementation for MMF.
         *
         * Implements the AudioOutputInterface for Symbian/S60's MMF
         * framework.
         *
         * This class has a very small role, we simply access CDrmPlayerUtility
         * in MediaObject::m_player and forward everything there.
         *
         * \section volume Volume
         *
         * Phonon's concept on volume is from 0.0 to 1.0, and from 1< it does
         * voltage multiplication. CDrmPlayerUtility goes from 1 to
         * CDrmPlayerUtility::MaxVolume(). We apply some basic math to convert
         * between the two.
         *
         * @author Frans Englich<frans.englich@nokia.com>
         */
        class AudioOutput : public QObject
                          , public AudioOutputInterface
        {
            Q_OBJECT
            Q_INTERFACES(Phonon::AudioOutputInterface)

        public:
            AudioOutput(Backend *backend, QObject *parent);
            virtual qreal volume() const;
            virtual void setVolume(qreal);

            virtual int outputDevice() const;

            /**
             * Has no effect.
             */
            virtual bool setOutputDevice(int);

            /**
             * Has no effect.
             */
            virtual bool setOutputDevice(const Phonon::AudioOutputDevice &);

            void setMediaObject(MediaObject *mo);

        Q_SIGNALS:
            void volumeChanged(qreal newVolume);

        private:
            MediaObject *   m_mediaObject;
            qreal           m_volume;
            TInt            m_maxVolume;
        };
    }
}

#endif

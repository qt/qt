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

#include "utils.h"
#include <e32std.h>
#include <mmf/common/mmferrors.h>

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \namespace Phonon::MMF::Utils
  \internal
*/

/*! \class Phonon::MMF::TTraceContext
  \internal
*/

/*! \enum Phonon::MMF::PanicCode
 \internal
*/

/*! \enum Phonon::MMF::TTraceCategory
 \internal
*/

/*! \enum Phonon::MMF::MediaType
 \internal
*/

_LIT(PanicCategory, "Phonon::MMF");

void MMF::Utils::panic(PanicCode code)
{
    User::Panic(PanicCategory, code);
}

_LIT(KMimePrefixAudio, "audio/");
_LIT(KMimePrefixVideo, "video/");
_LIT(KMimeSDP, "application/sdp");

enum ConstantStringLengths {
    KMimePrefixLength = 6, // either "audio/" or "video/",
    KMimeSDPLength = 15 // "application/sdp"
};

MMF::MediaType MMF::Utils::mimeTypeToMediaType(const TDesC& mimeType)
{
    if (mimeType.Left(KMimePrefixLength).Compare(KMimePrefixAudio) == 0) {
        return MediaTypeAudio;
    } else if (mimeType.Left(KMimePrefixLength).Compare(KMimePrefixVideo) == 0 ||
               mimeType.Left(KMimeSDPLength).Compare(KMimeSDP) == 0) {
        return MediaTypeVideo;
    } else
        return MediaTypeUnknown;
}

QString MMF::Utils::symbianErrorToString(int errorCode)
{
    /**
     * Here we translate only the error codes which are likely to be
     * meaningful to the user.  For example, when an error occurs
     * during opening of a media file, displaying "not found" or
     * "permission denied" is informative.  On the other hand,
     * differentiating between KErrGeneral and KErrArgument at the UI
     * level does not make sense.
     */
    switch (errorCode)
    {
    // System-wide errors
    case KErrNone:
        return tr("No error");
    case KErrNotFound:
        return tr("Not found");
    case KErrNoMemory:
        return tr("Out of memory");
    case KErrNotSupported:
        return tr("Not supported");
    case KErrOverflow:
        return tr("Overflow");
    case KErrUnderflow:
        return tr("Underflow");
    case KErrAlreadyExists:
        return tr("Already exists");
    case KErrPathNotFound:
        return tr("Path not found");
    case KErrInUse:
        return tr("In use");
    case KErrNotReady:
        return tr("Not ready");
    case KErrAccessDenied:
        return tr("Access denied");
    case KErrCouldNotConnect:
        return tr("Could not connect");
    case KErrDisconnected:
        return tr("Disconnected");
    case KErrPermissionDenied:
        return tr("Permission denied");

    // Multimedia framework errors
    case KErrMMNotEnoughBandwidth:
        return tr("Insufficient bandwidth");
    case KErrMMSocketServiceNotFound:
    case KErrMMServerSocket:
        return tr("Network unavailable");
    case KErrMMNetworkRead:
    case KErrMMNetworkWrite:
    case KErrMMUDPReceive:
        return tr("Network communication error");
    case KErrMMServerNotSupported:
        return tr("Streaming not supported");
    case KErrMMServerAlert:
        return tr("Server alert");
    case KErrMMInvalidProtocol:
        return tr("Invalid protocol");
    case KErrMMInvalidURL:
        return tr("Invalid URL");
    case KErrMMMulticast:
        return tr("Multicast error");
    case KErrMMProxyServer:
    case KErrMMProxyServerConnect:
        return tr("Proxy server error");
    case KErrMMProxyServerNotSupported:
        return tr("Proxy server not supported");
    case KErrMMAudioDevice:
        return tr("Audio output error");
    case KErrMMVideoDevice:
        return tr("Video output error");
    case KErrMMDecoder:
        return tr("Decoder error");
    case KErrMMPartialPlayback:
        return tr("Audio or video components could not be played");
    case KErrMMDRMNotAuthorized:
        return tr("DRM error");

    /*
    // We don't use QoS settings
    case KErrMMQosLowBandwidth:
    case KErrMMQosUnsupportedTrafficClass:
    case KErrMMQosPoorTrafficClass:
    case KErrMMQosUnsupportedParameters:
    case KErrMMQosPoorParameters:
    case KErrMMQosNotSupported:
    */

    // Catch-all for errors other than those above
    default:
    {
        return tr("Unknown error (%1)").arg(errorCode);
    }
    }
}

#ifndef QT_NO_DEBUG

#include <hal.h>
#include <hal_data.h>
#include <gdi.h>
#include <eikenv.h>

struct TScreenInfo
{
    int width;
    int height;
    int bpp;
    const char* address;
    int initialOffset;
    int lineOffset;
    TDisplayMode displayMode;
};

static void getScreenInfoL(TScreenInfo& info)
{
    info.displayMode = CEikonEnv::Static()->ScreenDevice()->DisplayMode();

    // Then we must set these as the input parameter
    info.width = info.displayMode;
    info.height = info.displayMode;
    info.initialOffset = info.displayMode;
    info.lineOffset = info.displayMode;
    info.bpp = info.displayMode;

    User::LeaveIfError( HAL::Get(HALData::EDisplayXPixels, info.width) );
    User::LeaveIfError( HAL::Get(HALData::EDisplayYPixels, info.width) );

    int address;
    User::LeaveIfError( HAL::Get(HALData::EDisplayMemoryAddress, address) );
    info.address = reinterpret_cast<const char*>(address);

    User::LeaveIfError( HAL::Get(HALData::EDisplayOffsetToFirstPixel, info.initialOffset) );

    User::LeaveIfError( HAL::Get(HALData::EDisplayOffsetBetweenLines, info.lineOffset) );

    User::LeaveIfError( HAL::Get(HALData::EDisplayBitsPerPixel, info.bpp) );
}


QColor MMF::Utils::getScreenPixel(const QPoint& pos)
{
    TScreenInfo info;
    TRAPD(err, getScreenInfoL(info));
    QColor pixel;
    if (err == KErrNone and pos.x() < info.width and pos.y() < info.height)
    {
        const int bytesPerPixel = info.bpp / 8;
        Q_ASSERT(bytesPerPixel >= 3);

        const int stride = (info.width * bytesPerPixel) + info.lineOffset;

        const char* ptr =
                info.address
            +    info.initialOffset
            +    pos.y() * stride
            +    pos.x() * bytesPerPixel;

        // BGRA
        pixel.setBlue(*ptr++);
        pixel.setGreen(*ptr++);
        pixel.setRed(*ptr++);

        if (bytesPerPixel == 4)
            pixel.setAlpha(*ptr++);
    }
    return pixel;
}

// Debugging: for debugging video visibility
void MMF::Utils::dumpScreenPixelSample()
{
    for (int i=0; i<20; ++i) {
        const QPoint pos(i*10, i*10);
        const QColor pixel = Utils::getScreenPixel(pos);
        RDebug::Printf(
            "Phonon::MMF::Utils::dumpScreenPixelSample %d %d = %d %d %d %d",
            pos.x(), pos.y(), pixel.red(), pixel.green(), pixel.blue(), pixel.alpha()
        );
    }
}

#endif // _DEBUG

QT_END_NAMESPACE


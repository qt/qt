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

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

_LIT(PanicCategory, "Phonon::MMF");

void MMF::Utils::panic(PanicCode code)
{
    User::Panic(PanicCategory, code);
}

QHBufC MMF::Utils::symbianFilename(const QString& qtFilename)
{
    _LIT(ForwardSlash, "/");
    _LIT(BackwardSlash, "\\");

    QHBufC result(qtFilename);
    TInt pos = result->Find(ForwardSlash);
    while(pos != KErrNotFound)
    {
        result->Des().Replace(pos, 1, BackwardSlash);
        pos = result->Find(ForwardSlash);
    }

    return result;
}


static const TInt KMimePrefixLength = 6; // either "audio/" or "video/"
_LIT(KMimePrefixAudio, "audio/");
_LIT(KMimePrefixVideo, "video/");

MMF::MediaType MMF::Utils::mimeTypeToMediaType(const TDesC& mimeType)
{
	MediaType result = MediaTypeUnknown;

	if(mimeType.Left(KMimePrefixLength).Compare(KMimePrefixAudio) == 0)
	{
		result = MediaTypeAudio;
	}
	else if(mimeType.Left(KMimePrefixLength).Compare(KMimePrefixVideo) == 0)
	{
		result = MediaTypeVideo;
	}

	return result;
}


QT_END_NAMESPACE


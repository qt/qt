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
#include "videooutput.h"

using namespace Phonon;
using namespace Phonon::MMF;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoOutput::VideoOutput(QWidget* parent)
					:	QWidget(parent)
{
	TRACE_CONTEXT(VideoOutput::VideoOutput, EVideoInternal);
	TRACE_ENTRY("parent 0x%08x", parent);
	
	TRACE_EXIT_0();
}

MMF::VideoOutput::~VideoOutput()
{
	TRACE_CONTEXT(VideoOutput::~VideoOutput, EVideoInternal);
	TRACE_ENTRY_0();
	
	TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------


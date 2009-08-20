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

#include "videowidget.h"


using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

static const Phonon::VideoWidget::AspectRatio DefaultAspectRatio = 
										Phonon::VideoWidget::AspectRatioAuto;
static const qreal DefaultBrightness = 1.0;
static const Phonon::VideoWidget::ScaleMode DefaultScaleMode = 
										Phonon::VideoWidget::FitInView;
static const qreal DefaultContrast = 1.0;
static const qreal DefaultHue = 1.0;
static const qreal DefaultSaturation = 1.0;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoWidget::VideoWidget()
					:	m_aspectRatio(DefaultAspectRatio)
					,	m_brightness(DefaultBrightness)
					,	m_scaleMode(DefaultScaleMode)
					,	m_contrast(DefaultContrast)
					,	m_hue(DefaultHue)
					,	m_saturation(DefaultSaturation)
{

}

MMF::VideoWidget::~VideoWidget()
{

}


//-----------------------------------------------------------------------------
// VideoWidgetInterface
//-----------------------------------------------------------------------------

Phonon::VideoWidget::AspectRatio MMF::VideoWidget::aspectRatio() const
{
	return m_aspectRatio;
}

void MMF::VideoWidget::setAspectRatio
	(Phonon::VideoWidget::AspectRatio aspectRatio)
{
	m_aspectRatio = aspectRatio;
}

qreal MMF::VideoWidget::brightness() const
{
	return m_brightness;
}

void MMF::VideoWidget::setBrightness(qreal brightness)
{
	m_brightness = brightness;
}

Phonon::VideoWidget::ScaleMode MMF::VideoWidget::scaleMode() const
{
	return m_scaleMode;
}

void MMF::VideoWidget::setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode)
{
	m_scaleMode = scaleMode;
}

qreal MMF::VideoWidget::contrast() const
{
	return m_contrast;
}

void MMF::VideoWidget::setContrast(qreal contrast)
{
	m_contrast = contrast;
}

qreal MMF::VideoWidget::hue() const
{
	return m_hue;
}

void MMF::VideoWidget::setHue(qreal hue)
{
	m_hue = hue;
}

qreal MMF::VideoWidget::saturation() const
{
	return m_saturation;
}

void MMF::VideoWidget::setSaturation(qreal saturation)
{
	m_saturation = saturation;
}

QWidget* MMF::VideoWidget::widget()
{
	return this;
}



/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "vmr9videowindowcontrol.h"

#include "directshowglobal.h"


QT_BEGIN_NAMESPACE

Vmr9VideoWindowControl::Vmr9VideoWindowControl(QObject *parent)
    : QVideoWindowControl(parent)
    , m_filter(com_new<IBaseFilter>(CLSID_VideoMixingRenderer9, IID_IBaseFilter))
    , m_windowId(0)
    , m_dirtyValues(0)
    , m_brightness(0)
    , m_contrast(0)
    , m_hue(0)
    , m_saturation(0)
    , m_fullScreen(false)
{
    if (IVMRFilterConfig9 *config = com_cast<IVMRFilterConfig9>(m_filter, IID_IVMRFilterConfig9)) {
        config->SetRenderingMode(VMR9Mode_Windowless);
        config->SetNumberOfStreams(1);
        config->Release();
    }
}

Vmr9VideoWindowControl::~Vmr9VideoWindowControl()
{
    if (m_filter)
        m_filter->Release();
}


WId Vmr9VideoWindowControl::winId() const
{
    return m_windowId;

}

void Vmr9VideoWindowControl::setWinId(WId id)
{
    m_windowId = id;

    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(
            m_filter, IID_IVMRWindowlessControl9)) {
        control->SetVideoClippingWindow(m_windowId);
        control->Release();
    }
}

QRect Vmr9VideoWindowControl::displayRect() const
{
    QRect rect;

    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(
            m_filter, IID_IVMRWindowlessControl9)) {
        RECT sourceRect;
        RECT displayRect;

        if (control->GetVideoPosition(&sourceRect, &displayRect) == S_OK) {
            rect = QRect(
                    displayRect.left,
                    displayRect.bottom,
                    displayRect.right - displayRect.left,
                    displayRect.bottom - displayRect.top);
        }
        control->Release();
    }
    return rect;
}

void Vmr9VideoWindowControl::setDisplayRect(const QRect &rect)
{
    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(
            m_filter, IID_IVMRWindowlessControl9)) {
        RECT sourceRect = { 0, 0, 0, 0 };
        RECT displayRect = { rect.left(), rect.top(), rect.right(), rect.bottom() };

        control->GetNativeVideoSize(&sourceRect.right, &sourceRect.bottom, 0, 0);
        control->SetVideoPosition(&sourceRect, &displayRect);
        control->Release();
    }
}

bool Vmr9VideoWindowControl::isFullScreen() const
{
    return m_fullScreen;
}

void Vmr9VideoWindowControl::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(m_fullScreen = fullScreen);
}

void Vmr9VideoWindowControl::repaint()
{

    if (QWidget *widget = QWidget::find(m_windowId)) {
        HDC dc = widget->getDC();
        if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(
                m_filter, IID_IVMRWindowlessControl9)) {
            control->RepaintVideo(m_windowId, dc);
            control->Release();
        }
        widget->releaseDC(dc);
    }
}

QSize Vmr9VideoWindowControl::nativeSize() const
{
    QSize size;

    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(
            m_filter, IID_IVMRWindowlessControl9)) {
        LONG width;
        LONG height;

        if (control->GetNativeVideoSize(&width, &height, 0, 0) == S_OK)
            size = QSize(width, height);
        control->Release();
    }
    return size;
}

QVideoWidget::AspectRatioMode Vmr9VideoWindowControl::aspectRatioMode() const
{
    QVideoWidget::AspectRatioMode mode = QVideoWidget::KeepAspectRatio;

    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(
            m_filter, IID_IVMRWindowlessControl9)) {
        DWORD arMode;

        if (control->GetAspectRatioMode(&arMode) == S_OK && arMode == VMR9ARMode_None)
            mode = QVideoWidget::IgnoreAspectRatio;
        control->Release();
    }
    return mode;
}

void Vmr9VideoWindowControl::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
    if (IVMRWindowlessControl9 *control = com_cast<IVMRWindowlessControl9>(
            m_filter, IID_IVMRWindowlessControl9)) {
        switch (mode) {
        case QVideoWidget::IgnoreAspectRatio:
            control->SetAspectRatioMode(VMR9ARMode_None);
            break;
        case QVideoWidget::KeepAspectRatio:
            control->SetAspectRatioMode(VMR9ARMode_LetterBox);
            break;
        default:
            break;
        }
        control->Release();
    }
}

int Vmr9VideoWindowControl::brightness() const
{
    return m_brightness;
}

void Vmr9VideoWindowControl::setBrightness(int brightness)
{
    m_brightness = brightness;

    m_dirtyValues |= ProcAmpControl9_Brightness;

    setProcAmpValues();

    emit brightnessChanged(brightness);
}

int Vmr9VideoWindowControl::contrast() const
{
    return m_contrast;
}

void Vmr9VideoWindowControl::setContrast(int contrast)
{
    m_contrast = contrast;

    m_dirtyValues |= ProcAmpControl9_Contrast;

    setProcAmpValues();

    emit contrastChanged(contrast);
}

int Vmr9VideoWindowControl::hue() const
{
    return m_hue;
}

void Vmr9VideoWindowControl::setHue(int hue)
{
    m_hue = hue;

    m_dirtyValues |= ProcAmpControl9_Hue;

    setProcAmpValues();

    emit hueChanged(hue);
}

int Vmr9VideoWindowControl::saturation() const
{
    return m_saturation;
}

void Vmr9VideoWindowControl::setSaturation(int saturation)
{
    m_saturation = saturation;

    m_dirtyValues |= ProcAmpControl9_Saturation;

    setProcAmpValues();

    emit saturationChanged(saturation);
}

void Vmr9VideoWindowControl::setProcAmpValues()
{
    if (IVMRMixerControl9 *control = com_cast<IVMRMixerControl9>(m_filter, IID_IVMRMixerControl9)) {
        VMR9ProcAmpControl procAmp;
        procAmp.dwSize = sizeof(VMR9ProcAmpControl);
        procAmp.dwFlags = m_dirtyValues;

        if (m_dirtyValues & ProcAmpControl9_Brightness) {
            procAmp.Brightness = scaleProcAmpValue(
                    control, ProcAmpControl9_Brightness, m_brightness);
        }
        if (m_dirtyValues & ProcAmpControl9_Contrast) {
            procAmp.Contrast = scaleProcAmpValue(
                    control, ProcAmpControl9_Contrast, m_contrast);
        }
        if (m_dirtyValues & ProcAmpControl9_Hue) {
            procAmp.Hue = scaleProcAmpValue(
                    control, ProcAmpControl9_Hue, m_hue);
        }
        if (m_dirtyValues & ProcAmpControl9_Saturation) {
            procAmp.Saturation = scaleProcAmpValue(
                    control, ProcAmpControl9_Saturation, m_saturation);
        }

        if (SUCCEEDED(control->SetProcAmpControl(0, &procAmp))) {
            m_dirtyValues = 0;
        }

        control->Release();
    }
}

float Vmr9VideoWindowControl::scaleProcAmpValue(
        IVMRMixerControl9 *control, VMR9ProcAmpControlFlags property, int value) const
{
    float scaledValue = 0.0;

    VMR9ProcAmpControlRange range;
    range.dwSize = sizeof(VMR9ProcAmpControlRange);
    range.dwProperty = property;

    if (SUCCEEDED(control->GetProcAmpControlRange(0, &range))) {
        scaledValue = range.DefaultValue;
        if (value > 0)
            scaledValue += float(value) * (range.MaxValue - range.DefaultValue) / 100;
        else if (value < 0)
            scaledValue -= float(value) * (range.MinValue - range.DefaultValue) / 100;
    }

    return scaledValue;
}

QT_END_NAMESPACE


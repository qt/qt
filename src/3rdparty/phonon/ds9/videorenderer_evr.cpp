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


#include "videorenderer_evr.h"

#ifndef QT_NO_PHONON_VIDEO

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QTimerEvent>

#include <d3d9.h>
#include <Evr9.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        VideoRendererEVR::~VideoRendererEVR()
        {
        }

        bool VideoRendererEVR::isNative() const
        {
            return true;
        }

        VideoRendererEVR::VideoRendererEVR(QWidget *target) : m_target(target)
        {
            m_filter = Filter(CLSID_EnhancedVideoRenderer, IID_IBaseFilter);
            if (!m_filter) {
                qWarning("the video widget could not be initialized correctly");
                return;
            }

            ComPointer<IMFVideoDisplayControl> filterControl = getService<IMFVideoDisplayControl>(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl);

            filterControl->SetVideoWindow(reinterpret_cast<HWND>(target->winId()));
            filterControl->SetAspectRatioMode(MFVideoARMode_None); // We're in control of the size
        }

        QImage VideoRendererEVR::snapshot() const
        {
            // TODO test
            // This will always capture black areas where no video is drawn, if any are present.
            // Due to the hack in notifyResize()
            ComPointer<IMFVideoDisplayControl> filterControl = getService<IMFVideoDisplayControl>(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl);
            if (filterControl) {
                BITMAPINFOHEADER bmi;
                BYTE *buffer = 0;
                DWORD bufferSize;
                LONGLONG timeStamp;

                bmi.biSize = sizeof(BITMAPINFOHEADER);

                HRESULT hr = filterControl->GetCurrentImage(&bmi, &buffer, &bufferSize, &timeStamp);
                if (SUCCEEDED(hr)) {

                    const int w = qAbs(bmi.biWidth),
                        h = qAbs(bmi.biHeight);

                    // Create image and copy data into image.
                    QImage ret(w, h, QImage::Format_RGB32);

                    if (!ret.isNull()) {
                        uchar *data = buffer;
                        const int bytes_per_line = w * sizeof(QRgb);
                        for (int y = h - 1; y >= 0; --y) {
                            qMemCopy(ret.scanLine(y), //destination
                                data,     //source
                                bytes_per_line);
                            data += bytes_per_line;
                        }
                    }
                    ::CoTaskMemFree(buffer);
                    return ret;
                }
            }
            return QImage();
        }

        QSize VideoRendererEVR::videoSize() const
        {
            SIZE nativeSize;
            SIZE aspectRatioSize;

            ComPointer<IMFVideoDisplayControl> filterControl = getService<IMFVideoDisplayControl>(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl);

            filterControl->GetNativeVideoSize(&nativeSize, &aspectRatioSize);

            return QSize(nativeSize.cx, nativeSize.cy);
        }
       
        void VideoRendererEVR::repaintCurrentFrame(QWidget *target, const QRect &rect)
        {
            // repaint the video
            ComPointer<IMFVideoDisplayControl> filterControl = getService<IMFVideoDisplayControl>(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl);
            // All failed results can be safely ignored
            filterControl->RepaintVideo();
        }

        void VideoRendererEVR::notifyResize(const QSize &size, Phonon::VideoWidget::AspectRatio aspectRatio,
            Phonon::VideoWidget::ScaleMode scaleMode)
        {
            if (!isActive()) {
                RECT dummyRect = { 0, 0, 0, 0};
                ComPointer<IMFVideoDisplayControl> filterControl = getService<IMFVideoDisplayControl>(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl);
                filterControl->SetVideoPosition(0, &dummyRect);
                return;
            }

            const QSize vsize = videoSize();
            internalNotifyResize(size, vsize, aspectRatio, scaleMode);

            RECT dstRectWin = { 0, 0, size.width(), size.height()};

            // Resize the Stream output rect instead of the destination rect.
            // Hacky workaround for flicker in the areas outside of the destination rect
            // This way these areas don't exist
            MFVideoNormalizedRect streamOutputRect = { float(m_dstX) / float(size.width()), float(m_dstY) / float(size.height()),
                                                       float(m_dstWidth + m_dstX) / float(size.width()), float(m_dstHeight + m_dstY) / float(size.height())};

            ComPointer<IMFVideoMixerControl> filterMixer = getService<IMFVideoMixerControl>(MR_VIDEO_MIXER_SERVICE, IID_IMFVideoMixerControl);
            ComPointer<IMFVideoDisplayControl> filterControl = getService<IMFVideoDisplayControl>(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl);

            filterMixer->SetStreamOutputRect(0, &streamOutputRect);
            filterControl->SetVideoPosition(0, &dstRectWin);
        }

        void VideoRendererEVR::applyMixerSettings(qreal brightness, qreal contrast, qreal hue, qreal saturation)
        {
            InputPin sink = BackendNode::pins(m_filter, PINDIR_INPUT).first();
            OutputPin source;
            if (FAILED(sink->ConnectedTo(source.pparam()))) {
                return; //it must be connected to work
            }

            // Get the "Video Processor" (used for brightness/contrast/saturation/hue)
            ComPointer<IMFVideoProcessor> processor = getService<IMFVideoProcessor>(MR_VIDEO_MIXER_SERVICE, IID_IMFVideoProcessor);
            Q_ASSERT(processor);

            DXVA2_ValueRange contrastRange;
            DXVA2_ValueRange brightnessRange;
            DXVA2_ValueRange saturationRange;
            DXVA2_ValueRange hueRange;

            if (FAILED(processor->GetProcAmpRange(DXVA2_ProcAmp_Contrast, &contrastRange)))
                return;
            if (FAILED(processor->GetProcAmpRange(DXVA2_ProcAmp_Brightness, &brightnessRange)))
                return;
            if (FAILED(processor->GetProcAmpRange(DXVA2_ProcAmp_Saturation, &saturationRange)))
                return;
            if (FAILED(processor->GetProcAmpRange(DXVA2_ProcAmp_Hue, &hueRange)))
                return;

            DXVA2_ProcAmpValues values;

            values.Contrast = DXVA2FloatToFixed(((contrast < 0
                                ? DXVA2FixedToFloat(contrastRange.MinValue) : DXVA2FixedToFloat(contrastRange.MaxValue))
                               - DXVA2FixedToFloat(contrastRange.DefaultValue)) * qAbs(contrast) + DXVA2FixedToFloat(contrastRange.DefaultValue));
            values.Brightness = DXVA2FloatToFixed(((brightness < 0
                                ? DXVA2FixedToFloat(brightnessRange.MinValue) : DXVA2FixedToFloat(brightnessRange.MaxValue))
                               - DXVA2FixedToFloat(brightnessRange.DefaultValue)) * qAbs(brightness) + DXVA2FixedToFloat(brightnessRange.DefaultValue));
            values.Saturation = DXVA2FloatToFixed(((saturation < 0
                                ? DXVA2FixedToFloat(saturationRange.MinValue) : DXVA2FixedToFloat(saturationRange.MaxValue))
                               - DXVA2FixedToFloat(saturationRange.DefaultValue)) * qAbs(saturation) + DXVA2FixedToFloat(saturationRange.DefaultValue));
            values.Hue = DXVA2FloatToFixed(((hue < 0
                                ? DXVA2FixedToFloat(hueRange.MinValue) : DXVA2FixedToFloat(hueRange.MaxValue))
                               - DXVA2FixedToFloat(hueRange.DefaultValue)) * qAbs(hue) + DXVA2FixedToFloat(hueRange.DefaultValue));

            //finally set the settings
            processor->SetProcAmpValues(DXVA2_ProcAmp_Contrast | DXVA2_ProcAmp_Brightness | DXVA2_ProcAmp_Saturation | DXVA2_ProcAmp_Hue, &values);

        }
    }
}

QT_END_NAMESPACE

#endif //QT_NO_PHONON_VIDEO

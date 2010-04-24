/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaServices module of the Qt Toolkit.
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

#ifndef QPAINTERVIDEOSURFACE_P_H
#define QPAINTERVIDEOSURFACE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qsize.h>
#include <QtGui/qimage.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qpaintengine.h>
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideoframe.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGLContext;

class QVideoSurfacePainter
{
public:
    virtual ~QVideoSurfacePainter();

    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType) const = 0;

    virtual bool isFormatSupported(
            const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const = 0;

    virtual QAbstractVideoSurface::Error start(const QVideoSurfaceFormat &format) = 0;
    virtual void stop() = 0;

    virtual QAbstractVideoSurface::Error setCurrentFrame(const QVideoFrame &frame) = 0;

    virtual QAbstractVideoSurface::Error paint(
            const QRectF &target, QPainter *painter, const QRectF &source) = 0;

    virtual void updateColors(int brightness, int contrast, int hue, int saturation) = 0;
};

class Q_MEDIASERVICES_EXPORT QPainterVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit QPainterVideoSurface(QObject *parent = 0);
    ~QPainterVideoSurface();

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    bool isFormatSupported(
            const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar = 0) const;

    bool start(const QVideoSurfaceFormat &format);
    void stop();

    bool present(const QVideoFrame &frame);

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);

    bool isReady() const;
    void setReady(bool ready);

    void paint(QPainter *painter, const QRectF &target, const QRectF &source = QRectF(0, 0, 1, 1));

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    const QGLContext *glContext() const;
    void setGLContext(QGLContext *context);

    enum ShaderType
    {
        NoShaders = 0x00,
        FragmentProgramShader = 0x01,
        GlslShader = 0x02
    };

    Q_DECLARE_FLAGS(ShaderTypes, ShaderType)

    ShaderTypes supportedShaderTypes() const;

    ShaderType shaderType() const;
    void setShaderType(ShaderType type);
#endif

Q_SIGNALS:
    void frameChanged();

private:
    void createPainter();

    QVideoSurfacePainter *m_painter;
#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    QGLContext *m_glContext;
    ShaderTypes m_shaderTypes;
    ShaderType m_shaderType;
#endif
    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;

    QVideoFrame::PixelFormat m_pixelFormat;
    QSize m_frameSize;
    QRect m_sourceRect;
    bool m_colorsDirty;
    bool m_ready;
};

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
Q_DECLARE_OPERATORS_FOR_FLAGS(QPainterVideoSurface::ShaderTypes)
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif

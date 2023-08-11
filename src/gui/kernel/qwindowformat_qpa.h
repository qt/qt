/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QPLATFORMWINDOWFORMAT_QPA_H
#define QPLATFORMWINDOWFORMAT_QPA_H

#include <QtGui/QPlatformWindow>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QWindowContext;
class QWindowFormatPrivate;

class Q_GUI_EXPORT QWindowFormat
{
public:
    enum FormatOption {
        StereoBuffers           = 0x0001,
        WindowSurface           = 0x0002
    };
    Q_DECLARE_FLAGS(FormatOptions, FormatOption)

    enum SwapBehavior {
        DefaultSwapBehavior,
        SingleBuffer,
        DoubleBuffer,
        TripleBuffer
    };

    enum OpenGLContextProfile {
        NoProfile,
        CoreProfile,
        CompatibilityProfile
    };

    QWindowFormat();
    QWindowFormat(FormatOptions options);
    QWindowFormat(const QWindowFormat &other);
    QWindowFormat &operator=(const QWindowFormat &other);
    ~QWindowFormat();

    void setDepthBufferSize(int size);
    int depthBufferSize() const;

    void setStencilBufferSize(int size);
    int stencilBufferSize() const;

    void setRedBufferSize(int size);
    int redBufferSize() const;
    void setGreenBufferSize(int size);
    int greenBufferSize() const;
    void setBlueBufferSize(int size);
    int blueBufferSize() const;
    void setAlphaBufferSize(int size);
    int alphaBufferSize() const;

    void setSamples(int numSamples);
    int samples() const;

    void setSwapBehavior(SwapBehavior behavior);
    SwapBehavior swapBehavior() const;

    bool hasAlpha() const;

    void setProfile(OpenGLContextProfile profile);
    OpenGLContextProfile profile() const;

    void setSharedContext(QWindowContext *context);
    QWindowContext *sharedContext() const;

    bool stereo() const;
    void setStereo(bool enable);
    bool windowSurface() const;
    void setWindowSurface(bool enable);

    void setOption(QWindowFormat::FormatOptions opt);
    bool testOption(QWindowFormat::FormatOptions opt) const;

private:
    QWindowFormatPrivate *d;

    void detach();

    friend Q_GUI_EXPORT bool operator==(const QWindowFormat&, const QWindowFormat&);
    friend Q_GUI_EXPORT bool operator!=(const QWindowFormat&, const QWindowFormat&);
#ifndef QT_NO_DEBUG_STREAM
    friend Q_GUI_EXPORT QDebug operator<<(QDebug, const QWindowFormat &);
#endif
};

Q_GUI_EXPORT bool operator==(const QWindowFormat&, const QWindowFormat&);
Q_GUI_EXPORT bool operator!=(const QWindowFormat&, const QWindowFormat&);

#ifndef QT_NO_DEBUG_STREAM
Q_OPENGL_EXPORT QDebug operator<<(QDebug, const QWindowFormat &);
#endif

Q_DECLARE_OPERATORS_FOR_FLAGS(QWindowFormat::FormatOptions)

inline bool QWindowFormat::stereo() const
{
    return testOption(QWindowFormat::StereoBuffers);
}

inline bool QWindowFormat::windowSurface() const
{
    return testOption(QWindowFormat::WindowSurface);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif //QPLATFORMWINDOWFORMAT_QPA_H

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools module of the Qt Toolkit.
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

#ifndef QVFBX11VIEW_H
#define QVFBX11VIEW_H

#include "qvfbview.h"

QT_BEGIN_NAMESPACE

class X11KeyFaker;
class QProcess;
class QTemporaryFile;

class QVFbX11View : public QVFbAbstractView
{
    Q_OBJECT
public:
    QVFbX11View( int id, int w, int h, int d, Rotation r, QWidget *parent = 0);
    virtual ~QVFbX11View();

    QString xServerPath() const { return xserver; }
    void setXServerPath(const QString& path) { xserver = path; }

    int displayId() const;
    int displayWidth() const;
    int displayHeight() const;
    int displayDepth() const;
    Rotation displayRotation() const;

    void skinKeyPressEvent( int code, const QString& text, bool autorep=FALSE );
    void skinKeyReleaseEvent( int code, const QString& text, bool autorep=FALSE );

    void setGamma(double gr, double gg, double gb);
    double gammaRed() const;
    double gammaGreen() const;
    double gammaBlue() const;
    void getGamma(int i, QRgb& rgb);

    bool touchScreenEmulation() const;
    bool lcdScreenEmulation() const;
    int rate();
    bool animating() const;
    QImage image() const;
    void setRate(int);

    double zoomH() const;
    double zoomV() const;

    QSize sizeHint() const;

public slots:
    void setTouchscreenEmulation( bool );
    void setLcdScreenEmulation( bool );
    void setZoom( double, double );
    void setRotation(Rotation);
    void startAnimation( const QString& );
    void stopAnimation();

protected:
    void showEvent(QShowEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

private slots:
    void startXnest();
    void xnestStopped();
    void startKeyFaker();

private:
    int id, w, h, d;
    Rotation rotation;
    double gr, gg, gb;
    bool touchscreen, lcd;
    X11KeyFaker *keyFaker;
    QProcess *xnest;
    QTemporaryFile *serverAuthFile;
    bool shutdown;
    QString xserver;
};

QT_END_NAMESPACE

#endif

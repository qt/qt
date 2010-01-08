/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools module of the Qt Toolkit.
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

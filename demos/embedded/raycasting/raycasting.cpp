/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <QtGui>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WORLD_SIZE 8
int world_map[WORLD_SIZE][WORLD_SIZE] = {
    { 1, 1, 1, 1, 6, 1, 1, 1 },
    { 1, 0, 0, 1, 0, 0, 0, 7 },
    { 1, 1, 0, 1, 0, 1, 1, 1 },
    { 6, 0, 0, 0, 0, 0, 0, 3 },
    { 1, 8, 8, 0, 8, 0, 8, 1 },
    { 2, 2, 0, 0, 8, 8, 7, 1 },
    { 3, 0, 0, 0, 0, 0, 0, 5 },
    { 2, 2, 2, 2, 7, 4, 4, 4 },
};

#define TEXTURE_SIZE 64
#define TEXTURE_BLOCK (TEXTURE_SIZE * TEXTURE_SIZE)

class Raycasting: public QWidget
{
public:
    Raycasting(QWidget *parent = 0)
            : QWidget(parent)
            , angle(0.5)
            , playerPos(1.5, 1.5)
            , angleDelta(0)
            , moveDelta(0) {

        // http://www.areyep.com/RIPandMCS-TextureLibrary.html
        textureImg.load(":/textures.png");
        textureImg = textureImg.convertToFormat(QImage::Format_ARGB32);
        Q_ASSERT(textureImg.width() == TEXTURE_SIZE * 2);
        Q_ASSERT(textureImg.bytesPerLine() == 4 * TEXTURE_SIZE * 2);
        textureCount = textureImg.height() / TEXTURE_SIZE;

        watch.start();
        ticker.start(25, this);
        setAttribute(Qt::WA_OpaquePaintEvent, true);
    }

    void updatePlayer() {
        int interval = qBound(20, watch.elapsed(), 250);
        watch.start();
        angle += angleDelta * interval / 1000;
        qreal step = moveDelta * interval / 1000;
        qreal dx = cos(angle) * step;
        qreal dy = sin(angle) * step;
        QPointF pos = playerPos + 3 * QPointF(dx, dy);
        int xi = static_cast<int>(pos.x());
        int yi = static_cast<int>(pos.y());
        if (world_map[yi][xi] == 0)
            playerPos = playerPos + QPointF(dx, dy);
    }

    void showFps() {
        static QTime frameTick;
        static int totalFrame = 0;
        if (!(totalFrame & 31)) {
            int elapsed = frameTick.elapsed();
            frameTick.start();
            int fps = 32 * 1000 / (1 + elapsed);
            setWindowTitle(QString("Raycasting (%1 FPS)").arg(fps));
        }
        totalFrame++;
    }

    void render() {

        // setup the screen surface
        if (buffer.size() != size())
            buffer = QImage(size(), QImage::Format_ARGB32);
        int bufw = buffer.width();
        int bufh = buffer.height();

        // we intentionally cheat here, to avoid detach
        const uchar *ptr = buffer.bits();
        QRgb *start = (QRgb*)(ptr);
        QRgb stride = buffer.bytesPerLine() / 4;
        QRgb *finish = start + stride * bufh;

        // prepare the texture pointer
        const uchar *src = textureImg.bits();
        const QRgb *texsrc = reinterpret_cast<const QRgb*>(src);

        // cast all rays here
        qreal sina = sin(angle);
        qreal cosa = cos(angle);
        qreal u = cosa - sina;
        qreal v = sina + cosa;
        qreal du = 2 * sina / bufw;
        qreal dv = -2 * cosa / bufw;

        for (int ray = 0; ray < bufw; ++ray, u += du, v += dv) {
            // everytime this ray advances 'u' units in x direction,
            // it also advanced 'v' units in y direction
            qreal uu = (u < 0) ? -u : u;
            qreal vv = (v < 0) ? -v : v;
            qreal duu = 1 / uu;
            qreal dvv = 1 / vv;
            int stepx = (u < 0) ? -1 : 1;
            int stepy = (v < 0) ? -1 : 1;

            // the cell in the map that we need to check
            qreal px = playerPos.x();
            qreal py = playerPos.y();
            int mapx = static_cast<int>(px);
            int mapy = static_cast<int>(py);

            // the position and texture for the hit
            int texture = 0;
            qreal hitdist = 0.1;
            qreal texofs = 0;
            bool dark = false;

            // first hit at constant x and constant y lines
            qreal distx = (u > 0) ? (mapx + 1 - px) * duu : (px - mapx) * duu;
            qreal disty = (v > 0) ? (mapy + 1 - py) * dvv : (py - mapy) * dvv;

            // loop until we hit something
            while (texture <= 0) {
                if (distx > disty) {
                    // shorter distance to a hit in constant y line
                    hitdist = disty;
                    disty += dvv;
                    mapy += stepy;
                    texture = world_map[mapy][mapx];
                    if (texture > 0) {
                        dark = true;
                        if (stepy > 0) {
                            qreal ofs = px + u * (mapy - py) / v;
                            texofs = ofs - floor(ofs);
                        } else {
                            qreal ofs = px + u * (mapy + 1 - py) / v;
                            texofs = ofs - floor(ofs);
                        }
                    }
                } else {
                    // shorter distance to a hit in constant x line
                    hitdist = distx;
                    distx += duu;
                    mapx += stepx;
                    texture = world_map[mapy][mapx];
                    if (texture > 0) {
                        if (stepx > 0) {
                            qreal ofs = py + v * (mapx - px) / u;
                            texofs = ofs - floor(ofs);
                        } else {
                            qreal ofs = py + v * (mapx + 1 - px) / u;
                            texofs = ceil(ofs) - ofs;
                        }
                    }
                }
            }

            // get the texture, note that the texture image
            // has two textures horizontally, "normal" vs "dark"
            int col = static_cast<int>(texofs * TEXTURE_SIZE);
            col = qBound(0, col, TEXTURE_SIZE - 1);
            texture = (texture - 1) % textureCount;
            const QRgb *tex = texsrc + TEXTURE_BLOCK * texture * 2 +
                              (TEXTURE_SIZE * 2 * col);
            if (dark)
                tex += TEXTURE_SIZE;

            // start from the texture center (horizontally)
            int h = static_cast<int>(bufw / hitdist / 2);
            int dy = (TEXTURE_SIZE << 12) / h;
            int p1 = ((TEXTURE_SIZE / 2) << 12) - dy;
            int p2 = p1 + dy;

            // start from the screen center (vertically)
            // y1 will go up (decrease), y2 will go down (increase)
            int y1 = bufh / 2;
            int y2 = y1 + 1;
            QRgb *pixel1 = start + y1 * stride + ray;
            QRgb *pixel2 = pixel1 + stride;

            // map the texture to the sliver
            while (y1 >= 0 && y2 < bufh && p1 >= 0) {
                *pixel1 = tex[p1 >> 12];
                *pixel2 = tex[p2 >> 12];
                p1 -= dy;
                p2 += dy;
                --y1;
                ++y2;
                pixel1 -= stride;
                pixel2 += stride;
            }

            // ceiling and floor
            for (; pixel1 > start; pixel1 -= stride)
                *pixel1 = qRgb(0, 0, 0);
            for (; pixel2 < finish; pixel2 += stride)
                *pixel2 = qRgb(96, 96, 96);
        }

        update();
    }

protected:

    void timerEvent(QTimerEvent*) {
        updatePlayer();
        render();
        showFps();
    }

    void paintEvent(QPaintEvent *event) {
        QPainter p(this);
        p.drawImage(event->rect(), buffer, event->rect());
    }

    void keyPressEvent(QKeyEvent *event) {
        event->accept();
        if (event->key() == Qt::Key_Left)
            angleDelta = 1.3 * M_PI;
        if (event->key() == Qt::Key_Right)
            angleDelta = -1.3 * M_PI;
        if (event->key() == Qt::Key_Up)
            moveDelta = 2.5;
        if (event->key() == Qt::Key_Down)
            moveDelta = -2.5;
    }

    void keyReleaseEvent(QKeyEvent *event) {
        event->accept();
        if (event->key() == Qt::Key_Left)
            angleDelta = (angleDelta > 0) ? 0 : angleDelta;
        if (event->key() == Qt::Key_Right)
            angleDelta = (angleDelta < 0) ? 0 : angleDelta;
        if (event->key() == Qt::Key_Up)
            moveDelta = (moveDelta > 0) ? 0 : moveDelta;
        if (event->key() == Qt::Key_Down)
            moveDelta = (moveDelta < 0) ? 0 : moveDelta;
    }

private:
    QTime watch;
    QBasicTimer ticker;
    QImage buffer;
    qreal angle;
    QPointF playerPos;
    qreal angleDelta;
    qreal moveDelta;
    QImage textureImg;
    int textureCount;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Raycasting w;
    w.setWindowTitle("Raycasting");
#if defined(Q_OS_SYMBIAN)
    w.showMaximized();
#else
    w.resize(640, 480);
    w.show();
#endif

    return app.exec();
}

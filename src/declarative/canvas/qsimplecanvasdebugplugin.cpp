/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsimplecanvasdebugplugin_p.h"
#include "qdebug.h"
#include <QtCore/qabstractanimation.h>
#include <qsimplecanvas.h>
#include <qsimplecanvasitem.h>

QT_BEGIN_NAMESPACE

class FrameBreakAnimation : public QAbstractAnimation
{
public:
    FrameBreakAnimation(QSimpleCanvasDebugPlugin *s)
    : QAbstractAnimation(s), server(s) 
    {
        start();
    }

    virtual int duration() const { return -1; }
    virtual void updateCurrentTime(int msecs) {
        Q_UNUSED(msecs);
        server->frameBreak();
    }

private:
    QSimpleCanvasDebugPlugin *server;
};

QSimpleCanvasDebugPlugin::QSimpleCanvasDebugPlugin(QObject *parent)
: QmlDebugServerPlugin(QLatin1String("CanvasFrameRate"), parent), _breaks(0)
{
    _time.start();
    new FrameBreakAnimation(this);
}

void QSimpleCanvasDebugPlugin::addTiming(quint32 paint, 
                                         quint32 repaint, 
                                         quint32 timeBetweenFrames)
{
    if (!isEnabled())
        return;

    bool isFrameBreak = _breaks > 1;
    _breaks = 0;
    int e = _time.elapsed();
    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << (int)paint << (int)repaint << (int)timeBetweenFrames << (int)e 
       << (bool)isFrameBreak;
    sendMessage(data);
}

void QSimpleCanvasDebugPlugin::frameBreak()
{
    _breaks++;
}

QSimpleCanvasSceneDebugPlugin::QSimpleCanvasSceneDebugPlugin(QSimpleCanvas *parent)
: QmlDebugServerPlugin(QLatin1String("CanvasScene"), parent), m_canvas(parent)
{
}

void QSimpleCanvasSceneDebugPlugin::messageReceived(const QByteArray &)
{
    refresh();
}

void QSimpleCanvasSceneDebugPlugin::refresh()
{
    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    const QList<QSimpleCanvasItem *> &children = m_canvas->root()->children();
    ds << children.count();
    for (int ii = 0; ii < children.count(); ++ii)
        refresh(ds, children.at(ii));

    sendMessage(data);
}

void QSimpleCanvasSceneDebugPlugin::refresh(QDataStream &ds, 
                                            QSimpleCanvasItem *item)
{
    ds << QmlDebugServerPlugin::objectToString(item) << item->x() << item->y() 
       << item->z() << item->width() << item->height() 
       << (int)item->transformOrigin() << item->scale() << (int)item->flip() 
#ifdef QFX_RENDER_OPENGL
       << item->transform().toTransform()
#elif defined(QFX_RENDER_QPAINTER)
       << item->transform()
#endif
       << item->hasActiveFocus() << (int)item->options();

    QPixmap pix;

    if(item->options() & QSimpleCanvasItem::HasContents && 
       item->width() > 0 && item->height() > 0) {

        pix = QPixmap(item->width(), item->height());
        pix.fill(QColor(0,0,0,0));
        QPainter p(&pix);
        item->paintContents(p);

    }

    ds << pix;

    const QList<QSimpleCanvasItem *> &children = item->children();
    ds << children.count();

    for(int ii = 0; ii < children.count(); ++ii) 
        refresh(ds, children.at(ii));
}

QT_END_NAMESPACE


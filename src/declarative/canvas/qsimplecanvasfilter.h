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

#ifndef QSIMPLECANVASFILTER_H
#define QSIMPLECANVASFILTER_H

#include <QtCore/qobject.h>
#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qsimplecanvasitem.h>
#include <QtDeclarative/qsimplecanvas.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QSimpleCanvasFilterPrivate;
class QRectF;
class QPoint;
class QGLFramebufferObject;
class GLShaderProgram;
class Q_DECLARATIVE_EXPORT QSimpleCanvasFilter : public QObject
{
Q_OBJECT
public:
    QSimpleCanvasFilter(QObject *parent);
    virtual ~QSimpleCanvasFilter();

    enum Layer { ChildrenUnderItem = 0x01, 
                 Item = 0x02, 
                 ChildrenAboveItem = 0x04,
                 All = 0x07 };

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    bool enabled() const;
    void setEnabled(bool);

    QSimpleCanvasItem *item() const;
    void setItem(QSimpleCanvasItem *);
Q_SIGNALS:
    void enabledChanged();

protected:

    virtual QRectF itemBoundingRect(const QRectF &) const;
    virtual void filterGL(QSimpleCanvasItem::GLPainter &p);
    virtual void filter(QPainter &p);

    QGLFramebufferObject *renderToFBO(const QRectF &src = QRect(), const QPoint &offset = QPoint(), Layer = All);
    QGLFramebufferObject *renderToFBO(float scale, const QRectF &src = QRect(), const QPoint &offset = QPoint(), Layer = All);
    QGLFramebufferObject *acquireFBO(const QSize & = QSize());
    void releaseFBO(QGLFramebufferObject *);

    void renderToScreen(const QRectF &src = QRect(), Layer = All);
    void renderToScreen(const QSimpleCanvas::Matrix &trans, const QRectF &src = QRect(), Layer = All);

    void update();

    bool isSimpleItem(QSimpleCanvasItem **, QSimpleCanvas::Matrix *);

private:
    friend class QSimpleCanvasFilterPrivate;
    friend class QSimpleCanvasItemPrivate;
    friend class QSimpleCanvasItem;

    QSimpleCanvasFilterPrivate *d;
};



QT_END_NAMESPACE

QT_END_HEADER
#endif

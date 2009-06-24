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

#ifndef QFXIMAGEITEM_H
#define QFXIMAGEITEM_H

#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qfxitem.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxPaintedItemPrivate;
class Q_DECLARATIVE_EXPORT QFxPaintedItem : public QFxItem
{
    Q_OBJECT

    Q_PROPERTY(QSize contentsSize READ contentsSize WRITE setContentsSize)
    Q_PROPERTY(bool smooth READ isSmooth WRITE setSmooth)
    Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)
    Q_PROPERTY(int cacheSize READ cacheSize WRITE setCacheSize)

public:
    QFxPaintedItem(QFxItem *parent=0);
    ~QFxPaintedItem();

#if defined(QFX_RENDER_QPAINTER)
    void paintContents(QPainter &painter);
#elif defined(QFX_RENDER_OPENGL)
    void paintGLContents(GLPainter &);
#endif

    bool isSmooth() const;
    QSize contentsSize() const;

    void setSmooth(bool);
    void setContentsSize(const QSize &);

    int cacheSize() const;
    void setCacheSize(int pixels);

    QColor fillColor() const;
    void setFillColor(const QColor&);

protected:
    QFxPaintedItem(QFxPaintedItemPrivate &dd, QFxItem *parent);

    virtual void drawContents(QPainter *p, const QRect &) = 0;

Q_SIGNALS:
    void fillColorChanged();

protected Q_SLOTS:
    void dirtyCache(const QRect &);
    void clearCache();

private:
    void init();
    Q_DISABLE_COPY(QFxPaintedItem)
    Q_DECLARE_PRIVATE(QFxPaintedItem)
};
QML_DECLARE_TYPE(QFxPaintedItem)


QT_END_NAMESPACE

QT_END_HEADER
#endif

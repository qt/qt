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

#ifndef QFXIMAGE_H
#define QFXIMAGE_H

#include <QtDeclarative/qfxitem.h>
#include <QtNetwork/qnetworkreply.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QFxImagePrivate;
class QFxScaleGrid;
class QFxGridScaledImage;

class Q_DECLARATIVE_EXPORT QFxImage : public QFxItem
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(FillMode)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(QFxScaleGrid *scaleGrid READ scaleGrid)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap DESIGNABLE false)
    Q_PROPERTY(bool smooth READ smoothTransform WRITE setSmoothTransform)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged);

public:
    QFxImage(QFxItem *parent=0);
    ~QFxImage();

    enum FillMode { Stretch, PreserveAspect, Tile, TileVertically, TileHorizontally };
    FillMode fillMode() const;
    void setFillMode(FillMode);

    QPixmap pixmap() const;
    void setPixmap(const QPixmap &);

    bool smoothTransform() const;
    void setSmoothTransform(bool);

    enum Status { Null, Ready, Loading, Error };
    Status status() const;
    qreal progress() const;

    QUrl source() const;
    virtual void setSource(const QUrl &url);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

Q_SIGNALS:
    void sourceChanged(const QUrl &);
    void statusChanged(Status);
    void progressChanged(qreal progress);
    void fillModeChanged();

protected:
    QFxImage(QFxImagePrivate &dd, QFxItem *parent);
    virtual void componentComplete();

private Q_SLOTS:
    void requestFinished();
    void sciRequestFinished();
    void requestProgress(qint64,qint64);

private:
    Q_DISABLE_COPY(QFxImage)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QFxImage)
    void setGridScaledImage(const QFxGridScaledImage& sci);
    QFxScaleGrid *scaleGrid();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxImage)

QT_END_HEADER

#endif // QFXIMAGE_H

// Commit: ab71df83ba4eb9d749efc0f3a2d4a0fe5486023f
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QSGIMAGEBASE_P_H
#define QSGIMAGEBASE_P_H

#include "qsgimplicitsizeitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSGImageBasePrivate;
class Q_AUTOTEST_EXPORT QSGImageBase : public QSGImplicitSizeItem
{
    Q_OBJECT
    Q_ENUMS(Status)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(QSize sourceSize READ sourceSize WRITE setSourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(bool mirror READ mirror WRITE setMirror NOTIFY mirrorChanged)

public:
    QSGImageBase(QSGItem *parent=0);
    ~QSGImageBase();
    enum Status { Null, Ready, Loading, Error };
    Status status() const;
    qreal progress() const;

    QUrl source() const;
    virtual void setSource(const QUrl &url);

    bool asynchronous() const;
    void setAsynchronous(bool);

    bool cache() const;
    void setCache(bool);

    virtual void setSourceSize(const QSize&);
    QSize sourceSize() const;

    virtual void setMirror(bool mirror);
    bool mirror() const;

Q_SIGNALS:
    void sourceChanged(const QUrl &);
    void sourceSizeChanged();
    void statusChanged(QSGImageBase::Status);
    void progressChanged(qreal progress);
    void asynchronousChanged();
    void cacheChanged();
    void mirrorChanged();

protected:
    virtual void load();
    virtual void componentComplete();
    virtual void pixmapChange();
    QSGImageBase(QSGImageBasePrivate &dd, QSGItem *parent);

private Q_SLOTS:
    virtual void requestFinished();
    void requestProgress(qint64,qint64);

private:
    Q_DISABLE_COPY(QSGImageBase)
    Q_DECLARE_PRIVATE(QSGImageBase)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSGIMAGEBASE_P_H

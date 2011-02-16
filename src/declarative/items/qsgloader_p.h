// Commit: 6f78a6080b84cc3ef96b73a4ff58d1b5a72f08f4
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

#ifndef QSGLOADER_P_H
#define QSGLOADER_P_H

#include "qsgimplicitsizeitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGLoaderPrivate;
class Q_AUTOTEST_EXPORT QSGLoader : public QSGImplicitSizeItem
{
    Q_OBJECT
    Q_ENUMS(Status)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QDeclarativeComponent *sourceComponent READ sourceComponent WRITE setSourceComponent RESET resetSourceComponent NOTIFY sourceChanged)
    Q_PROPERTY(QSGItem *item READ item NOTIFY itemChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

public:
    QSGLoader(QSGItem *parent=0);
    virtual ~QSGLoader();

    QUrl source() const;
    void setSource(const QUrl &);

    QDeclarativeComponent *sourceComponent() const;
    void setSourceComponent(QDeclarativeComponent *);
    void resetSourceComponent();

    enum Status { Null, Ready, Loading, Error };
    Status status() const;
    qreal progress() const;

    QSGItem *item() const;

Q_SIGNALS:
    void itemChanged();
    void sourceChanged();
    void statusChanged();
    void progressChanged();
    void loaded();

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void componentComplete();

private:
    Q_DISABLE_COPY(QSGLoader)
    Q_DECLARE_PRIVATE(QSGLoader)
    Q_PRIVATE_SLOT(d_func(), void _q_sourceLoaded())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGLoader)

QT_END_HEADER

#endif // QSGLOADER_P_H

// Commit: 2c7cab4172f1acc86fd49345a2847417e162f2c3
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

#ifndef QSGANCHORS_P_H
#define QSGANCHORS_P_H

#include <qdeclarative.h>

#include <QtCore/QObject>

#include <private/qdeclarativeglobal_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGItem;
class QSGAnchorsPrivate;
class QSGAnchorLine;
class Q_DECLARATIVE_PRIVATE_EXPORT QSGAnchors : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QSGAnchorLine left READ left WRITE setLeft RESET resetLeft NOTIFY leftChanged)
    Q_PROPERTY(QSGAnchorLine right READ right WRITE setRight RESET resetRight NOTIFY rightChanged)
    Q_PROPERTY(QSGAnchorLine horizontalCenter READ horizontalCenter WRITE setHorizontalCenter RESET resetHorizontalCenter NOTIFY horizontalCenterChanged)
    Q_PROPERTY(QSGAnchorLine top READ top WRITE setTop RESET resetTop NOTIFY topChanged)
    Q_PROPERTY(QSGAnchorLine bottom READ bottom WRITE setBottom RESET resetBottom NOTIFY bottomChanged)
    Q_PROPERTY(QSGAnchorLine verticalCenter READ verticalCenter WRITE setVerticalCenter RESET resetVerticalCenter NOTIFY verticalCenterChanged)
    Q_PROPERTY(QSGAnchorLine baseline READ baseline WRITE setBaseline RESET resetBaseline NOTIFY baselineChanged)
    Q_PROPERTY(qreal margins READ margins WRITE setMargins NOTIFY marginsChanged)
    Q_PROPERTY(qreal leftMargin READ leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin READ rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)
    Q_PROPERTY(qreal horizontalCenterOffset READ horizontalCenterOffset WRITE setHorizontalCenterOffset NOTIFY horizontalCenterOffsetChanged)
    Q_PROPERTY(qreal topMargin READ topMargin WRITE setTopMargin NOTIFY topMarginChanged)
    Q_PROPERTY(qreal bottomMargin READ bottomMargin WRITE setBottomMargin NOTIFY bottomMarginChanged)
    Q_PROPERTY(qreal verticalCenterOffset READ verticalCenterOffset WRITE setVerticalCenterOffset NOTIFY verticalCenterOffsetChanged)
    Q_PROPERTY(qreal baselineOffset READ baselineOffset WRITE setBaselineOffset NOTIFY baselineOffsetChanged)
    Q_PROPERTY(QSGItem *fill READ fill WRITE setFill RESET resetFill NOTIFY fillChanged)
    Q_PROPERTY(QSGItem *centerIn READ centerIn WRITE setCenterIn RESET resetCenterIn NOTIFY centerInChanged)
    Q_PROPERTY(bool mirrored READ mirrored NOTIFY mirroredChanged)

public:
    QSGAnchors(QSGItem *item, QObject *parent=0);
    virtual ~QSGAnchors();

    enum Anchor {
        LeftAnchor = 0x01,
        RightAnchor = 0x02,
        TopAnchor = 0x04,
        BottomAnchor = 0x08,
        HCenterAnchor = 0x10,
        VCenterAnchor = 0x20,
        BaselineAnchor = 0x40,
        Horizontal_Mask = LeftAnchor | RightAnchor | HCenterAnchor,
        Vertical_Mask = TopAnchor | BottomAnchor | VCenterAnchor | BaselineAnchor
    };
    Q_DECLARE_FLAGS(Anchors, Anchor)

    QSGAnchorLine left() const;
    void setLeft(const QSGAnchorLine &edge);
    void resetLeft();

    QSGAnchorLine right() const;
    void setRight(const QSGAnchorLine &edge);
    void resetRight();

    QSGAnchorLine horizontalCenter() const;
    void setHorizontalCenter(const QSGAnchorLine &edge);
    void resetHorizontalCenter();

    QSGAnchorLine top() const;
    void setTop(const QSGAnchorLine &edge);
    void resetTop();

    QSGAnchorLine bottom() const;
    void setBottom(const QSGAnchorLine &edge);
    void resetBottom();

    QSGAnchorLine verticalCenter() const;
    void setVerticalCenter(const QSGAnchorLine &edge);
    void resetVerticalCenter();

    QSGAnchorLine baseline() const;
    void setBaseline(const QSGAnchorLine &edge);
    void resetBaseline();

    qreal leftMargin() const;
    void setLeftMargin(qreal);

    qreal rightMargin() const;
    void setRightMargin(qreal);

    qreal horizontalCenterOffset() const;
    void setHorizontalCenterOffset(qreal);

    qreal topMargin() const;
    void setTopMargin(qreal);

    qreal bottomMargin() const;
    void setBottomMargin(qreal);

    qreal margins() const;
    void setMargins(qreal);

    qreal verticalCenterOffset() const;
    void setVerticalCenterOffset(qreal);

    qreal baselineOffset() const;
    void setBaselineOffset(qreal);

    QSGItem *fill() const;
    void setFill(QSGItem *);
    void resetFill();

    QSGItem *centerIn() const;
    void setCenterIn(QSGItem *);
    void resetCenterIn();

    Anchors usedAnchors() const;

    bool mirrored();

    void classBegin();
    void componentComplete();

Q_SIGNALS:
    void leftChanged();
    void rightChanged();
    void topChanged();
    void bottomChanged();
    void verticalCenterChanged();
    void horizontalCenterChanged();
    void baselineChanged();
    void fillChanged();
    void centerInChanged();
    void leftMarginChanged();
    void rightMarginChanged();
    void topMarginChanged();
    void bottomMarginChanged();
    void marginsChanged();
    void verticalCenterOffsetChanged();
    void horizontalCenterOffsetChanged();
    void baselineOffsetChanged();
    void mirroredChanged();

private:
    friend class QSGItemPrivate;
    Q_DISABLE_COPY(QSGAnchors)
    Q_DECLARE_PRIVATE(QSGAnchors)
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QSGAnchors::Anchors)

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGAnchors)

QT_END_HEADER

#endif // QSGANCHORS_P_H

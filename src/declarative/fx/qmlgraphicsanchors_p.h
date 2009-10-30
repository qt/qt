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

#ifndef QMLGRAPHICSANCHORS_H
#define QMLGRAPHICSANCHORS_H

#include <QtCore/QObject>
#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlgraphicsitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlGraphicsAnchorsPrivate;
class QmlGraphicsAnchorLine;
class Q_DECLARATIVE_EXPORT QmlGraphicsAnchors : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QmlGraphicsAnchorLine left READ left WRITE setLeft RESET resetLeft)
    Q_PROPERTY(QmlGraphicsAnchorLine right READ right WRITE setRight RESET resetRight)
    Q_PROPERTY(QmlGraphicsAnchorLine horizontalCenter READ horizontalCenter WRITE setHorizontalCenter RESET resetHorizontalCenter)
    Q_PROPERTY(QmlGraphicsAnchorLine top READ top WRITE setTop RESET resetTop)
    Q_PROPERTY(QmlGraphicsAnchorLine bottom READ bottom WRITE setBottom RESET resetBottom)
    Q_PROPERTY(QmlGraphicsAnchorLine verticalCenter READ verticalCenter WRITE setVerticalCenter RESET resetVerticalCenter)
    Q_PROPERTY(QmlGraphicsAnchorLine baseline READ baseline WRITE setBaseline RESET resetBaseline)
    Q_PROPERTY(qreal leftMargin READ leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin READ rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)
    Q_PROPERTY(qreal horizontalCenterOffset READ horizontalCenterOffset WRITE setHorizontalCenterOffset NOTIFY horizontalCenterOffsetChanged())
    Q_PROPERTY(qreal topMargin READ topMargin WRITE setTopMargin NOTIFY topMarginChanged)
    Q_PROPERTY(qreal bottomMargin READ bottomMargin WRITE setBottomMargin NOTIFY bottomMarginChanged)
    Q_PROPERTY(qreal verticalCenterOffset READ verticalCenterOffset WRITE setVerticalCenterOffset NOTIFY verticalCenterOffsetChanged())
    Q_PROPERTY(qreal baselineOffset READ baselineOffset WRITE setBaselineOffset NOTIFY baselineOffsetChanged())
    Q_PROPERTY(QmlGraphicsItem *fill READ fill WRITE setFill)
    Q_PROPERTY(QmlGraphicsItem *centerIn READ centerIn WRITE setCenterIn)

public:
    QmlGraphicsAnchors(QObject *parent=0);
    virtual ~QmlGraphicsAnchors();

    enum UsedAnchor { 
        HasLeftAnchor = 0x01,
        HasRightAnchor = 0x02,
        HasTopAnchor = 0x04,
        HasBottomAnchor = 0x08,
        HasHCenterAnchor = 0x10,
        HasVCenterAnchor = 0x20,
        HasBaselineAnchor = 0x40,
        Horizontal_Mask = HasLeftAnchor | HasRightAnchor | HasHCenterAnchor,
        Vertical_Mask = HasTopAnchor | HasBottomAnchor | HasVCenterAnchor | HasBaselineAnchor
    };
    Q_DECLARE_FLAGS(UsedAnchors, UsedAnchor)

    QmlGraphicsAnchorLine left() const;
    void setLeft(const QmlGraphicsAnchorLine &edge);
    void resetLeft();

    QmlGraphicsAnchorLine right() const;
    void setRight(const QmlGraphicsAnchorLine &edge);
    void resetRight();

    QmlGraphicsAnchorLine horizontalCenter() const;
    void setHorizontalCenter(const QmlGraphicsAnchorLine &edge);
    void resetHorizontalCenter();

    QmlGraphicsAnchorLine top() const;
    void setTop(const QmlGraphicsAnchorLine &edge);
    void resetTop();

    QmlGraphicsAnchorLine bottom() const;
    void setBottom(const QmlGraphicsAnchorLine &edge);
    void resetBottom();

    QmlGraphicsAnchorLine verticalCenter() const;
    void setVerticalCenter(const QmlGraphicsAnchorLine &edge);
    void resetVerticalCenter();

    QmlGraphicsAnchorLine baseline() const;
    void setBaseline(const QmlGraphicsAnchorLine &edge);
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

    qreal verticalCenterOffset() const;
    void setVerticalCenterOffset(qreal);

    qreal baselineOffset() const;
    void setBaselineOffset(qreal);

    QmlGraphicsItem *fill() const;
    void setFill(QmlGraphicsItem *);

    QmlGraphicsItem *centerIn() const;
    void setCenterIn(QmlGraphicsItem *);

    UsedAnchors usedAnchors() const;

    void setItem(QmlGraphicsItem *item);

    void classBegin();
    void componentComplete();

Q_SIGNALS:
    void leftMarginChanged();
    void rightMarginChanged();
    void topMarginChanged();
    void bottomMarginChanged();
    void verticalCenterOffsetChanged();
    void horizontalCenterOffsetChanged();
    void baselineOffsetChanged();

private:
    friend class QmlGraphicsItem;
    Q_DISABLE_COPY(QmlGraphicsAnchors)
    Q_DECLARE_PRIVATE(QmlGraphicsAnchors)
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QmlGraphicsAnchors::UsedAnchors)

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsAnchors)

QT_END_HEADER

#endif

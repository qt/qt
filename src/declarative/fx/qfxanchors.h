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

#ifndef QFXANCHORS_H
#define QFXANCHORS_H

#include <qfxglobal.h>
#include <QObject>
#include <qml.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QFxItem;
class QFxAnchorsPrivate;

class QFxAnchorLine
{
public:
    QFxAnchorLine() : item(0), anchorLine(Left)
    {
    }

    enum AnchorLine {
        Left = 0x01,
        Right = 0x02,
        Top = 0x04,
        Bottom = 0x08,
        HCenter = 0x10,
        VCenter = 0x20,
        Baseline = 0x40,
        Horizontal_Mask = Left | Right | HCenter,
        Vertical_Mask = Top | Bottom | VCenter | Baseline
    };

    QFxItem *item;
    AnchorLine anchorLine;
};

Q_DECLARE_METATYPE(QFxAnchorLine);

class Q_DECLARATIVE_EXPORT QFxAnchors : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QFxAnchorLine left READ left WRITE setLeft);
    Q_PROPERTY(QFxAnchorLine right READ right WRITE setRight);
    Q_PROPERTY(QFxAnchorLine horizontalCenter READ horizontalCenter WRITE setHorizontalCenter);
    Q_PROPERTY(QFxAnchorLine top READ top WRITE setTop);
    Q_PROPERTY(QFxAnchorLine bottom READ bottom WRITE setBottom);
    Q_PROPERTY(QFxAnchorLine verticalCenter READ verticalCenter WRITE setVerticalCenter);
    Q_PROPERTY(int leftMargin READ leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged);
    Q_PROPERTY(int rightMargin READ rightMargin WRITE setRightMargin NOTIFY rightMarginChanged);
    Q_PROPERTY(int horizontalCenterOffset READ horizontalCenterOffset WRITE setHorizontalCenterOffset NOTIFY horizontalCenterOffsetChanged());
    Q_PROPERTY(int topMargin READ topMargin WRITE setTopMargin NOTIFY topMarginChanged);
    Q_PROPERTY(int bottomMargin READ bottomMargin WRITE setBottomMargin NOTIFY bottomMarginChanged);
    Q_PROPERTY(int verticalCenterOffset READ verticalCenterOffset WRITE setVerticalCenterOffset NOTIFY verticalCenterOffsetChanged());
    Q_PROPERTY(QFxItem *fill READ fill WRITE setFill);
    Q_PROPERTY(QFxItem *centeredIn READ centeredIn WRITE setCenteredIn);

public:
    QFxAnchors(QObject *parent=0);

    enum UsedAnchor { 
        HasLeftAnchor = 0x01,
        HasRightAnchor = 0x02,
        HasTopAnchor = 0x04,
        HasBottomAnchor = 0x08,
        HasHCenterAnchor = 0x10,
        HasVCenterAnchor = 0x20,
        HasBaselineAnchor = 0x40
    };
    Q_DECLARE_FLAGS(UsedAnchors, UsedAnchor);

    QFxAnchorLine left() const;
    void setLeft(const QFxAnchorLine &edge);

    QFxAnchorLine right() const;
    void setRight(const QFxAnchorLine &edge);

    QFxAnchorLine horizontalCenter() const;
    void setHorizontalCenter(const QFxAnchorLine &edge);

    QFxAnchorLine top() const;
    void setTop(const QFxAnchorLine &edge);

    QFxAnchorLine bottom() const;
    void setBottom(const QFxAnchorLine &edge);

    QFxAnchorLine verticalCenter() const;
    void setVerticalCenter(const QFxAnchorLine &edge);

    int leftMargin() const;
    void setLeftMargin(int);

    int rightMargin() const;
    void setRightMargin(int);

    int horizontalCenterOffset() const;
    void setHorizontalCenterOffset(int);

    int topMargin() const;
    void setTopMargin(int);

    int bottomMargin() const;
    void setBottomMargin(int);

    int verticalCenterOffset() const;
    void setVerticalCenterOffset(int);

    QFxItem *fill() const;
    void setFill(QFxItem *);

    QFxItem *centeredIn() const;
    void setCenteredIn(QFxItem *);

    UsedAnchors usedAnchors() const;

    void setItem(QFxItem *item);

    void connectHAnchors();
    void connectVAnchors();

Q_SIGNALS:
    void leftMarginChanged();
    void rightMarginChanged();
    void topMarginChanged();
    void bottomMarginChanged();
    void verticalCenterOffsetChanged();
    void horizontalCenterOffsetChanged();

private Q_SLOTS:
    void fillChanged();
    void updateLeftAnchor();
    void updateRightAnchor();
    void updateHCenterAnchor();
    void updateTopAnchor();
    void updateBottomAnchor();
    void updateVCenterAnchor();

private:
    //### should item be a friend? (and make some of the public methods private or protected)
    Q_DISABLE_COPY(QFxAnchors)
    Q_DECLARE_PRIVATE(QFxAnchors)
};

QML_DECLARE_TYPE(QFxAnchors);


QT_END_NAMESPACE

QT_END_HEADER
#endif

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

#ifndef QFXITEM_P_H
#define QFXITEM_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtDeclarative/qfxitem.h>
#include <private/qsimplecanvasitem_p.h>
#include <private/qmlnullablevalue_p.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QNetworkReply;

class QFxItemPrivate : public QSimpleCanvasItemPrivate
{
    Q_DECLARE_PUBLIC(QFxItem)

    typedef QHash<QString, QFxItem *> QmlChildren;

public:
    QFxItemPrivate()
    : _anchors(0), _contents(0), qmlItem(0), _qmlcomp(0),
      _baselineOffset(0), _rotation(0.),
      _classComplete(true), _componentComplete(true), _keepMouse(false), 
      visible(true), _anchorLines(0), visibleOp(1), reparentedChildren(0),
      _stateGroup(0)
    {}
    ~QFxItemPrivate() 
    { delete _anchors; }

    void init(QFxItem *parent)
    {
        Q_Q(QFxItem);

        if (parent)
            q->setItemParent(parent);
        _baselineOffset.invalidate();
        q->setAcceptedMouseButtons(Qt::NoButton);
    }

    QString _id;

    // data property
    void data_removeAt(int);
    int data_count() const;
    void data_append(QObject *);
    void data_insert(int, QObject *);
    QObject *data_at(int) const;
    void data_clear();
    QML_DECLARE_LIST_PROXY(QFxItemPrivate, QObject *, data)

    // resources property
    void resources_removeAt(int);
    int resources_count() const;
    void resources_append(QObject *);
    void resources_insert(int, QObject *);
    QObject *resources_at(int) const;
    void resources_clear();
    QML_DECLARE_LIST_PROXY(QFxItemPrivate, QObject *, resources)

    // children property
    void children_removeAt(int);
    int children_count() const;
    void children_append(QFxItem *);
    void children_insert(int, QFxItem *);
    QFxItem *children_at(int) const;
    void children_clear();
    QML_DECLARE_LIST_PROXY(QFxItemPrivate, QFxItem *, children)

    QList<QFxTransform *> _transform;
    QFxAnchors *anchors() {
        if (!_anchors) {
            Q_Q(QFxItem);
            _anchors = new QFxAnchors;
            _anchors->setItem(q);
        }
        return _anchors;
    }
    QList<QFxAnchors *> dependantAnchors;
    QFxAnchors *_anchors;
    QFxContents *_contents;
    QFxItem *qmlItem;
    QmlComponent *_qmlcomp;
    QUrl _qml;
    QList<QUrl> _qmlnewloading;
    QList<QmlComponent*> _qmlnewcomp;

    QmlNullableValue<int> _baselineOffset;
    float _rotation;

    bool _classComplete:1;
    bool _componentComplete:1;
    bool _keepMouse:1;
    bool visible:1;

    QmlChildren _qmlChildren;

    struct AnchorLines {
        AnchorLines(QFxItem *);
        QFxAnchorLine left;
        QFxAnchorLine right;
        QFxAnchorLine hCenter;
        QFxAnchorLine top;
        QFxAnchorLine bottom;
        QFxAnchorLine vCenter;
        QFxAnchorLine baseline;
    };
    mutable AnchorLines *_anchorLines;
    AnchorLines *anchorLines() const {
        Q_Q(const QFxItem);
        if (!_anchorLines) _anchorLines =
            new AnchorLines(const_cast<QFxItem *>(q));
        return _anchorLines;
    }

    float visibleOp;

    int reparentedChildren;

    QmlStateGroup *states();
    QmlStateGroup *_stateGroup;
};

QT_END_NAMESPACE
#endif // QFXITEM_P_H

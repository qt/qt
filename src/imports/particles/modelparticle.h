/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
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

#ifndef MODELPARTICLE_H
#define MODELPARTICLE_H
#include "particle.h"
#include <QPointer>
#include <QSet>
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QSGVisualDataModel;

class ModelParticle : public ParticleType
{
    Q_OBJECT

    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(int modelCount READ modelCount NOTIFY modelCountChanged)
    Q_CLASSINFO("DefaultProperty", "delegate")
public:
    explicit ModelParticle(QSGItem *parent = 0);
    QVariant model() const;
    void setModel(const QVariant &);

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *);

    int modelCount() const;

    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);
    virtual int count();
signals:
    void modelChanged();
    void delegateChanged();
    void modelCountChanged();

public slots:
    void take(int idx);//TODO: Rename pause and unpause? freeze and unfreeze? take and restore?
    void recover(int idx);

protected:
    virtual void reset();
    void prepareNextFrame();
private:
    bool m_ownModel;
    QDeclarativeComponent* m_comp;
    QSGVisualDataModel *m_model;
    QVariant m_dataSource;
    QList<QPointer<QSGItem> > m_deletables;
    int m_particleCount;

    QVector<QSGItem*> m_items;
    QVector<ParticleData*> m_data;
    QVector<int> m_idx;
    QList<int> m_available;
    QSet<QSGItem*> m_stasis;
    qreal m_lastT;
};

QT_END_NAMESPACE

QT_END_HEADER
#endif // MODELPARTICLE_H

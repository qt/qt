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

#ifndef QMLSTATEOPERATIONS_H
#define QMLSTATEOPERATIONS_H

#include <QtDeclarative/qmlstate.h>
#include <QtDeclarative/qfxitem.h>
#include <QtDeclarative/qfxanchors.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlParentChangePrivate;
class Q_DECLARATIVE_EXPORT QmlParentChange : public QmlStateOperation, public ActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlParentChange)

    Q_PROPERTY(QFxItem *target READ object WRITE setObject)
    Q_PROPERTY(QFxItem *parent READ parent WRITE setParent)
public:
    QmlParentChange(QObject *parent=0);
    ~QmlParentChange();

    QFxItem *object() const;
    void setObject(QFxItem *);

    QFxItem *parent() const;
    void setParent(QFxItem *);

    virtual ActionList actions();

    virtual void execute();
    virtual bool isReversable();
    virtual void reverse();
    virtual QString typeName() const;
};

class QmlRunScriptPrivate;
class Q_DECLARATIVE_EXPORT QmlRunScript : public QmlStateOperation, public ActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlRunScript)

    Q_PROPERTY(QString script READ script WRITE setScript)
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QmlRunScript(QObject *parent=0);
    ~QmlRunScript();

    virtual ActionList actions();

    QString script() const;
    void setScript(const QString &);
    
    QString name() const;
    void setName(const QString &);

    virtual void execute();
};

class QmlSetAnchorsPrivate;
class Q_DECLARATIVE_EXPORT QmlSetAnchors : public QmlStateOperation, public ActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlSetAnchors)

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QFxItem *target READ object WRITE setObject)
    Q_PROPERTY(QString reset READ reset WRITE setReset)
    Q_PROPERTY(QFxAnchorLine left READ left WRITE setLeft)
    Q_PROPERTY(QFxAnchorLine right READ right WRITE setRight)
    Q_PROPERTY(QFxAnchorLine top READ top WRITE setTop)
    Q_PROPERTY(QFxAnchorLine bottom READ bottom WRITE setBottom)

public:
    QmlSetAnchors(QObject *parent=0);
    ~QmlSetAnchors();

    virtual ActionList actions();

    QString name() const;
    void setName(const QString &);

    QFxItem *object() const;
    void setObject(QFxItem *);

    QString reset() const;
    void setReset(const QString &);

    QFxAnchorLine left() const;
    void setLeft(const QFxAnchorLine &edge);

    QFxAnchorLine right() const;
    void setRight(const QFxAnchorLine &edge);

    QFxAnchorLine top() const;
    void setTop(const QFxAnchorLine &edge);

    QFxAnchorLine bottom() const;
    void setBottom(const QFxAnchorLine &edge);

    virtual void execute();
    virtual bool isReversable();
    virtual void reverse();
    virtual QString typeName() const;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlParentChange)
QML_DECLARE_TYPE(QmlRunScript)
QML_DECLARE_TYPE(QmlSetAnchors)

QT_END_HEADER

#endif // QMLSTATEOPERATIONS_H

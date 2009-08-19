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

#ifndef QFXVIEW_H
#define QFXVIEW_H

#include <QtCore/qdatetime.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qwidget.h>
#include <QtDeclarative/qfxglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxItem;
class QmlEngine;
class QmlContext;
class QmlError;

class QFxViewPrivate;
class Q_DECLARATIVE_EXPORT QFxView : public QGraphicsView
{
    Q_OBJECT
    Q_PROPERTY(bool contentResizable READ contentResizable WRITE setContentResizable)
public:
    explicit QFxView(QWidget *parent = 0);

    virtual ~QFxView();

    void setUrl(const QUrl&);
    void setQml(const QString &qml, const QString &filename=QString());
    QString qml() const;
    QmlEngine* engine();
    QmlContext* rootContext();
    virtual void execute();
    virtual void reset();

    virtual QFxItem* addItem(const QString &qml, QFxItem* parent=0);
    virtual void clearItems();

    virtual QFxItem *root() const;

    void setContentResizable(bool);
    bool contentResizable() const;
    QSize sizeHint() const;

Q_SIGNALS:
    void sceneResized(QSize size);
    void errors(const QList<QmlError> &error);

private Q_SLOTS:
    void continueExecute();
    void sizeChanged();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent(QPaintEvent *event);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
    void timerEvent(QTimerEvent*);

private:
    friend class QFxViewPrivate;
    QFxViewPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFXVIEW_H

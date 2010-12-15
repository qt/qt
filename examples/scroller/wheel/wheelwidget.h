/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef WHEELWIDGET_H
#define WHEELWIDGET_H

#include <QWidget>
#include <QStringList>

class QPainter;
class QRect;

class AbstractWheelWidget : public QWidget {
    Q_OBJECT

public:
    AbstractWheelWidget(bool touch, QWidget *parent = 0);
    virtual ~AbstractWheelWidget();

    int currentIndex() const;
    void setCurrentIndex(int index);

    bool event(QEvent*);
    void paintEvent(QPaintEvent *e);
    virtual void paintItem(QPainter* painter, int index, const QRect &rect) = 0;

    virtual int itemHeight() const = 0;
    virtual int itemCount() const = 0;

public slots:
    void scrollTo(int index);

signals:
    void stopped(int index);

protected:
    int m_currentItem;
    int m_itemOffset; // 0-itemHeight()
    qreal m_lastY;
};


class StringWheelWidget : public AbstractWheelWidget {
    Q_OBJECT

public:
    StringWheelWidget(bool touch);

    QStringList items() const;
    void setItems( const QStringList &items );

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void paintItem(QPainter* painter, int index, const QRect &rect);

    int itemHeight() const;
    int itemCount() const;

private:
    QStringList m_items;
};

#endif // WHEELWIDGET_H

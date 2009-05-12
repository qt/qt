/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QGRAPHICSPIEMENU_H
#define QGRAPHICSPIEMENU_H

#include <QtGui/qgraphicswidget.h>

class QGraphicsPieMenuPrivate;
class QGraphicsPieMenu : public QGraphicsWidget
{
    Q_OBJECT
public:
    QGraphicsPieMenu(QGraphicsItem *parent = 0);
    QGraphicsPieMenu(const QString &title, QGraphicsItem *parent = 0);
    QGraphicsPieMenu(const QIcon &icon, const QString &title, QGraphicsItem *parent = 0);
    ~QGraphicsPieMenu();

#ifdef Q_NO_USING_KEYWORD
    inline void addAction(QAction *action) { QGraphicsWidget::addAction(action); }
#else
    using QGraphicsWidget::addAction;
#endif
    QAction *addAction(const QString &text);
    QAction *addAction(const QIcon &icon, const QString &text);
    QAction *addAction(const QString &text, const QObject *receiver, const char *member, const QKeySequence &shortcut = 0);
    QAction *addAction(const QIcon &icon, const QString &text, const QObject *receiver, const char *member, const QKeySequence &shortcut = 0);
    QAction *addMenu(QGraphicsPieMenu *menu);
    QGraphicsPieMenu *addMenu(const QString &title);
    QGraphicsPieMenu *addMenu(const QIcon &icon, const QString &title);
    QAction *addSeparator();
    QAction *insertMenu(QAction *before, QGraphicsPieMenu *menu);
    QAction *insertSeparator(QAction *before);

    QAction *menuAction() const;

    bool isEmpty() const;
    void clear();

    void popup(const QPointF &pos);
    QAction *exec();
    QAction *exec(const QPointF &pos);
    static QAction *exec(QList<QAction *> actions, const QPointF &pos);

    QString title() const;
    void setTitle(const QString &title);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

Q_SIGNALS:
    void aboutToShow();
    void aboutToHide();
    void triggered(QAction *action);
    void hovered(QAction *action);

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

private:
    Q_DISABLE_COPY(QGraphicsPieMenu)
    Q_DECLARE_PRIVATE(QGraphicsPieMenu)
    QGraphicsPieMenuPrivate *d_ptr;
};

#endif

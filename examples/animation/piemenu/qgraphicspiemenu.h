/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
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

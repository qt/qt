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

#ifndef __MENU__H__
#define __MENU__H__

#include <QtGui>

class MenuAction;

class Menu : public QGraphicsWidget
{
    Q_OBJECT
public:
    Menu(QGraphicsItem *parent);
    ~Menu();
    
    MenuAction *addAction(const QString&, QObject *receiver = 0, const char* slot = 0 );

    QRectF boundingRect() const;
    void keyPressEvent ( QKeyEvent * event );
public slots:
    void show();
    void hide();
private:
    QList<MenuAction*> m_actions;
    QGraphicsRectItem *m_selection;
    int m_selected;
};

class MenuAction : public QGraphicsTextItem
{
    Q_OBJECT
public:
    MenuAction(const QString &text, Menu * parent);
    void trigger();
signals:
    void triggered();
};


#endif //__RIVERITEM__H__

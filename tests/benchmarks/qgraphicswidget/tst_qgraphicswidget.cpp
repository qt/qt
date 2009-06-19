/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
****************************************************************************/

#include <qtest.h>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
//TESTED_FILES=

class tst_QGraphicsWidget : public QObject
{
    Q_OBJECT

public:
    tst_QGraphicsWidget();
    virtual ~tst_QGraphicsWidget();

public slots:
    void init();
    void cleanup();

private slots:
    void move();
};

tst_QGraphicsWidget::tst_QGraphicsWidget()
{
}

tst_QGraphicsWidget::~tst_QGraphicsWidget()
{
}

void tst_QGraphicsWidget::init()
{
}

void tst_QGraphicsWidget::cleanup()
{
}

void tst_QGraphicsWidget::move()
{
    QGraphicsScene scene;
    QGraphicsWidget *widget = new QGraphicsWidget();
    scene.addItem(widget);
    QGraphicsView view(&scene);
    view.show();
    QBENCHMARK {
        widget->setPos(qrand(),qrand());
    }
}

QTEST_MAIN(tst_QGraphicsWidget)
#include "tst_qgraphicswidget.moc"

#include "graphicsview.h"

#include <QtGui/QKeyEvent>

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {}

void GraphicsView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();

    emit keyPressed(Qt::Key(e->key()));
}



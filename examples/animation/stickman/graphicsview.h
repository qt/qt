#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW

#include <QtGui/QGraphicsView>

class GraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *);

signals:
    void keyPressed(int key);
};

#endif

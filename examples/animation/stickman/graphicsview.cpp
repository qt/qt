#include "graphicsview.h"
#include "editor/mainwindow.h"
#include "stickman.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent), m_editor(0) {}

void GraphicsView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();

#if 0
    if (e->key() == Qt::Key_F1) {
        if (m_editor == 0) {
            QGraphicsScene *scene = new QGraphicsScene;
            StickMan *stickMan = new StickMan;
            stickMan->setDrawSticks(true);
            scene->addItem(stickMan);

            QGraphicsView *view = new QGraphicsView;
            view->setBackgroundBrush(Qt::black);
            view->setRenderHints(QPainter::Antialiasing);
            view->setScene(scene);

            m_editor = new MainWindow(stickMan);
            m_editor->setCentralWidget(view);
        }

        m_editor->showMaximized();
    }
#endif

    emit keyPressed(Qt::Key(e->key()));
}



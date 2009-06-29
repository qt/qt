/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtWebKit>

class PannableWebView : public QWebView
{
public:
    PannableWebView(QWidget *parent = 0)
        : QWebView(parent)
    {
#if 0
        QPushButton *btn = new QPushButton("Some test button", this);
        btn->resize(300, 200);
        btn->move(40, 300);
#endif
        grabGesture(Qt::PanGesture);
    }
protected:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::Gesture)
        {
            QGestureEvent *ev = static_cast<QGestureEvent*>(event);
            if (const QGesture *g = ev->gesture(Qt::PanGesture)) {
                if (QWebFrame *frame = page()->mainFrame()) {
                    QPoint offset = g->pos() - g->lastPos();
                    frame->setScrollPosition(frame->scrollPosition() - offset);
                }
                event->accept();
            }
            return true;
        }
        return QWebView::event(event);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QGraphicsScene scene;
    QGraphicsView w(&scene);

    QWebView *wv = new PannableWebView;
    wv->resize(480, 800);
    wv->setUrl(QUrl("http://www.trolltech.com"));
    scene.addWidget(wv);
    w.show();

    return app.exec();
}

/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

class Pixmap : public QGraphicsWidget
{
    Q_OBJECT
public:
    Pixmap(const QPixmap &pix) : QGraphicsWidget(), p(pix)
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->drawPixmap(QPointF(), p);
    }

protected:
    QSizeF sizeHint(Qt::SizeHint, const QSizeF & = QSizeF())
    {
        return QSizeF(p.width(), p.height());
    }

private:
    QPixmap p;
};

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(states);

    QApplication app(argc, argv);

    // Text edit and button
    QTextEdit *edit = new QTextEdit;
    edit->setText("asdf lkjha yuoiqwe asd iuaysd u iasyd uiy "
                  "asdf lkjha yuoiqwe asd iuaysd u iasyd uiy "
                  "asdf lkjha yuoiqwe asd iuaysd u iasyd uiy "
                  "asdf lkjha yuoiqwe asd iuaysd u iasyd uiy!");

    QPushButton *button = new QPushButton;
    QGraphicsProxyWidget *buttonProxy = new QGraphicsProxyWidget;
    buttonProxy->setWidget(button);
    QGraphicsProxyWidget *editProxy = new QGraphicsProxyWidget;
    editProxy->setWidget(edit);

    QGroupBox *box = new QGroupBox;
    box->setFlat(true);
    box->setTitle("Options");

    QVBoxLayout *layout2 = new QVBoxLayout;
    box->setLayout(layout2);
    layout2->addWidget(new QRadioButton("Herring"));
    layout2->addWidget(new QRadioButton("Blue Parrot"));
    layout2->addWidget(new QRadioButton("Petunias"));
    layout2->addStretch();

    QGraphicsProxyWidget *boxProxy = new QGraphicsProxyWidget;
    boxProxy->setWidget(box);

    // Parent widget
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, widget);
    layout->addItem(editProxy);
    layout->addItem(buttonProxy);
    widget->setLayout(layout);

    Pixmap *p1 = new Pixmap(QPixmap(":/digikam.png"));
    Pixmap *p2 = new Pixmap(QPixmap(":/akregator.png"));
    Pixmap *p3 = new Pixmap(QPixmap(":/accessories-dictionary.png"));
    Pixmap *p4 = new Pixmap(QPixmap(":/k3b.png"));
    Pixmap *p5 = new Pixmap(QPixmap(":/help-browser.png"));
    Pixmap *p6 = new Pixmap(QPixmap(":/kchart.png"));

    QGraphicsScene scene(0, 0, 400, 300);
    scene.setBackgroundBrush(scene.palette().window());
    scene.addItem(widget);
    scene.addItem(boxProxy);
    scene.addItem(p1);
    scene.addItem(p2);
    scene.addItem(p3);
    scene.addItem(p4);
    scene.addItem(p5);
    scene.addItem(p6);

    QStateMachine machine;
    QState *root = machine.rootState();
    QState *state1 = new QState(root);
    QState *state2 = new QState(root);
    QState *state3 = new QState(root);
    machine.setInitialState(state1);

    // State 1
    state1->assignProperty(button, "text", "Switch to state 2");
    state1->assignProperty(widget, "geometry", QRectF(0, 0, 400, 150));
    state1->assignProperty(box, "geometry", QRect(-200, 150, 200, 150));
    state1->assignProperty(p1, "geometry", QRectF(68, 185, 64, 64));
    state1->assignProperty(p2, "geometry", QRectF(168, 185, 64, 64));
    state1->assignProperty(p3, "geometry", QRectF(268, 185, 64, 64));
    state1->assignProperty(p4, "geometry", QRectF(68-150, 48-150, 64, 64));
    state1->assignProperty(p5, "geometry", QRectF(168, 48-150, 64, 64));
    state1->assignProperty(p6, "geometry", QRectF(268+150, 48-150, 64, 64));
    state1->assignProperty(p1, "zRotation", qreal(0));
    state1->assignProperty(p2, "zRotation", qreal(0));
    state1->assignProperty(p3, "zRotation", qreal(0));
    state1->assignProperty(p4, "zRotation", qreal(-270));
    state1->assignProperty(p5, "zRotation", qreal(-90));
    state1->assignProperty(p6, "zRotation", qreal(270));
    state1->assignProperty(boxProxy, "opacity", qreal(0));
    state1->assignProperty(p1, "opacity", qreal(1));
    state1->assignProperty(p2, "opacity", qreal(1));
    state1->assignProperty(p3, "opacity", qreal(1));
    state1->assignProperty(p4, "opacity", qreal(0));
    state1->assignProperty(p5, "opacity", qreal(0));
    state1->assignProperty(p6, "opacity", qreal(0));

    // State 2
    state2->assignProperty(button, "text", "Switch to state 3");
    state2->assignProperty(widget, "geometry", QRectF(200, 150, 200, 150));
    state2->assignProperty(box, "geometry", QRect(9, 150, 190, 150));
    state2->assignProperty(p1, "geometry", QRectF(68-150, 185+150, 64, 64));
    state2->assignProperty(p2, "geometry", QRectF(168, 185+150, 64, 64));
    state2->assignProperty(p3, "geometry", QRectF(268+150, 185+150, 64, 64));
    state2->assignProperty(p4, "geometry", QRectF(64, 48, 64, 64));
    state2->assignProperty(p5, "geometry", QRectF(168, 48, 64, 64));
    state2->assignProperty(p6, "geometry", QRectF(268, 48, 64, 64));
    state2->assignProperty(p1, "zRotation", qreal(-270));
    state2->assignProperty(p2, "zRotation", qreal(90));
    state2->assignProperty(p3, "zRotation", qreal(270));
    state2->assignProperty(p4, "zRotation", qreal(0));
    state2->assignProperty(p5, "zRotation", qreal(0));
    state2->assignProperty(p6, "zRotation", qreal(0));
    state2->assignProperty(boxProxy, "opacity", qreal(1));
    state2->assignProperty(p1, "opacity", qreal(0));
    state2->assignProperty(p2, "opacity", qreal(0));
    state2->assignProperty(p3, "opacity", qreal(0));
    state2->assignProperty(p4, "opacity", qreal(1));
    state2->assignProperty(p5, "opacity", qreal(1));
    state2->assignProperty(p6, "opacity", qreal(1));

    // State 3
    state3->assignProperty(button, "text", "Switch to state 1");
    state3->assignProperty(p1, "geometry", QRectF(5, 5, 64, 64));
    state3->assignProperty(p2, "geometry", QRectF(5, 5 + 64 + 5, 64, 64));
    state3->assignProperty(p3, "geometry", QRectF(5, 5 + (64 + 5) + 64, 64, 64));
    state3->assignProperty(p4, "geometry", QRectF(5 + 64 + 5, 5, 64, 64));
    state3->assignProperty(p5, "geometry", QRectF(5 + 64 + 5, 5 + 64 + 5, 64, 64));
    state3->assignProperty(p6, "geometry", QRectF(5 + 64 + 5, 5 + (64 + 5) + 64, 64, 64));
    state3->assignProperty(widget, "geometry", QRectF(138, 5, 400 - 138, 200));
    state3->assignProperty(box, "geometry", QRect(5, 205, 400, 90));
    state3->assignProperty(p1, "opacity", qreal(1));
    state3->assignProperty(p2, "opacity", qreal(1));
    state3->assignProperty(p3, "opacity", qreal(1));
    state3->assignProperty(p4, "opacity", qreal(1));
    state3->assignProperty(p5, "opacity", qreal(1));
    state3->assignProperty(p6, "opacity", qreal(1));

    QParallelAnimationGroup animation1;

    QSequentialAnimationGroup *animation1SubGroup;
    animation1SubGroup = new QSequentialAnimationGroup(&animation1);
    animation1SubGroup->addPause(250);
    animation1SubGroup->addAnimation(new QPropertyAnimation(box, "geometry"));

    animation1.addAnimation(new QPropertyAnimation(widget, "geometry"));
    animation1.addAnimation(new QPropertyAnimation(p1, "geometry"));
    animation1.addAnimation(new QPropertyAnimation(p2, "geometry"));
    animation1.addAnimation(new QPropertyAnimation(p3, "geometry"));
    animation1.addAnimation(new QPropertyAnimation(p4, "geometry"));
    animation1.addAnimation(new QPropertyAnimation(p5, "geometry"));
    animation1.addAnimation(new QPropertyAnimation(p6, "geometry"));
    animation1.addAnimation(new QPropertyAnimation(p1, "zRotation"));
    animation1.addAnimation(new QPropertyAnimation(p2, "zRotation"));
    animation1.addAnimation(new QPropertyAnimation(p3, "zRotation"));
    animation1.addAnimation(new QPropertyAnimation(p4, "zRotation"));
    animation1.addAnimation(new QPropertyAnimation(p5, "zRotation"));
    animation1.addAnimation(new QPropertyAnimation(p6, "zRotation"));
    animation1.addAnimation(new QPropertyAnimation(p1, "opacity"));
    animation1.addAnimation(new QPropertyAnimation(p2, "opacity"));
    animation1.addAnimation(new QPropertyAnimation(p3, "opacity"));
    animation1.addAnimation(new QPropertyAnimation(p4, "opacity"));
    animation1.addAnimation(new QPropertyAnimation(p5, "opacity"));
    animation1.addAnimation(new QPropertyAnimation(p6, "opacity"));

    QParallelAnimationGroup animation2;
    animation2.addAnimation(new QPropertyAnimation(box, "geometry"));
    animation2.addAnimation(new QPropertyAnimation(widget, "geometry"));
    animation2.addAnimation(new QPropertyAnimation(p1, "geometry"));
    animation2.addAnimation(new QPropertyAnimation(p2, "geometry"));
    animation2.addAnimation(new QPropertyAnimation(p3, "geometry"));
    animation2.addAnimation(new QPropertyAnimation(p4, "geometry"));
    animation2.addAnimation(new QPropertyAnimation(p5, "geometry"));
    animation2.addAnimation(new QPropertyAnimation(p6, "geometry"));
    animation2.addAnimation(new QPropertyAnimation(p1, "zRotation"));
    animation2.addAnimation(new QPropertyAnimation(p2, "zRotation"));
    animation2.addAnimation(new QPropertyAnimation(p3, "zRotation"));
    animation2.addAnimation(new QPropertyAnimation(p4, "zRotation"));
    animation2.addAnimation(new QPropertyAnimation(p5, "zRotation"));
    animation2.addAnimation(new QPropertyAnimation(p6, "zRotation"));
    animation2.addAnimation(new QPropertyAnimation(p1, "opacity"));
    animation2.addAnimation(new QPropertyAnimation(p2, "opacity"));
    animation2.addAnimation(new QPropertyAnimation(p3, "opacity"));
    animation2.addAnimation(new QPropertyAnimation(p4, "opacity"));
    animation2.addAnimation(new QPropertyAnimation(p5, "opacity"));
    animation2.addAnimation(new QPropertyAnimation(p6, "opacity"));

    QParallelAnimationGroup animation3;
    animation3.addAnimation(new QPropertyAnimation(box, "geometry"));
    animation3.addAnimation(new QPropertyAnimation(widget, "geometry"));
    animation3.addAnimation(new QPropertyAnimation(p1, "geometry"));
    animation3.addAnimation(new QPropertyAnimation(p2, "geometry"));
    animation3.addAnimation(new QPropertyAnimation(p3, "geometry"));
    animation3.addAnimation(new QPropertyAnimation(p4, "geometry"));
    animation3.addAnimation(new QPropertyAnimation(p5, "geometry"));
    animation3.addAnimation(new QPropertyAnimation(p6, "geometry"));
    animation3.addAnimation(new QPropertyAnimation(p1, "zRotation"));
    animation3.addAnimation(new QPropertyAnimation(p2, "zRotation"));
    animation3.addAnimation(new QPropertyAnimation(p3, "zRotation"));
    animation3.addAnimation(new QPropertyAnimation(p4, "zRotation"));
    animation3.addAnimation(new QPropertyAnimation(p5, "zRotation"));
    animation3.addAnimation(new QPropertyAnimation(p6, "zRotation"));
    animation3.addAnimation(new QPropertyAnimation(p1, "opacity"));
    animation3.addAnimation(new QPropertyAnimation(p2, "opacity"));
    animation3.addAnimation(new QPropertyAnimation(p3, "opacity"));
    animation3.addAnimation(new QPropertyAnimation(p4, "opacity"));
    animation3.addAnimation(new QPropertyAnimation(p5, "opacity"));
    animation3.addAnimation(new QPropertyAnimation(p6, "opacity"));

    QAbstractTransition *t1 = state1->addTransition(button, SIGNAL(clicked()), state2);
    t1->addAnimation(&animation1);
    QAbstractTransition *t2 = state2->addTransition(button, SIGNAL(clicked()), state3);
    t2->addAnimation(&animation2);
    QAbstractTransition *t3 = state3->addTransition(button, SIGNAL(clicked()), state1);
    t3->addAnimation(&animation3);

    machine.start();

    QGraphicsView view(&scene);
    view.show();

    return app.exec();
}

#include "main.moc"

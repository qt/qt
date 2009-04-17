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

#include "window.h"

Window::Window(QWidget *parent) 
    : QWidget(parent), m_iconSize(64, 64)
{
    m_ui.setupUi(this);
    QButtonGroup *buttonGroup = qFindChild<QButtonGroup *>(this);     // ### workaround for uic in 4.4
    m_ui.easingCurvePicker->setIconSize(m_iconSize);  
    m_ui.easingCurvePicker->setMinimumHeight(m_iconSize.height() + 50);
    buttonGroup->setId(m_ui.lineRadio, 0);
    buttonGroup->setId(m_ui.circleRadio, 1);
    
    QEasingCurve dummy;
    m_ui.periodSpinBox->setValue(dummy.period());
    m_ui.amplitudeSpinBox->setValue(dummy.amplitude());
    m_ui.overshootSpinBox->setValue(dummy.overshoot());
    
    connect(m_ui.easingCurvePicker, SIGNAL(currentRowChanged(int)), this, SLOT(curveChanged(int)));
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(pathChanged(int)));
    connect(m_ui.periodSpinBox, SIGNAL(valueChanged(double)), this, SLOT(periodChanged(double)));
    connect(m_ui.amplitudeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(amplitudeChanged(double)));
    connect(m_ui.overshootSpinBox, SIGNAL(valueChanged(double)), this, SLOT(overshootChanged(double)));
    createCurveIcons();
    
    QPixmap pix(QLatin1String(":/images/qt-logo.png"));
    m_item = new PixmapItem(pix);
    m_scene.addItem(m_item);
    m_ui.graphicsView->setScene(&m_scene);

    m_anim = new Animation(m_item, "pos");
    m_anim->setEasingCurve(QEasingCurve::OutBounce);
    m_ui.easingCurvePicker->setCurrentRow(int(QEasingCurve::OutBounce));

    startAnimation();
}

void Window::createCurveIcons()
{
    QPixmap pix(m_iconSize);
    QPainter painter(&pix);
    QLinearGradient gradient(0,0, 0, m_iconSize.height());
    gradient.setColorAt(0.0, QColor(240, 240, 240));
    gradient.setColorAt(1.0, QColor(224, 224, 224));
    QBrush brush(gradient);
    const QMetaObject &mo = QEasingCurve::staticMetaObject;
    QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
    // Skip QEasingCurve::Custom
    for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i) {
        painter.fillRect(QRect(QPoint(0, 0), m_iconSize), brush);
        QEasingCurve curve((QEasingCurve::Type)i);
        painter.setPen(QColor(0, 0, 255, 64));
        qreal xAxis = m_iconSize.height()/1.5;
        qreal yAxis = m_iconSize.width()/3;
        painter.drawLine(0, xAxis, m_iconSize.width(),  xAxis);
        painter.drawLine(yAxis, 0, yAxis, m_iconSize.height());
        painter.setPen(Qt::black);
        
        qreal curveScale = m_iconSize.height()/2;
        QPoint currentPos(yAxis, xAxis);
        
        for (qreal t = 0; t < 1.0; t+=1.0/curveScale) {
            QPoint to;
            to.setX(yAxis + curveScale * t);
            to.setY(xAxis - curveScale * curve.valueForProgress(t));
            painter.drawLine(currentPos, to);
            currentPos = to;
        }
        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(QIcon(pix));
        item->setText(metaEnum.key(i));
        m_ui.easingCurvePicker->addItem(item);
    }
}

void Window::startAnimation()
{
    m_anim->setStartValue(QPointF(0, 0));
    m_anim->setEndValue(QPointF(100, 100));
    m_anim->setDuration(2000);
    m_anim->setIterationCount(-1); // forever
    m_anim->start();
}

void Window::curveChanged(int row)
{
    QEasingCurve::Type curveType = (QEasingCurve::Type)row;
    m_anim->setEasingCurve(curveType);
    m_anim->setCurrentTime(0);
    
    bool isElastic = curveType >= QEasingCurve::InElastic && curveType <= QEasingCurve::OutInElastic;
    bool isBounce = curveType >= QEasingCurve::InBounce && curveType <= QEasingCurve::OutInBounce;
    m_ui.periodSpinBox->setEnabled(isElastic);
    m_ui.amplitudeSpinBox->setEnabled(isElastic || isBounce);
    m_ui.overshootSpinBox->setEnabled(curveType >= QEasingCurve::InBack && curveType <= QEasingCurve::OutInBack);
}

void Window::pathChanged(int index)
{
    m_anim->setPathType((Animation::PathType)index);
}

void Window::periodChanged(double value)
{
    QEasingCurve curve = m_anim->easingCurve();
    curve.setPeriod(value);
    m_anim->setEasingCurve(curve);
}

void Window::amplitudeChanged(double value)
{
    QEasingCurve curve = m_anim->easingCurve();
    curve.setAmplitude(value);
    m_anim->setEasingCurve(curve);
}

void Window::overshootChanged(double value)
{
    QEasingCurve curve = m_anim->easingCurve();
    curve.setOvershoot(value);
    m_anim->setEasingCurve(curve);
}


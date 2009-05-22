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

#include <QtGui>

void createCurveIcons();

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    createCurveIcons();
    return app.exit();
}

void createCurveIcons()
{
    QDir dir(QDir::current());
    if (dir.dirName() == QLatin1String("debug") || dir.dirName() == QLatin1String("release")) {
        dir.cdUp();
    }
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    QSize iconSize(128, 128);
    QPixmap pix(iconSize);
    QPainter painter(&pix);
    QLinearGradient gradient(0,0, 0, iconSize.height());
    gradient.setColorAt(0.0, QColor(240, 240, 240));
    gradient.setColorAt(1.0, QColor(224, 224, 224));
    QBrush brush(gradient);
    const QMetaObject &mo = QEasingCurve::staticMetaObject;
    QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
    QFont oldFont = painter.font();
    // Skip QEasingCurve::Custom
    QString output(QString::fromAscii("%1/images").arg(dir.absolutePath()));
    printf("Generating images to %s\n", qPrintable(output));
    for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i) {
        painter.setFont(oldFont);
        QString name(QLatin1String(metaEnum.key(i)));
        painter.fillRect(QRect(QPoint(0, 0), iconSize), brush);
        QEasingCurve curve((QEasingCurve::Type)i);
        painter.setPen(QColor(0, 0, 255, 64));
        qreal xAxis = iconSize.height()/1.5;
        qreal yAxis = iconSize.width()/3;
        painter.drawLine(0, xAxis, iconSize.width(),  xAxis); // hor
        painter.drawLine(yAxis, 0, yAxis, iconSize.height()); // ver

        qreal curveScale = iconSize.height()/2;

        painter.drawLine(yAxis - 2, xAxis - curveScale, yAxis + 2, xAxis - curveScale); // hor 
        painter.drawLine(yAxis + curveScale, xAxis + 2, yAxis + curveScale, xAxis - 2); // ver
        painter.drawText(yAxis + curveScale - 8, xAxis - curveScale - 4, QLatin1String("(1,1)"));
        
        painter.drawText(yAxis + 42, xAxis + 10, QLatin1String("progress"));
        painter.drawText(15, xAxis - curveScale - 10, QLatin1String("ease"));
        
        painter.setPen(QPen(Qt::red, 1, Qt::DotLine));        
        painter.drawLine(yAxis, xAxis - curveScale, yAxis + curveScale, xAxis - curveScale); // hor
        painter.drawLine(yAxis + curveScale, xAxis, yAxis + curveScale, xAxis - curveScale); // ver
        
        QPoint currentPos(yAxis, xAxis);
        
        painter.setPen(Qt::black);
        QFont font = oldFont;
        font.setPixelSize(oldFont.pixelSize() + 15);
        painter.setFont(font);
        painter.drawText(0, iconSize.height() - 20, iconSize.width(), 20, Qt::AlignHCenter, name);
       
        for (qreal t = 0; t < 1.0; t+=1.0/curveScale) {
            QPoint to;
            to.setX(yAxis + curveScale * t);
            to.setY(xAxis - curveScale * curve.valueForProgress(t));
            painter.drawLine(currentPos, to);
            currentPos = to;
        }
        QString fileName(QString::fromAscii("qeasingcurve-%1.png").arg(name.toLower()));
        printf("%s\n", qPrintable(fileName));
        pix.save(QString::fromAscii("%1/%2").arg(output).arg(fileName), "PNG");
    }
}



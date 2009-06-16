/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#ifndef WIDGETS_H
#define WIDGETS_H

#include "paintcommands.h"

#include <QWidget>
#include <QSettings>
#include <QFileInfo>
#include <QPainter>
#include <QPaintEvent>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QPaintEngine>

#include <qmath.h>

const int CP_RADIUS = 10;

template <class T>
class OnScreenWidget : public T
{
public:
    OnScreenWidget(QWidget *parent = 0) : T(parent)
    {
        QSettings settings("Trolltech", "lance");
        for (int i=0; i<10; ++i) {
            QPointF suggestion(100 + i * 40, 100 + 100 * qSin(i * 3.1415 / 10.0));
            m_controlPoints << settings.value("cp" + QString::number(i), suggestion).toPointF();
        }

        m_currentPoint = -1;
        m_showControlPoints = false;
        m_deviceType = WidgetType;
        m_checkersBackground = true;
        m_verboseMode = false;
    }

    void setVerboseMode(bool v) { m_verboseMode = v; }
    void setCheckersBackground(bool b) { m_checkersBackground = b; }
    void setType(DeviceType t) { m_deviceType = t; }

    ~OnScreenWidget()
    {
        QSettings settings("Trolltech", "lance");
        for (int i=0; i<10; ++i) {
            settings.setValue("cp" + QString::number(i), m_controlPoints.at(i));
        }
        settings.sync();
    }

    void resizeEvent(QResizeEvent *e) {
        m_image = QImage();
        T::resizeEvent(e);
    }

    void paintEvent(QPaintEvent *)
    {
        QPainter pt;
        QPaintDevice *dev = this;
        if (m_deviceType == ImageWidgetType) {
            if (m_image.size() != T::size())
                m_image = QImage(T::size(), QImage::Format_ARGB32_Premultiplied);
            m_image.fill(0);
            dev = &m_image;
        }

        pt.begin(dev);

        PaintCommands paintCommands(m_commands, 800, 800);
        paintCommands.setVerboseMode(m_verboseMode);
        paintCommands.setCheckersBackground(m_checkersBackground);
        paintCommands.setType(m_deviceType);
        paintCommands.setPainter(&pt);
        paintCommands.setControlPoints(m_controlPoints);
        paintCommands.setFilePath(QFileInfo(m_filename).absolutePath());
#ifdef DO_QWS_DEBUGGING
        qt_show_painter_debug_output = true;
#endif
        pt.save();
        paintCommands.runCommands();
        pt.restore();
#ifdef DO_QWS_DEBUGGING
        qt_show_painter_debug_output = false;
#endif

        pt.end();

        if (m_deviceType == ImageWidgetType) {
            QPainter(this).drawImage(0, 0, m_image);
        }

        if (m_currentPoint >= 0 || m_showControlPoints) {
            pt.begin(this);
            pt.setRenderHint(QPainter::Antialiasing);
            pt.setFont(this->font());
            pt.resetMatrix();
            pt.setPen(QColor(127, 127, 127, 191));
            pt.setBrush(QColor(191, 191, 255, 63));
            for (int i=0; i<m_controlPoints.size(); ++i) {
                if (m_showControlPoints || m_currentPoint == i) {
                    QPointF cp = m_controlPoints.at(i);
                    QRectF rect(cp.x() - CP_RADIUS, cp.y() - CP_RADIUS,
                                CP_RADIUS * 2, CP_RADIUS * 2);
                    pt.drawEllipse(rect);
                    pt.drawText(rect, Qt::AlignCenter, QString::number(i));
                }
            }
        }

    }


    void mouseMoveEvent(QMouseEvent *e)
    {
        if (m_currentPoint == -1)
            return;
        if (T::rect().contains(e->pos()))
            m_controlPoints[m_currentPoint] = e->pos();
        T::update();
    }

    void mousePressEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::RightButton) {
            m_showControlPoints = true;
        }

        if (e->button() == Qt::LeftButton) {
            for (int i=0; i<m_controlPoints.size(); ++i) {
                if (QLineF(m_controlPoints.at(i), e->pos()).length() < CP_RADIUS) {
                    m_currentPoint = i;
                    break;
                }
            }
        }
        T::update();
    }

    void mouseReleaseEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton)
            m_currentPoint = -1;
        if (e->button() == Qt::RightButton)
            m_showControlPoints = false;
        T::update();
    }



    QSize sizeHint() const { return QSize(800, 800); }

    QVector<QPointF> m_controlPoints;
    int m_currentPoint;
    bool m_showControlPoints;

    QStringList m_commands;
    QString m_filename;

    bool m_verboseMode;
    bool m_checkersBackground;
    DeviceType m_deviceType;

    QImage m_image;
};

#endif

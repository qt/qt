/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "stationdialog.h"

#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QLinearGradient>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

MainWindow::MainWindow()
    : QWidget(0, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcuts(QKeySequence::Quit);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    QAction *configAction = new QAction(tr("&Select station..."), this);
    configAction->setShortcut(tr("Ctrl+C"));
    connect(configAction, SIGNAL(triggered()), this, SLOT(configure()));

    addAction(configAction);
    addAction(quitAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    setWindowTitle(tr("Traffic Info Oslo"));

    const QSettings settings("Qt Traffic Info", "trafficinfo");
    m_station = StationInformation(settings.value("stationId", "03012130").toString(),
                                   settings.value("stationName", "Nydalen [T-bane] (OSL)").toString());
    m_lines = settings.value("lines", QStringList()).toStringList();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeInformation()));
    timer->start(1000*60*5);
    QMetaObject::invokeMethod(this, SLOT(updateTimeInformation()), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    QSettings settings("Qt Traffic Info", "trafficinfo");
    settings.setValue("stationId", m_station.id());
    settings.setValue("stationName", m_station.name());
    settings.setValue("lines", m_lines);
}

QSize MainWindow::sizeHint() const
{
    return QSize(300, 200);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::paintEvent(QPaintEvent*)
{
    const QPoint start(width()/2, 0);
    const QPoint finalStop(width()/2, height());
    QLinearGradient gradient(start, finalStop);
    const QColor qtGreen(102, 176, 54);
    gradient.setColorAt(0, qtGreen.dark());
    gradient.setColorAt(0.5, qtGreen);
    gradient.setColorAt(1, qtGreen.dark());

    QPainter p(this);
    p.fillRect(0, 0, width(), height(), gradient);

    QFont headerFont("Sans Serif", 12, QFont::Bold);
    QFont normalFont("Sans Serif", 9, QFont::Normal);

    // draw it twice for shadow effect
    p.setFont(headerFont);
    QRect headerRect(1, 1, width(), 25);
    p.setPen(Qt::black);
    p.drawText(headerRect, Qt::AlignCenter, m_station.name());

    headerRect.moveTopLeft(QPoint(0, 0));
    p.setPen(Qt::white);
    p.drawText(headerRect, Qt::AlignCenter, m_station.name());

    p.setFont(normalFont);
    int pos = 40;
    for (int i = 0; i < m_times.count() && i < 9; ++i) {
        p.setPen(Qt::black);
        p.drawText(51, pos + 1, m_times.at(i).time());
        p.drawText(101, pos + 1, m_times.at(i).direction());

        p.setPen(Qt::white);
        p.drawText(50, pos, m_times.at(i).time());
        p.drawText(100, pos, m_times.at(i).direction());

        pos += 18;
    }
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    QBitmap maskBitmap(width(), height());
    maskBitmap.clear();

    QPainter p(&maskBitmap);
    p.setBrush(Qt::black);
    p.drawRoundRect(0, 0, width(), height(), 20, 30);
    p.end();

    setMask(maskBitmap);
}

void MainWindow::updateTimeInformation()
{
    m_times = TimeQuery::query(m_station.id(), m_lines, QDateTime::currentDateTime());

    update();
}

void MainWindow::configure()
{
    StationDialog dlg(m_station.name(), m_lines, this);
    if (dlg.exec()) {
        m_station = dlg.selectedStation();
        m_lines = dlg.lineNumbers();
        updateTimeInformation();
    }
}

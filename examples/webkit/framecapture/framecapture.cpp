/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "framecapture.h"

#include <iostream>
#include <QtWebKit>

FrameCapture::FrameCapture(): QObject(), m_percent(0)
{
    connect(&m_page, SIGNAL(loadProgress(int)), this, SLOT(printProgress(int)));
    connect(&m_page, SIGNAL(loadFinished(bool)), this, SLOT(saveResult(bool)));
}

void FrameCapture::load(const QUrl &url, const QString &outputFileName)
{
    std::cout << "Loading " << qPrintable(url.toString()) << std::endl;
    m_percent = 0;
    int index = outputFileName.lastIndexOf('.');
    m_fileName = (index == -1) ? outputFileName + ".png" : outputFileName;
    m_page.mainFrame()->load(url);
    m_page.mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    m_page.mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    m_page.setViewportSize(QSize(1024, 768));
}

void FrameCapture::printProgress(int percent)
{
    if (m_percent >= percent)
        return;

    while (m_percent++ < percent)
        std::cout << "#" << std::flush;
}

void FrameCapture::saveResult(bool ok)
{
    std::cout << std::endl;

    // crude error-checking
    if (!ok) {
        std::cerr << "Failed loading " << qPrintable(m_page.mainFrame()->url().toString()) << std::endl;
        emit finished();
        return;
    }

    // save each frame in different image files
    saveFrame(m_page.mainFrame());

    emit finished();
}

void FrameCapture::saveFrame(QWebFrame *frame)
{
    static int frameCounter = 0;

    QString fileName(m_fileName);
    if (frameCounter) {
        int index = m_fileName.lastIndexOf('.');
        fileName = fileName.insert(index, "_frame" + QString::number(frameCounter));
    }

    QImage image(frame->contentsSize(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    frame->documentElement().render(&painter);
    painter.end();

    image.save(fileName);

    ++frameCounter;
    foreach(QWebFrame *childFrame, frame->childFrames())
        saveFrame(childFrame);
}


/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <QtGui>
#include <QtDebug>

#include <private/qpaintengineex_p.h>
#include <private/qpaintbuffer_p.h>

class ReplayWidget : public QWidget
{
    Q_OBJECT
public:
    ReplayWidget(const QString &filename);

    void paintEvent(QPaintEvent *event);

public slots:
    void updateRect();

private:
    QList<QRegion> updates;
    QPaintBuffer buffer;

    int currentFrame;
    int currentIteration;
    QTime timer;

    QList<uint> iterationTimes;
    QString filename;
};

void ReplayWidget::updateRect()
{
    update(updates.at(currentFrame));
}

void ReplayWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

//    p.setClipRegion(frames.at(currentFrame).updateRegion);

    buffer.draw(&p, currentFrame);

    ++currentFrame;
    if (currentFrame >= buffer.numFrames()) {
        currentFrame = 0;
        ++currentIteration;

        if (currentIteration == 3)
            timer.start();
        else if (currentIteration > 3) {
            iterationTimes << timer.elapsed();
            timer.restart();

            if (iterationTimes.size() >= 3) {
                qreal mean = 0;
                qreal stddev = 0;
                uint min = INT_MAX;

                for (int i = 0; i < iterationTimes.size(); ++i) {
                    mean += iterationTimes.at(i);
                    min = qMin(min, iterationTimes.at(i));
                }

                mean /= qreal(iterationTimes.size());

                for (int i = 0; i < iterationTimes.size(); ++i) {
                    qreal delta = iterationTimes.at(i) - mean;
                    stddev += delta * delta;
                }

                stddev = qSqrt(stddev / iterationTimes.size());

                qSort(iterationTimes.begin(), iterationTimes.end());
                uint median = iterationTimes.at(iterationTimes.size() / 2);

                stddev = 100 * stddev / mean;

                if (iterationTimes.size() >= 10 || stddev < 4) {
                    printf("%s, iterations: %d, frames: %d, min(ms): %d, median(ms): %d, stddev: %f %%, max(fps): %f\n", qPrintable(filename),
                            iterationTimes.size(), updates.size(), min, median, stddev, 1000. * updates.size() / min);
                    deleteLater();
                    return;
                }
            }
        }
    }

    QTimer::singleShot(0, this, SLOT(updateRect()));
}

ReplayWidget::ReplayWidget(const QString &filename_)
    : currentFrame(0)
    , currentIteration(0)
    , filename(filename_)
{
    setWindowTitle(filename);
    QFile file(filename);

    QRect bounds;
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> buffer >> updates;
    }

    qDebug() << "Read paint buffer with" << buffer.numFrames() << "frames";

    resize(buffer.boundingRect().size().toSize());

    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground);

    QTimer::singleShot(10, this, SLOT(updateRect()));
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if (argc <= 1) {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }

    ReplayWidget *widget = new ReplayWidget(argv[1]);
    widget->show();

    return app.exec();
}
#include "main.moc"

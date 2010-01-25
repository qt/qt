/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
    ReplayWidget(const QString &filename, int from, int to, bool single);

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

public slots:
    void updateRect();

public:
    QList<QRegion> updates;
    QPaintBuffer buffer;

    int currentFrame;
    int currentIteration;
    QTime timer;

    QList<uint> visibleUpdates;
    QList<uint> iterationTimes;
    QString filename;

    int from;
    int to;

    bool single;
};

void ReplayWidget::updateRect()
{
    if (!visibleUpdates.isEmpty())
        update(updates.at(visibleUpdates.at(currentFrame)));
}

void ReplayWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

//    p.setClipRegion(frames.at(currentFrame).updateRegion);

    buffer.draw(&p, visibleUpdates.at(currentFrame));

    ++currentFrame;
    if (currentFrame >= visibleUpdates.size()) {
        currentFrame = 0;
        ++currentIteration;

        if (single) {
            deleteLater();
            return;
        }

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
                            iterationTimes.size(), visibleUpdates.size(), min, median, stddev, 1000. * visibleUpdates.size() / min);
                    deleteLater();
                    return;
                }
            }
        }
    }

    QTimer::singleShot(0, this, SLOT(updateRect()));
}

void ReplayWidget::resizeEvent(QResizeEvent *event)
{
    visibleUpdates.clear();

    QRect bounds = rect();

    int first = qMax(0, from);
    int last = qMin(unsigned(to), unsigned(updates.size()));
    for (int i = first; i < last; ++i) {
        if (updates.at(i).intersects(bounds))
            visibleUpdates << i;
    }

    int range = last - first;

    if (visibleUpdates.size() != range)
        printf("Warning: skipped %d frames due to limited resolution\n", range - visibleUpdates.size());

}

ReplayWidget::ReplayWidget(const QString &filename_, int from_, int to_, bool single_)
    : currentFrame(0)
    , currentIteration(0)
    , filename(filename_)
    , from(from_)
    , to(to_)
    , single(single_)
{
    setWindowTitle(filename);
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        printf("Failed to load input file '%s'\n", qPrintable(filename_));
        return;
    }

    QDataStream in(&file);

    char *data;
    uint size;
    in.readBytes(data, size);
    bool isTraceFile = size == 7 && qstrncmp(data, "qttrace", 7) == 0;
    delete [] data;
    if (!isTraceFile) {
        printf("File '%s' is not a trace file\n", qPrintable(filename_));
        return;
    }

    in >> buffer >> updates;
    printf("Read paint buffer with %d frames\n", buffer.numFrames());

    resize(buffer.boundingRect().size().toSize());

    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground);

    QTimer::singleShot(10, this, SLOT(updateRect()));
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if (argc <= 1 || qstrcmp(argv[1], "-h") == 0 || qstrcmp(argv[1], "--help") == 0) {
        printf("Replays a tracefile generated with '-graphicssystem trace'\n");
        printf("Usage:\n  > %s [OPTIONS] [traceFile]\n", argv[0]);
        printf("OPTIONS\n"
               "   --range=from-to to specify a frame range.\n"
               "   --singlerun to do only one run (without statistics)\n");
        return 1;
    }

    QFile file(app.arguments().last());
    if (!file.exists()) {
        printf("%s does not exist\n", qPrintable(app.arguments().last()));
        return 1;
    }

    bool single = false;

    int from = 0;
    int to = -1;
    for (int i = 1; i < app.arguments().size() - 1; ++i) {
        QString arg = app.arguments().at(i);
        if (arg.startsWith(QLatin1String("--range="))) {
            QString rest = arg.mid(8);
            QStringList components = rest.split(QLatin1Char('-'));

            bool ok1 = false;
            bool ok2 = false;
            int fromCandidate = 0;
            int toCandidate = 0;
            if (components.size() == 2) {
                fromCandidate = components.first().toInt(&ok1);
                toCandidate = components.last().toInt(&ok2);
            }

            if (ok1 && ok2) {
                from = fromCandidate;
                to = toCandidate;
            } else {
                printf("ERROR: malformed syntax in argument %s\n", qPrintable(arg));
            }
        } else if (arg == QLatin1String("--singlerun")) {
            single = true;
        } else {
            printf("Unrecognized argument: %s\n", qPrintable(arg));
            return 1;
        }
    }

    ReplayWidget *widget = new ReplayWidget(app.arguments().last(), from, to, single);

    if (!widget->updates.isEmpty()) {
        widget->show();
        return app.exec();
    }

}
#include "main.moc"

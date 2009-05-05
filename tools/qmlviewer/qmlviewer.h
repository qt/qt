/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ $TROLLTECH$. All rights reserved.
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QMLVIEWER_H
#define QMLVIEWER_H

#include <QWidget>
#include <QBasicTimer>
#include <QTime>
#include <qfxtestengine.h>
#include <QList>


class QFxView;
class PreviewDeviceSkin;
class QFxTestEngine;

class QmlViewer : public QWidget
{
Q_OBJECT
public:
    QmlViewer(QFxTestEngine::TestMode = QFxTestEngine::NoTest, const QString &testDir = QString(), QWidget *parent=0, Qt::WindowFlags flags=0);

    void setRecordDither(const QString& s) { record_dither = s; }
    void setRecordPeriod(int ms);
    void setRecordFile(const QString&);
    int recordPeriod() const { return record_period; }
    void setRecording(bool on);
    bool isRecording() const { return recordTimer.isActive(); }
    void setAutoRecord(int from, int to);
    void setSkin(const QString& skinDirectory);
    void setDeviceKeys(bool);
    void setCacheEnabled(bool);

public slots:
    void sceneResized(QSize size);
    void openQml(const QString& fileName);
    void reload();

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    QString currentFileName;
    PreviewDeviceSkin *skin;
    QSize skinscreensize;
    QFxView *canvas;
    void init(QFxTestEngine::TestMode, const QString &, const QString& fileName);
    QBasicTimer recordTimer;
    QList<QImage*> frames;
    QIODevice* frame_stream;
    QBasicTimer autoStartTimer;
    QTime autoTimer;
    QString record_dither;
    QString record_file;
    int record_period;
    int record_autotime;
    bool devicemode;

    QFxTestEngine *testEngine;
};

#endif

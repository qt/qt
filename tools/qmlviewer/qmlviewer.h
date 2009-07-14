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

#include <QMenuBar>
#include <QBasicTimer>
#include <QTime>
#include <qfxtestengine.h>
#include <QList>

QT_BEGIN_NAMESPACE

class QFxView;
class PreviewDeviceSkin;
class QFxTestEngine;
class QmlPalette;
class QProcess;

class QmlViewer : public QWidget
{
Q_OBJECT
public:
    QmlViewer(QFxTestEngine::TestMode = QFxTestEngine::NoTest, const QString &testDir = QString(), QWidget *parent=0, Qt::WindowFlags flags=0);

    void setRecordDither(const QString& s) { record_dither = s; }
    void setRecordPeriod(int ms);
    void setRecordFile(const QString&);
    void setRecordArgs(const QStringList&);
    int recordPeriod() const { return record_period; }
    void setRecording(bool on);
    bool isRecording() const { return recordTimer.isActive(); }
    void setAutoRecord(int from, int to);
    void setDeviceKeys(bool);
    void setCacheEnabled(bool);
    void addLibraryPath(const QString& lib);

    QMenuBar *menuBar() const;

public slots:
    void sceneResized(QSize size);
    void openQml(const QString& fileName);
    void open();
    void reload();
    void takeSnapShot();
    void toggleRecording();
    void toggleRecordingWithSelection();
    void ffmpegFinished(int code);
    void setSkin(const QString& skinDirectory);

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual bool event(QEvent *event);

    void createMenu(QMenuBar *menu, QMenu *flatmenu);

private slots:
    void setScaleSkin();
    void setScaleView();

private:
    void setupProxy();
    void setupPalettes();

    QString currentFileName;
    PreviewDeviceSkin *skin;
    QSize skinscreensize;
    QFxView *canvas;
    QmlPalette *palette;
    QmlPalette *disabledPalette;
    void init(QFxTestEngine::TestMode, const QString &, const QString& fileName);
    QBasicTimer recordTimer;
    QList<QImage*> frames;
    QProcess* frame_stream;
    QBasicTimer autoStartTimer;
    QTime autoTimer;
    QString record_dither;
    QString record_file;
    QStringList record_args;
    int record_period;
    int record_autotime;
    bool devicemode;
    QAction *recordAction;
    QString currentSkin;
    bool scaleSkin;
    mutable QMenuBar *mb;

    QFxTestEngine *testEngine;
};

QT_END_NAMESPACE

#endif

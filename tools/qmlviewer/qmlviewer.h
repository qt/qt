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
#include <QmlTimer>
#include <QTime>
#include <QList>

QT_BEGIN_NAMESPACE

class QFxView;
class PreviewDeviceSkin;
class QFxTestEngine;
class QProcess;
class RecordingDialog;

class QmlViewer : public QWidget
{
Q_OBJECT
public:
    QmlViewer(QWidget *parent=0, Qt::WindowFlags flags=0);

    void setRecordDither(const QString& s) { record_dither = s; }
    void setRecordRate(int fps);
    void setRecordFile(const QString&);
    void setRecordArgs(const QStringList&);
    void setRecording(bool on);
    bool isRecording() const { return recordTimer.isRunning(); }
    void setAutoRecord(int from, int to);
    void setDeviceKeys(bool);
    void setNetworkCacheSize(int size);
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

    void createMenu(QMenuBar *menu, QMenu *flatmenu);

private slots:
    void setScaleSkin();
    void setScaleView();
    void autoStartRecording();
    void autoStopRecording();
    void recordFrame();
    void chooseRecordingOptions();
    void pickRecordingFile();

private:
    void setupProxy();
    QString getVideoFileName();

    QString currentFileName;
    PreviewDeviceSkin *skin;
    QSize skinscreensize;
    QFxView *canvas;
    QmlTimer recordTimer;
    QString frame_fmt;
    QImage frame;
    QList<QImage*> frames;
    QProcess* frame_stream;
    QmlTimer autoStartTimer;
    QmlTimer autoStopTimer;
    QString record_dither;
    QString record_file;
    QSize record_outsize;
    QStringList record_args;
    int record_rate;
    int record_autotime;
    bool devicemode;
    QAction *recordAction;
    QString currentSkin;
    bool scaleSkin;
    mutable QMenuBar *mb;
    RecordingDialog *recdlg;

    void senseImageMagick();
    void senseFfmpeg();
    QWidget *ffmpegHelpWindow;
    bool ffmpegAvailable;
    bool convertAvailable;
};

QT_END_NAMESPACE

#endif

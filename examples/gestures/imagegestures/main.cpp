/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
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

#include <QtGui>

#include "mainwidget.h"
#include "imagewidget.h"

static const char helpText[] =
"Usage: ./imagegestures [options] Directory\n\
Image Gestures Example\n\
\n\
Options:\n\
  --no-pan    Disable pan gesture\n\
  --no-pinch  Disable pinch gesture\n\
  --no-swipe  Disable swipe gesture\n\
  -h, --help  Displays this help.\n\
\n\
Arguments:\n\
  Directory   Directory to display\n";

static void showHelp(const QString errorMessage = QString())
{
    QString text;
    QTextStream str(&text);
    str << "<html><head/><body>";
    if (!errorMessage.isEmpty())
        str << "<p>" << errorMessage << "</p>";
    str << "<pre>" << helpText << "</pre></body></html>";
    QMessageBox box(errorMessage.isEmpty() ? QMessageBox::Information : QMessageBox::Warning,
                    QLatin1String("Image Gestures Example"), text,
                    QMessageBox::Ok);
    box.exec();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCoreApplication::setApplicationName(QLatin1String("imagegestures"));

    QStringList arguments = QApplication::arguments();

    QString directory;
    QList<Qt::GestureType> gestures;
    gestures << Qt::PanGesture << Qt::PinchGesture << Qt::SwipeGesture;
    for (int a = 1; a < arguments.size(); ++a) {
        const QString &argument = arguments.at(a);
        if (argument.startsWith(QLatin1Char('-'))) {
            if (argument == QLatin1String("--no-pan"))
                gestures.removeAll(Qt::PanGesture);
            else if (argument == QLatin1String("--no-pinch"))
                gestures.removeAll(Qt::PinchGesture);
            else if (argument == QLatin1String("--no-swipe"))
                gestures.removeAll(Qt::SwipeGesture);
            else if (argument == QLatin1String("--verbose"))
                ImageWidget::setVerbose(true);
            else if (argument == QLatin1String("-h") || argument == QLatin1String("--help")) {
                showHelp();
                return 0;
            } else {
                showHelp(QLatin1String("Invalid option: ") + argument);
                return 1;
            }
        } else
            directory = argument;
    }

    MainWidget w;
    w.grabGestures(gestures);
    w.show();

    if (directory.isEmpty()) {
        const QString picturesLocation = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
        directory =
            QFileDialog::getExistingDirectory(&w, "Select image folder",
                                              picturesLocation.isEmpty() ? QString() : picturesLocation);
        if (directory.isEmpty())
            return 0;
    }

    w.openDirectory(directory);

    return app.exec();
}

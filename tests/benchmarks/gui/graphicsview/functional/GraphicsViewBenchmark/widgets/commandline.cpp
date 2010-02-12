/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QStringList>
#include <QDebug>

#include "commandline.h"

static void usage(const char *appname)
{
    printf("%s [options]\n", appname);
    printf("Options:\n");
    printf("\t -h,-help       : This help\n");
#ifdef AUTO_TESTS
    printf("\t -o file        : Write output to file\n");
    printf("\t -xml           : Outputs results as XML document\n");
    printf("\t -lightxml      : Outputs results as stream of XML tags\n");
    printf("\t -script-name file  : Use this script instead of internal script file\n");
#endif
    printf("\t -resolution    : UI resolution in format WxH where width and height are positive values\n");
    printf("\t -rotation      : UI rotation in degrees\n");
    printf("\t -subtree-cache : Enables usage of subtree caching method\n");
    printf("\t -noresusage : Disables CPU and Memory usage measurement\n");
#ifndef  AUTO_TESTS
    printf("\t -fps           : Output FPS count to stdout during application execution\n");    
    printf("\t -items         : Count of items created to the list\n");
#endif
#if ENABLE_OPENGL
#ifndef QT_NO_OPENGL
    printf("\t -opengl        : Enables OpenGL usage. Building PRECONDITIONS: ENABLE_OPENGL is on. QT_NO_OPENGL is off.\n");
#endif
#endif
    printf("\n");
}

bool readSettingsFromCommandLine(int argc, char *argv[],
                  Settings& config)
{
    bool builtWithOpenGL = false;
    Settings::Options options;

#if ENABLE_OPENGL 
#ifndef QT_NO_OPENGL
    builtWithOpenGL = true;
#endif
#endif
    for (int i=0; i<argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0) {
            usage(argv[0]);
            return false;
        }
#ifdef AUTO_TESTS
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                printf("-o needs an extra parameter specifying the filename\n");
                usage(argv[0]);
                return false;
            } else {
                config.setOutputFileName(QString(argv[i+1]));
                i++;
            }
        }
        if (strcmp(argv[i], "-xml") == 0) {
            config.setResultFormat(1);  // See FileLogger::ResultFormat
        }
        if (strcmp(argv[i], "-lightxml") == 0) {
            config.setResultFormat(2);  // See FileLogger::ResultFormat
        }
        if (strcmp(argv[i], "-script-name") == 0) {
            if (i + 1 >= argc) {
                printf("-script-name  needs an extra parameter specifying the filename\n");
                usage(argv[0]);
                return false;
            } else {
                config.setScriptName(QString(argv[i+1]));
                i++;
            }
        }
#endif
        if (strcmp(argv[i], "-resolution") == 0) {
            if (i + 1 >= argc) {
                printf("-resolution needs an extra parameter specifying the application UI resolution\n");
                usage(argv[0]);
                return false;
            }
            else {
                QStringList res = QString(argv[i+1]).split("x");
                if (res.count() != 2) {
                    printf("-resolution parameter UI resolution should be set in format WxH where width and height are positive values\n");
                    usage(argv[0]);
                    return false;
                }
                int width = res.at(0).toInt();
                int height = res.at(1).toInt();

                config.setSize(QSize(width, height));

                if (width <=0 || height <=0) {
                    printf("-resolution parameter UI resolution should be set in format WxH where width and height are positive values\n");
                    usage(argv[0]);
                    return false;
                }
                i++;
            }        
        }
        if (strcmp(argv[i], "-rotation") == 0) {
            if (i + 1 >= argc) {
                printf("-rotation needs an extra parameter specifying the application UI rotation in degrees\n");
                usage(argv[0]);
                return false;
            }
            else {
                bool ok;
                int angle = QString(argv[i+1]).toInt(&ok);
                if (!ok) {
                    printf("-rotation parameter should specify rotation angle in degrees\n");
                    usage(argv[0]);
                    return false;
                }
                config.setAngle(angle);
                i++;
            }        
        }
        if (strcmp(argv[i], "-subtree-cache") == 0) {
            options |= Settings::UseListItemCache;
        }
        if (strcmp(argv[i], "-opengl") == 0) {
            if (builtWithOpenGL)
                options |= Settings::UseOpenGL;
            else {
                printf("-opengl parameter can be used only with building PRECONDITIONS: ENABLE_OPENGL is on. QT_NO_OPENGL is off.\n");
                usage(argv[0]);
                return false;
            }
        }
        if (strcmp(argv[i], "-noresusage") == 0) {
             options |= Settings::NoResourceUsage;
        }
#ifndef AUTO_TESTS
        if (strcmp(argv[i], "-fps") == 0) {
             options |= Settings::OutputFps;
        }
        if (strcmp(argv[i], "-items") == 0) {
            if (i + 1 >= argc) {
                printf("-items needs an extra parameter specifying amount of list items\n");
                usage(argv[0]);
                return false;
            }
            else {
                bool ok;
                int amount = QString(argv[i+1]).toInt(&ok);
                if (!ok) {
                    printf("-items needs an extra parameter specifying amount (integer) of list items\n");
                    usage(argv[0]);
                    return false;
                }
                config.setListItemCount(amount);
                i++;
            }
        }
#endif
    }

    config.setOptions(options);

    return true;
}


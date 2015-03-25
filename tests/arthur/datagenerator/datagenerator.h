/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include "xmlgenerator.h"
#include "framework.h"

#include <QTextStream>
#include <QSettings>
#include <QSize>
#include <QColor>

QT_FORWARD_DECLARE_CLASS(QSvgRenderer)
QT_FORWARD_DECLARE_CLASS(QEngine)
QT_FORWARD_DECLARE_CLASS(QFileInfo)

class DataGenerator
{
public:
    DataGenerator();
    ~DataGenerator();

    void run(int argc, char **argv);
private:
    bool processArguments(int argc, char **argv);
    void testEngines(XMLGenerator &generator, const QString &file,
                     const QString &refUrl);
    void testDirectory(const QString &dirname, const QString &refUrl);
    void testFile(const QString &file, const QString &refUrl,
                  QTextStream &out, QTextStream &hout);
    void testGivenFile();
    void testSuite(XMLGenerator &generator, const QString &suite,
                   const QString &dirName, const QString &refUrl);
    void prepareDirs();

    void testGivenEngines(const QList<QEngine*> engines,
                          const QFileInfo &fileInfo,
                          const QString &file,
                          XMLGenerator &generator,
                          GeneratorFlags flags);
    void testGivenEngines(const QList<QEngine*> engines,
                          const QFileInfo &fileInfo,
                          const QString &file,
                          XMLGenerator &generator,
                          int iterations,
                          GeneratorFlags flags);

    bool wantedEngine(const QString &engine) const;
private:
    QSvgRenderer *renderer;
    Framework settings;

    QString engineName;
    QString suiteName;
    QString testcase;
    QString fileName;
    QString outputDirName;
    QString baseDataDir;
    int     iterations;
    QSize   size;
    QColor  fillColor;
};

#endif

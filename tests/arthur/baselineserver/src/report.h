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
#ifndef REPORT_H
#define REPORT_H

#include "baselineprotocol.h"
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QStringList>

class BaselineHandler;

class Report
{
public:
    Report();
    ~Report();

    void init(const BaselineHandler *h, const QString &r, const PlatformInfo &p);
    void addItems(const ImageItemList& items);
    void addMismatch(const ImageItem& item);
    void end();

    QString filePath();

    static void handleCGIQuery(const QString &query);

private:
    void write();
    void writeFunctionResults(const ImageItemList &list);
    void writeItem(const QString &baseline, const QString &rendered, const ImageItem &item,
                   const QString &itemFile, const QString &ctx, const QString &misCtx, const QString &metadata);
    void writeHeader();
    void writeFooter();
    QString generateCompared(const QString &baseline, const QString &rendered, bool fuzzy = false);
    QString generateThumbnail(const QString &image);

    const BaselineHandler *handler;
    QString runId;
    PlatformInfo plat;
    QString rootDir;
    QString reportDir;
    QString path;
    QStringList testFunctions;
    QMap<QString, ImageItemList> itemLists;
    bool written;
    int numItems;
    int numMismatches;
    QTextStream out;
    bool hasOverride;
};

#endif // REPORT_H

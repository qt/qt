/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef GBUILD_H
#define GBUILD_H

#include "makefile.h"

QT_BEGIN_NAMESPACE

class GBuildMakefileGenerator : public MakefileGenerator
{
    virtual bool write();

    QString projectSuffix() const { return QString(".gpj"); };
    QString writeOne(QString filename, QString pathtoremove = "");

public:
    GBuildMakefileGenerator();
    ~GBuildMakefileGenerator();

    virtual bool supportsMetaBuild() { return false; }
    virtual bool openOutput(QFile &, const QString &) const;
protected:
    bool doPrecompiledHeaders() const { return false; }
    virtual bool doDepends() const { return true; }
    QStringList nativebins;

};

inline GBuildMakefileGenerator::~GBuildMakefileGenerator()
{ }

QT_END_NAMESPACE

#endif // GBUILD_H

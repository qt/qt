/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef INITPROJECTDEPLOYSYMBIAN_H
#define INITPROJECTDEPLOYSYMBIAN_H

#include <qstring.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <option.h>
#include <qdir.h>
#include <qfile.h>
#include <stdlib.h>

struct CopyItem
{
    CopyItem(const QString& f, const QString& t) : from(f) , to(t) { }
    QString from;
    QString to;
};
typedef QList<CopyItem> DeploymentList;

extern void initProjectDeploySymbian(QMakeProject* project,
                              DeploymentList &deploymentList,
                              const QString &testPath,
                              bool deployBinaries,
                              const QString &platform,
                              const QString &build,
                              QStringList& generatedDirs,
                              QStringList& generatedFiles);
#endif // INITPROJECTDEPLOYSYMBIAN_H
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
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

#ifndef MSVC_VCXPROJ_H
#define MSVC_VCXPROJ_H

#include "winmakefile.h"
#include "msbuild_objectmodel.h"
#include "msvc_vcproj.h"

QT_BEGIN_NAMESPACE

class VcxprojGenerator : public VcprojGenerator
{
    bool writeMakefile(QTextStream &);
    bool writeProjectMakefile();

public:
    VcxprojGenerator();
    ~VcxprojGenerator();

protected:
    virtual bool supportsMetaBuild() { return true; }
    virtual bool supportsMergedBuilds() { return true; }
    virtual bool mergeBuildProject(MakefileGenerator *other);

    virtual void initProject();

    void initConfiguration();
    void initCompilerTool();
    void initDeploymentTool();
    void initLinkerTool();
    void initPreLinkEventTools();
    void initPostBuildEventTools();
    void initRootFiles();
    void initResourceTool();
    void initSourceFiles();
    void initHeaderFiles();
    void initGeneratedFiles();
    void initTranslationFiles();
    void initFormFiles();
    void initResourceFiles();
    void initLexYaccFiles();
    void initExtraCompilerOutputs();

    // Used for single project
    VCXProjectSingleConfig vcxProject;

    // Holds all configurations for glue (merged) project
    QList<VcxprojGenerator*> mergedProjects;

private:
    friend class VCXFilter;

};

inline VcxprojGenerator::~VcxprojGenerator()
{ }

QT_END_NAMESPACE

#endif // MSVC_VCXPROJ_H

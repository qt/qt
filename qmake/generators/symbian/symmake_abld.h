/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef SYMMAKE_ABLD_H
#define SYMMAKE_ABLD_H

#include <symmake.h>

QT_BEGIN_NAMESPACE

class SymbianAbldMakefileGenerator : public SymbianMakefileGenerator {

protected:

    // Inherited from parent
    virtual bool writeBldInfExtensionRulesPart(QTextStream& t);
    virtual void writeBldInfMkFilePart(QTextStream& t, bool addDeploymentExtension);
    virtual bool writeMkFile(const QString& wrapperFileName, bool deploymentOnly);
    virtual void writeWrapperMakefile(QFile& wrapperFile, bool isPrimaryMakefile);

    bool writeDeploymentTargets(QTextStream &t);

public:

    SymbianAbldMakefileGenerator();
    inline ~SymbianAbldMakefileGenerator();

};

#endif // SYMMAKE_ABLD_H


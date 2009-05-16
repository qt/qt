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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_XsdParticleChecker_H
#define Patternist_XsdParticleChecker_H

#include "qxsdelement_p.h"
#include "qxsdparticle_p.h"
#include "qxsdschemacontext_p.h"
#include "qxsdwildcard_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A helper class to check validity of particles.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdParticleChecker
    {
        public:
            /**
             * Checks whether the given @p particle has two or more element
             * declarations with the same name but different type definitions.
             */
            static bool hasDuplicatedElements(const XsdParticle::Ptr &particle, const NamePool::Ptr &namePool, XsdElement::Ptr &conflictingElement);

            /**
             * Checks whether the given @p particle is valid according the
             * UPA (http://www.w3.org/TR/xmlschema-1/#cos-nonambig) constraint.
             */
            static bool isUPAConform(const XsdParticle::Ptr &particle, const NamePool::Ptr &namePool);

            /**
             * Checks whether the given @p particle subsumes the given @p derivedParticle.
             * (http://www.w3.org/TR/xmlschema-1/#cos-particle-restrict)
             */
            static bool subsumes(const XsdParticle::Ptr &particle, const XsdParticle::Ptr &derivedParticle, const XsdSchemaContext::Ptr &context, QString &errorMsg);
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

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

#ifndef Patternist_XsdAnnotated_H
#define Patternist_XsdAnnotated_H

#include "qxsdannotation_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Base class for all XSD components with annotation content.
     *
     * @ingroup Patternist_schema
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
     */
    class XsdAnnotated
    {
        public:
            /**
             * Adds a new @p annotation to the component.
             */
            void addAnnotation(const XsdAnnotation::Ptr &annotation);

            /**
             * Adds a list of new @p annotations to the component.
             */
            void addAnnotations(const XsdAnnotation::List &annotations);

            /**
             * Returns the list of all annotations of the component.
             */
            XsdAnnotation::List annotations() const;

        private:
            XsdAnnotation::List m_annotations;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

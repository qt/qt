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

#ifndef Patternist_ComparisonFactory_H
#define Patternist_ComparisonFactory_H

#include "qatomiccomparator_p.h"
#include "qderivedstring_p.h"
#include "qitem_p.h"
#include "qreportcontext_p.h"
#include "qschematype_p.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Provides compare(), which is a high-level helper function for
     * comparing atomic values.
     *
     * This class wraps the helper class ComparisonPlatform with a more specific,
     * high-level API.
     *
     * @see ComparisonPlatform
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_schema
     */
    class ComparisonFactory
    {
    public:
        /**
         * @short Returns the result of evaluating operator @p op applied to the atomic
         * values @p operand1 and @p operand2.
         *
         * The caller guarantees that both values are of type @p type.
         *
         * ComparisonFactory does not take ownership of @p sourceLocationReflection.
         */
        static bool compare(const AtomicValue::Ptr &operand1,
                            const AtomicComparator::Operator op,
                            const AtomicValue::Ptr &operand2,
                            const SchemaType::Ptr &type,
                            const ReportContext::Ptr &context,
                            const SourceLocationReflection *const sourceLocationReflection);

        /**
         * @short Returns the result of evaluating operator @p op applied to the atomic
         * values @p operand1 and @p operand2.
         *
         * In opposite to compare() it converts the operands from string type
         * to @p type and compares these constructed types.
         *
         * The caller guarantees that both values are of type @p type.
         *
         * ComparisonFactory does not take ownership of @p sourceLocationReflection.
         */
        static bool constructAndCompare(const DerivedString<TypeString>::Ptr &operand1,
                                        const AtomicComparator::Operator op,
                                        const DerivedString<TypeString>::Ptr &operand2,
                                        const SchemaType::Ptr &type,
                                        const ReportContext::Ptr &context,
                                        const SourceLocationReflection *const sourceLocationReflection);

    private:
        Q_DISABLE_COPY(ComparisonFactory)
    };
}

QT_END_NAMESPACE
QT_END_HEADER

#endif

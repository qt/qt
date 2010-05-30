/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtDebug>

#include "qabstractfloat_p.h"
#include "qandexpression_p.h"
#include "qanyuri_p.h"
#include "qapplytemplate_p.h"
#include "qargumentreference_p.h"
#include "qarithmeticexpression_p.h"
#include "qatomicstring_p.h"
#include "qatomizer_p.h"
#include "qattributeconstructor_p.h"
#include "qattributenamevalidator_p.h"
#include "qaxisstep_p.h"
#include "qbase64binary_p.h"
#include "qboolean_p.h"
#include "qcardinalityverifier_p.h"
#include "qcastableas_p.h"
#include "qcastas_p.h"
#include "qcombinenodes_p.h"
#include "qcontextitem_p.h"
#include "qdate_p.h"
#include "qdecimal_p.h"
#include "qdynamiccontextstore_p.h"
#include "qelementconstructor_p.h"
#include "qemptysequence_p.h"
#include "qevaluationcache_p.h"
#include "qexpressionsequence_p.h"
#include "qexpressionvariablereference_p.h"
#include "qfirstitempredicate_p.h"
#include "qforclause_p.h"
#include "qfunctioncall_p.h"
#include "qgday_p.h"
#include "qgeneralcomparison_p.h"
#include "qgenericpredicate_p.h"
#include "qgmonthday_p.h"
#include "qgmonth_p.h"
#include "qgyearmonth_p.h"
#include "qgyear_p.h"
#include "qhexbinary_p.h"
#include "qifthenclause_p.h"
#include "qinstanceof_p.h"
#include "qinteger_p.h"
#include "qitem_p.h"
#include "qitemverifier_p.h"
#include "qliteral_p.h"
#include "qnamespaceconstructor_p.h"
#include "qncnameconstructor_p.h"
#include "qnodecomparison_p.h"
#include "qorexpression_p.h"
#include "qpath_p.h"
#include "qpositionalvariablereference_p.h"
#include "qqnameconstructor_p.h"
#include "qqnamevalue_p.h"
#include "qquantifiedexpression_p.h"
#include "qrangeexpression_p.h"
#include "qrangevariablereference_p.h"
#include "qschemadatetime_p.h"
#include "qschematime_p.h"
#include "qsimplecontentconstructor_p.h"
#include "qtreatas_p.h"
#include "qtruthpredicate_p.h"
#include "quntypedatomicconverter_p.h"
#include "quntypedatomic_p.h"
#include "quserfunctioncallsite_p.h"
#include "qvalidationerror_p.h"
#include "qvaluecomparison_p.h"

#include "ExpressionInfo.h"
#include "Global.h"

#include "ExpressionNamer.h"

using namespace QPatternistSDK;

/* Simple ones, they have no additional data. */
#define implClass(cls)                                                                              \
QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::cls *) const    \
{                                                                                                   \
    return QPatternist::ExpressionVisitorResult::Ptr                                                \
           (new ExpressionInfo(QLatin1String(#cls), QString()));                                    \
}

implClass(AndExpression)
implClass(ArgumentConverter)
implClass(Atomizer)
implClass(AttributeConstructor)
implClass(AttributeNameValidator)
implClass(CallTemplate)
implClass(CardinalityVerifier)
implClass(CollationChecker)
implClass(CommentConstructor)
implClass(ComputedNamespaceConstructor)
implClass(ContextItem)
implClass(CopyOf)
implClass(CurrentItemStore)
implClass(DocumentConstructor)
implClass(DynamicContextStore)
implClass(EBVExtractor)
implClass(ElementConstructor)
implClass(EmptySequence)
implClass(ExpressionSequence)
implClass(ExternalVariableReference)
implClass(FirstItemPredicate)
implClass(ForClause)
implClass(GenericPredicate)
implClass(IfThenClause)
implClass(ItemVerifier)
implClass(LetClause)
implClass(LiteralSequence)
implClass(NCNameConstructor)
implClass(NodeSortExpression)
implClass(OrderBy)
implClass(OrExpression)
implClass(ParentNodeAxis)
implClass(ProcessingInstructionConstructor)
implClass(QNameConstructor)
implClass(RangeExpression)
implClass(ReturnOrderBy)
implClass(SimpleContentConstructor)
implClass(StaticBaseURIStore)
implClass(StaticCompatibilityStore)
implClass(TemplateParameterReference)
implClass(TextNodeConstructor)
implClass(TreatAs)
implClass(TruthPredicate)
implClass(UnresolvedVariableReference)
implClass(UntypedAtomicConverter)
implClass(UserFunctionCallsite)
implClass(ValidationError)
#undef implClass

/** Variable references. */
#define implVarRef(name)                                                                            \
QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::name *i) const    \
{                                                                                                   \
    return QPatternist::ExpressionVisitorResult::Ptr                                                 \
           (new ExpressionInfo(QLatin1String(#name),                                                \
                               QString(QLatin1String("Slot: %1")).arg(i->slot())));                 \
}
implVarRef(RangeVariableReference)
implVarRef(ArgumentReference)
implVarRef(ExpressionVariableReference)
implVarRef(PositionalVariableReference)
#undef implVarRef

/* Type related classes which have a targetType() function. */
#define implTypeClass(cls)                                                                          \
QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::cls *i) const     \
{                                                                                                   \
    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(QLatin1String(#cls),         \
                                                i->targetType()->displayName(Global::namePool()))); \
}

implTypeClass(InstanceOf)
implTypeClass(CastableAs)
#undef implTypeClass

/* Type related classes which have a targetType() function. */
QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::CastAs *i) const
{
    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(QLatin1String("CastAs"),
                                                i->targetSequenceType()->displayName(Global::namePool())));
}

/* Classes which represent operators. */
#define implOPClass(cls, compClass)                                                                     \
QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::cls *i) const         \
{                                                                                                       \
    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(QLatin1String(#cls),             \
                                                QPatternist::compClass::displayName(i->operatorID())));  \
}

implOPClass(ArithmeticExpression,   AtomicMathematician)
implOPClass(NodeComparison,         NodeComparison)
implOPClass(QuantifiedExpression,   QuantifiedExpression)
implOPClass(CombineNodes,            CombineNodes)
#undef implOPClass

/* Classes which represent operators. */
#define implCompClass(cls, type)                                                                \
QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::cls *i) const \
{                                                                                               \
    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(QLatin1String(#cls),     \
                                    QPatternist::AtomicComparator::displayName(i->operatorID(),  \
                                                    QPatternist::AtomicComparator::type)));      \
}

implCompClass(GeneralComparison,    AsGeneralComparison)
implCompClass(ValueComparison,      AsValueComparison)
#undef implCompClass

QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::FunctionCall *i) const
{
    return QPatternist::ExpressionVisitorResult::Ptr
           (new ExpressionInfo(QLatin1String("FunctionCall"),
                               Global::namePool()->displayName(i->signature()->name())));
}

QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::Literal *i) const
{
    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(
                                                    i->item().type()->displayName(Global::namePool()),
                                                    i->item().stringValue()));
}

QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::AxisStep *i) const
{
    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(QLatin1String("AxisStep"),
                                                                        QPatternist::AxisStep::axisName(i->axis()) +
                                                                        QLatin1String("::") +
                                                                        i->nodeTest()->displayName(Global::namePool())));

}


QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::EvaluationCache<true> *i) const
{
    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(QLatin1String("EvaluationCache<IsForGlobal=true>"),
                                                                        QLatin1String("Slot: ") + QString::number(i->slot())));

}

QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::EvaluationCache<false> *i) const
{
    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(QLatin1String("EvaluationCache<IsForGlobal=false>"),
                                                                        QLatin1String("Slot: ") + QString::number(i->slot())));

}

QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::NamespaceConstructor *i) const
{
    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(QLatin1String("NamespaceConstructor"),
                                                                        Global::namePool()->stringForPrefix(i->namespaceBinding().prefix()) +
                                                                        QLatin1Char('=') +
                                                                        Global::namePool()->stringForNamespace(i->namespaceBinding().namespaceURI())));

}

QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::Path *path) const
{

    QPatternist::Path::Kind k = path->kind();
    QString type;

    switch(k)
    {
        case QPatternist::Path::XSLTForEach:
        {
            type = QLatin1String("XSLTForEach");
            break;
        }
        case QPatternist::Path::RegularPath:
        {
            type = QLatin1String("RegularPath");
            break;
        }
        case QPatternist::Path::ForApplyTemplate:
        {
            type = QLatin1String("ForApplyTemplate");
            break;
        }
    }

    return QPatternist::ExpressionVisitorResult::Ptr(new ExpressionInfo(QLatin1String("Path"), type));
}

QPatternist::ExpressionVisitorResult::Ptr ExpressionNamer::visit(const QPatternist::ApplyTemplate *path) const
{
    const QPatternist::TemplateMode::Ptr mode(path->mode());
    return QPatternist::ExpressionVisitorResult::Ptr
           (new ExpressionInfo(QLatin1String("ApplyTemplate"), mode ? Global::namePool()->displayName(mode->name()) : QString::fromLatin1("#current")));
}

// vim: et:ts=4:sw=4:sts=4


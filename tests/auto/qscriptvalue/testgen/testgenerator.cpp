/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include "testgenerator.h"

#include <QtCore/qdatastream.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>
#include <QtCore/qnumeric.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qvariant.h>
#include <QtScript/qscriptvalue.h>

void TestGenerator::save(const QString& data)
{
    QTextStream out(&m_ofile);
    out << data;
}

static QString escape(QString txt)
{
    return txt.replace("\\","\\\\").replace("\"","\\\"").replace("\n","\\n");
}

template<typename T>
QString prepareToInsert(T value) {return QString::fromAscii("\"") + escape(value) + "\"";}
template<>
QString prepareToInsert<qsreal>(qsreal value)
{
    if (qIsNaN(value))
        return "qQNaN()";
    if (qIsInf(value))
        return "qInf()";
    return QString::number(value, 'g', 16);
}
template<>
QString prepareToInsert<qint32>(qint32 value) {return QString::number(value);}
template<>
QString prepareToInsert<quint32>(quint32 value) {return QString::number(value);}
template<>
QString prepareToInsert<quint16>(quint16 value) {return QString::number(value);}
template<>
QString prepareToInsert<bool>(bool value) {return value ? "true" : "false";}
template<>
QString prepareToInsert<QString>(QString value) {return QString::fromAscii("\"") + escape(value) + "\"";}

template<typename T>
QString typeName() {return QString();}
template<>
QString typeName<qsreal>() {return "qsreal";}
template<>
QString typeName<qint32>() {return "qint32";}
template<>
QString typeName<quint32>() {return "quint32";}
template<>
QString typeName<quint16>() {return "quint16";}
template<>
QString typeName<bool>() {return "bool";}
template<>
QString typeName<QString>() {return "QString";}

static QString generateIsXXXDef(const QString& name, const QList<QString>& list)
{
    static const QString templ("void tst_QScriptValue::%1_initData()\n"\
                               "{\n"\
                               "    QTest::addColumn<bool>(\"expected\");\n"\
                               "    initScriptValues();\n"\
                               "}\n"\
                               "\n"\
                               "void tst_QScriptValue::%1_makeData(const char* expr)\n"\
                               "{\n"\
                               "    static QSet<QString> %1;\n"\
                               "    if (%1.isEmpty()) {\n"\
                               "        %1%2\n"\
                               "    }\n"\
                               "    newRow(expr) << %1.contains(expr);\n"\
                               "}\n"\
                               "\n"\
                               "void tst_QScriptValue::%1_test(const char*, const QScriptValue& value)\n"\
                               "{\n"\
                               "    QFETCH(bool, expected);\n"\
                               "    QCOMPARE(value.%1(), expected);\n"\
                               "}\n"\
                               "\n"\
                               "DEFINE_TEST_FUNCTION(%1)\n"\
                               "\n");

    if (!list.size()) {
        qWarning() << name << ": nothing to add!" ;
        return QString();
    }

    QString result = templ;
    QStringList set;
    foreach(QString t, list) {
        t = escape(t);
        t.append('\"');
        t.prepend('\"');
        set.append(QString(" << "));
        set.append(t);
        set.append("\n               ");
    }
    set.append(";");
    return result.arg(name, set.join(QString()));
}

template<typename T>
static QString generateToXXXDef(const QString& name, const QList<QPair<QString, T> >& list)
{
    static const QString templ = "\n"\
                                 "void tst_QScriptValue::%1_initData()\n"\
                                 "{\n"\
                                 "    QTest::addColumn<%2>(\"expected\");\n"\
                                 "    initScriptValues();\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::%1_makeData(const char* expr)\n"\
                                 "{\n"\
                                 "    static QHash<QString, %2> %1;\n"\
                                 "    if (%1.isEmpty()) {\n"\
                                 "%3"\
                                 "    }\n"\
                                 "    newRow(expr) << %1.value(expr);\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::%1_test(const char*, const QScriptValue& value)\n"\
                                 "{\n"\
                                 "    QFETCH(%2, expected);\n"\
                                 "    QCOMPARE(value.%1(), expected);\n"\
                                 "}\n"\
                                 "\n"\
                                 "DEFINE_TEST_FUNCTION(%1)\n";
    QString result = templ;

    typename QList<QPair<QString, T> >::const_iterator i = list.constBegin();
    QStringList set;
    for(; i != list.constEnd(); ++i) {
        QPair<QString, T> t = *i;
        t.first = escape(t.first);
        set.append(QString("        "));
        set.append(name);
        set.append(".insert(\"");
        set.append(t.first);
        set.append(QString::fromAscii("\", "));
        set.append(prepareToInsert<T>(t.second));
        set.append(QString::fromAscii(");\n"));
    }
    return result.arg(name, typeName<T>(), set.join(QString()));
}


template<>
QString generateToXXXDef<qsreal>(const QString& name, const QList<QPair<QString, qsreal> >& list)
{
    static const QString templ = "\n"\
                                 "void tst_QScriptValue::%1_initData()\n"\
                                 "{\n"\
                                 "    QTest::addColumn<%2>(\"expected\");\n"\
                                 "    initScriptValues();\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::%1_makeData(const char* expr)\n"\
                                 "{\n"\
                                 "    static QHash<QString, %2> %1;\n"\
                                 "    if (%1.isEmpty()) {\n"\
                                 "%3"\
                                 "    }\n"\
                                 "    newRow(expr) << %1.value(expr);\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::%1_test(const char*, const QScriptValue& value)\n"\
                                 "{\n"\
                                 "    QFETCH(%2, expected);\n"\
                                 "%666"
                                 "    if (qIsInf(expected)) {\n"\
                                 "        QVERIFY(qIsInf(value.%1()));\n"\
                                 "        return;\n"\
                                 "    }\n"\
                                 "    QCOMPARE(value.%1(), expected);\n"\
                                 "}\n"\
                                 "\n"\
                                 "DEFINE_TEST_FUNCTION(%1)\n";
    QString result = templ;

    QList<QPair<QString, qsreal> >::const_iterator i = list.constBegin();
    QStringList set;
    for(; i != list.constEnd(); ++i) {
        QPair<QString, qsreal> t = *i;
        t.first = escape(t.first);
        set.append(QString("        "));
        set.append(name);
        set.append(".insert(\"");
        set.append(t.first);
        set.append(QString::fromAscii("\", "));
        set.append(prepareToInsert<qsreal>(t.second));
        set.append(QString::fromAscii(");\n"));
    }
    // toInteger shouldn't return NaN, so it would be nice to catch the case.
    QString hook;
    if (name == "toNumber") {
        hook =
        "    if (qIsNaN(expected)) {\n"\
        "        QVERIFY(qIsNaN(value.toNumber()));\n"\
        "        return;\n"\
        "    }\n";
    }
    return result.arg(name, typeName<qsreal>(), set.join(QString()), hook);
}

template<typename T>
static QString generateCastDef(const QList<QPair<QString, T> >& list)
{
    static const QString templ = "\n"\
                                 "void tst_QScriptValue::qscriptvalue_cast%2_initData()\n"\
                                 "{\n"\
                                 "    QTest::addColumn<%2>(\"expected\");\n"\
                                 "    initScriptValues();\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::qscriptvalue_cast%2_makeData(const char* expr)\n"\
                                 "{\n"\
                                 "    static QHash<QString, %2> value;\n"\
                                 "    if (value.isEmpty()) {\n"\
                                 "%3"\
                                 "    }\n"\
                                 "    newRow(expr) << value.value(expr);\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::qscriptvalue_cast%2_test(const char*, const QScriptValue& value)\n"\
                                 "{\n"\
                                 "    QFETCH(%2, expected);\n"\
                                 "    QCOMPARE(qscriptvalue_cast<%2>(value), expected);\n"\
                                 "}\n"\
                                 "\n"\
                                 "DEFINE_TEST_FUNCTION(qscriptvalue_cast%2)\n";
    QString result = templ;

    typename QList<QPair<QString, T> >::const_iterator i = list.constBegin();
    QStringList set;
    for(; i != list.constEnd(); ++i) {
        QPair<QString, T> t = *i;
        t.first = escape(t.first);
        set.append(QString("        "));
        set.append("value.insert(\"");
        set.append(t.first);
        set.append(QString::fromAscii("\", "));
        set.append(prepareToInsert<T>(t.second));
        set.append(QString::fromAscii(");\n"));
    }
    return result.arg(typeName<T>(), set.join(QString()));
}

template<>
QString generateCastDef<qsreal>(const QList<QPair<QString, qsreal> >& list)
{
    static const QString templ = "\n"\
                                 "void tst_QScriptValue::qscriptvalue_cast%2_initData()\n"\
                                 "{\n"\
                                 "    QTest::addColumn<%2>(\"expected\");\n"\
                                 "    initScriptValues();\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::qscriptvalue_cast%2_makeData(const char* expr)\n"\
                                 "{\n"\
                                 "    static QHash<QString, %2> value;\n"\
                                 "    if (value.isEmpty()) {\n"\
                                 "%3"\
                                 "    }\n"\
                                 "    newRow(expr) << value.value(expr);\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::qscriptvalue_cast%2_test(const char*, const QScriptValue& value)\n"\
                                 "{\n"\
                                 "    QFETCH(%2, expected);\n"\
                                 "    if (qIsNaN(expected)) {\n"
                                 "        QVERIFY(qIsNaN(qscriptvalue_cast<%2>(value)));\n"
                                 "        return;\n"
                                 "    }\n"\
                                 "    if (qIsInf(expected)) {\n"
                                 "        QVERIFY(qIsInf(qscriptvalue_cast<%2>(value)));\n"
                                 "        return;\n"
                                 "    }\n"
                                 "    QCOMPARE(qscriptvalue_cast<%2>(value), expected);\n"\
                                 "}\n"\
                                 "\n"\
                                 "DEFINE_TEST_FUNCTION(qscriptvalue_cast%2)\n";
    QString result = templ;

    QList<QPair<QString, qsreal> >::const_iterator i = list.constBegin();
    QStringList set;
    for(; i != list.constEnd(); ++i) {
        QPair<QString, qsreal> t = *i;
        t.first = escape(t.first);
        set.append(QString("        "));
        set.append("value.insert(\"");
        set.append(t.first);
        set.append(QString::fromAscii("\", "));
        set.append(prepareToInsert<qsreal>(t.second));
        set.append(QString::fromAscii(");\n"));
    }
    return result.arg(typeName<qsreal>(), set.join(QString()));
}

static QString generateCompareDef(const QString& comparisionType, const QList<QString> tags)
{
    static const QString templ = "\n"\
                                 "void tst_QScriptValue::%1_initData()\n"\
                                 "{\n"\
                                 "    QTest::addColumn<QScriptValue>(\"other\");\n"\
                                 "    QTest::addColumn<bool>(\"expected\");\n"\
                                 "    initScriptValues();\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::%1_makeData(const char *expr)\n"\
                                 "{\n"\
                                 "    static QSet<QString> equals;\n"\
                                 "    if (equals.isEmpty()) {\n"\
                                 "%2\n"\
                                 "    }\n"\
                                 "    QHash<QString, QScriptValue>::const_iterator it;\n"\
                                 "    for (it = m_values.constBegin(); it != m_values.constEnd(); ++it) {\n"\
                                 "        QString tag = QString::fromLatin1(\"%20 <=> %21\").arg(expr).arg(it.key());\n"\
                                 "        newRow(tag.toLatin1()) << it.value() << equals.contains(tag);\n"\
                                 "    }\n"\
                                 "}\n"\
                                 "\n"\
                                 "void tst_QScriptValue::%1_test(const char *, const QScriptValue& value)\n"\
                                 "{\n"\
                                 "    QFETCH(QScriptValue, other);\n"\
                                 "    QFETCH(bool, expected);\n"\
                                 "    QCOMPARE(value.%1(other), expected);\n"\
                                 "}\n"\
                                 "\n"\
                                 "DEFINE_TEST_FUNCTION(%1)\n";
    Q_ASSERT(comparisionType == "strictlyEquals"
             || comparisionType == "equals"
             || comparisionType == "lessThan"
             || comparisionType == "instanceOf");
    QString result = templ;

    QStringList set;
    foreach(const QString& tmp, tags) {
        set.append("        equals.insert(\"" + escape(tmp) + "\");");
    }
    return result.arg(comparisionType, set.join("\n"));
}

static QString generateInitDef(const QVector<QString>& allDataTags)
{
    static const QString templ = "/****************************************************************************\n"
                                 "**\n"
                                 "** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).\n"
                                 "** All rights reserved.\n"
                                 "** Contact: Nokia Corporation (qt-info@nokia.com)\n"
                                 "**\n"
                                 "** This file is part of the test suite of the Qt Toolkit.\n"
                                 "**\n"
                                 "** $QT_BEGIN_LICENSE:LGPL$\n"
                                 "** No Commercial Usage\n"
                                 "** This file contains pre-release code and may not be distributed.\n"
                                 "** You may use this file in accordance with the terms and conditions\n"
                                 "** contained in the Technology Preview License Agreement accompanying\n"
                                 "** this package.\n"
                                 "**\n"
                                 "** GNU Lesser General Public License Usage\n"
                                 "** Alternatively, this file may be used under the terms of the GNU Lesser\n"
                                 "** General Public License version 2.1 as published by the Free Software\n"
                                 "** Foundation and appearing in the file LICENSE.LGPL included in the\n"
                                 "** packaging of this file.  Please review the following information to\n"
                                 "** ensure the GNU Lesser General Public License version 2.1 requirements\n"
                                 "** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.\n"
                                 "**\n"
                                 "** In addition, as a special exception, Nokia gives you certain additional\n"
                                 "** rights.  These rights are described in the Nokia Qt LGPL Exception\n"
                                 "** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.\n"
                                 "**\n"
                                 "** If you have questions regarding the use of this file, please contact\n"
                                 "** Nokia at qt-info@nokia.com.\n"
                                 "**\n"
                                 "**\n"
                                 "**\n"
                                 "**\n"
                                 "**\n"
                                 "**\n"
                                 "**\n"
                                 "**\n"
                                 "** $QT_END_LICENSE$\n"
                                 "**\n"
                                 "****************************************************************************/\n"
                                 "\n"\
                                 "#include \"tst_qscriptvalue.h\"\n\n"\
                                 "#define DEFINE_TEST_VALUE(expr) m_values.insert(QString::fromLatin1(#expr), expr)\n"\
                                 "\n"\
                                 "void tst_QScriptValue::initScriptValues()\n"\
                                 "{\n"\
                                 "    m_values.clear();\n"\
                                 "    if (engine) \n"\
                                 "        delete engine;\n"\
                                 "    engine = new QScriptEngine;\n"\
                                 "%1\n}\n\n";
    QString result = templ;
    QStringList set;
    foreach(const QString tag, allDataTags) {
        set.append("    DEFINE_TEST_VALUE(" + tag + ");");
    }

    return result.arg(set.join("\n"));
}

static void squashTags(QString dataTag, const QVector<bool>& results, QList<QString>& tags, QVector<QString> dataTags)
{
    for(int i = 0; i < results.count(); ++i) {
        if (results.at(i))
            tags.append(dataTag + " <=> " + dataTags[i]);
    }
}


QString TestGenerator::generateTest()
{
    // All data tags keept in one place.
    QVector<QString> dataTags;

    // Data tags for values that return true in isXXX call
    QList<QString> isValidList;
    QList<QString> isBoolList;
    QList<QString> isBooleanList;
    QList<QString> isNumberList;
    QList<QString> isFunctionList;
    QList<QString> isNullList;
    QList<QString> isStringList;
    QList<QString> isUndefinedList;
    QList<QString> isVariantList;
    QList<QString> isQObjectList;
    QList<QString> isQMetaObjectList;
    QList<QString> isObjectList;
    QList<QString> isDateList;
    QList<QString> isRegExpList;
    QList<QString> isArrayList;
    QList<QString> isErrorList;

    // List of pairs data tag and value returned from toXXX call
    QList<QPair<QString, QString> > toStringList;
    QList<QPair<QString, qsreal> > toNumberList;
    QList<QPair<QString, bool> > toBoolList;
    QList<QPair<QString, bool> > toBooleanList;
    QList<QPair<QString, qsreal> > toIntegerList;
    QList<QPair<QString, qint32> > toInt32List;
    QList<QPair<QString, quint32> > toUInt32List;
    QList<QPair<QString, quint16> > toUInt16List;

    // List of complex tags returning true
    QList<QString> equalsList;
    QList<QString> strictlyEqualsList;
    QList<QString> lessThanList;
    QList<QString> instanceOfList;

    QList<QPair<QString, QString> > castStringList;
    QList<QPair<QString, qsreal> > castSRealList;
    QList<QPair<QString, bool> > castBoolList;
    QList<QPair<QString, qint32> > castInt32List;
    QList<QPair<QString, quint32> > castUInt32List;
    QList<QPair<QString, quint16> > castUInt16List;

    // Load.
    m_tempFile.seek(0);
    QDataStream in(&m_tempFile);
    in >> dataTags;
    Q_ASSERT(in.status() == in.Ok);

    while(!in.atEnd())
    {
        bool isValidRes;
        bool isBoolRes;
        bool isBooleanRes;
        bool isNumberRes;
        bool isFunctionRes;
        bool isNullRes;
        bool isStringRes;
        bool isUndefinedRes;
        bool isVariantRes;
        bool isQObjectRes;
        bool isQMetaObjectRes;
        bool isObjectRes;
        bool isDateRes;
        bool isRegExpRes;
        bool isArrayRes;
        bool isErrorRes;

        QString toStringRes;
        qsreal toNumberRes;
        bool toBoolRes;
        bool toBooleanRes;
        qsreal toIntegerRes;
        qint32 toInt32Res;
        quint32 toUInt32Res;
        quint16 toUInt16Res;
        //toVariantRes;
        //toDateTimeRes;

        QVector<bool> equalsRes;
        QVector<bool> strictlyEqualsRes;
        QVector<bool> lessThanRes;
        QVector<bool> instanceOfRes;

        QString castStringRes;
        qsreal castSRealRes;
        bool castBoolRes;
        qint32 castInt32Res;
        quint32 castUInt32Res;
        quint16 castUInt16Res;

        QString dataTag;
        in >> dataTag;
        in >> isValidRes;
        in >> isBoolRes;
        in >> isBooleanRes;
        in >> isNumberRes;
        in >> isFunctionRes;
        in >> isNullRes;
        in >> isStringRes;
        in >> isUndefinedRes;
        in >> isVariantRes;
        in >> isQObjectRes;
        in >> isQMetaObjectRes;
        in >> isObjectRes;
        in >> isDateRes;
        in >> isRegExpRes;
        in >> isArrayRes;
        in >> isErrorRes;

        if (isValidRes) isValidList.append(dataTag);
        if (isBoolRes) isBoolList.append(dataTag);
        if (isBooleanRes) isBooleanList.append(dataTag);
        if (isNumberRes) isNumberList.append(dataTag);
        if (isFunctionRes) isFunctionList.append(dataTag);
        if (isNullRes) isNullList.append(dataTag);
        if (isStringRes) isStringList.append(dataTag);
        if (isUndefinedRes) isUndefinedList.append(dataTag);
        if (isVariantRes) isVariantList.append(dataTag);
        if (isQObjectRes) isQObjectList.append(dataTag);
        if (isQMetaObjectRes) isQMetaObjectList.append(dataTag);
        if (isObjectRes) isObjectList.append(dataTag);
        if (isDateRes) isDateList.append(dataTag);
        if (isRegExpRes) isRegExpList.append(dataTag);
        if (isArrayRes) isArrayList.append(dataTag);
        if (isErrorRes) isErrorList.append(dataTag);

        in >> toStringRes;
        in >> toNumberRes;
        in >> toBoolRes;
        in >> toBooleanRes;
        in >> toIntegerRes;
        in >> toInt32Res;
        in >> toUInt32Res;
        in >> toUInt16Res;
        //in >> toVariantRes;
        //in >> toDateTimeRes;

        toStringList.append(QPair<QString, QString>(dataTag, toStringRes));
        toNumberList.append(QPair<QString, qsreal>(dataTag, toNumberRes));
        toBoolList.append(QPair<QString, bool>(dataTag, toBoolRes));
        toBooleanList.append(QPair<QString, bool>(dataTag, toBooleanRes));
        toIntegerList.append(QPair<QString, qsreal>(dataTag, toIntegerRes));
        toInt32List.append(QPair<QString, qint32>(dataTag, toInt32Res));
        toUInt32List.append(QPair<QString, quint32>(dataTag, toUInt32Res));
        toUInt16List.append(QPair<QString, quint16>(dataTag, toUInt16Res));

        in >> equalsRes;
        in >> strictlyEqualsRes;
        in >> lessThanRes;
        in >> instanceOfRes;

        squashTags(dataTag, equalsRes, equalsList, dataTags);
        squashTags(dataTag, strictlyEqualsRes, strictlyEqualsList, dataTags);
        squashTags(dataTag, lessThanRes, lessThanList, dataTags);
        squashTags(dataTag, instanceOfRes, instanceOfList, dataTags);

        in >> castStringRes;
        in >> castSRealRes;
        in >> castBoolRes;
        in >> castInt32Res;
        in >> castUInt32Res;
        in >> castUInt16Res;

        castStringList.append(QPair<QString, QString>(dataTag, castStringRes));
        castSRealList.append(QPair<QString, qsreal>(dataTag, castSRealRes));
        castBoolList.append(QPair<QString, bool>(dataTag, castBoolRes));
        castInt32List.append(QPair<QString, qint32>(dataTag, castInt32Res));
        castUInt32List.append(QPair<QString, quint32>(dataTag, castUInt32Res));
        castUInt16List.append(QPair<QString, quint16>(dataTag, castUInt16Res));

        Q_ASSERT(in.status() == in.Ok);
    }

    Q_ASSERT(in.atEnd());

    // Generate.
    QStringList result;
    result.append(generateInitDef(dataTags));
    result.append(generateIsXXXDef("isValid", isValidList));
    result.append(generateIsXXXDef("isBool", isBoolList));
    result.append(generateIsXXXDef("isBoolean", isBooleanList));
    result.append(generateIsXXXDef("isNumber", isNumberList));
    result.append(generateIsXXXDef("isFunction", isFunctionList));
    result.append(generateIsXXXDef("isNull", isNullList));
    result.append(generateIsXXXDef("isString", isStringList));
    result.append(generateIsXXXDef("isUndefined", isUndefinedList));
    result.append(generateIsXXXDef("isVariant", isVariantList));
    result.append(generateIsXXXDef("isQObject", isQObjectList));
    result.append(generateIsXXXDef("isQMetaObject", isQMetaObjectList));
    result.append(generateIsXXXDef("isObject", isObjectList));
    result.append(generateIsXXXDef("isDate", isDateList));
    result.append(generateIsXXXDef("isRegExp", isRegExpList));
    result.append(generateIsXXXDef("isArray", isArrayList));
    result.append(generateIsXXXDef("isError", isErrorList));

    result.append(generateToXXXDef<QString>("toString", toStringList));
    result.append(generateToXXXDef<qsreal>("toNumber", toNumberList));
    result.append(generateToXXXDef<bool>("toBool", toBoolList));
    result.append(generateToXXXDef<bool>("toBoolean", toBooleanList));
    result.append(generateToXXXDef<qsreal>("toInteger", toIntegerList));
    result.append(generateToXXXDef<qint32>("toInt32", toInt32List));
    result.append(generateToXXXDef<quint32>("toUInt32", toUInt32List));
    result.append(generateToXXXDef<quint16>("toUInt16", toUInt16List));

    result.append(generateCompareDef("equals", equalsList));
    result.append(generateCompareDef("strictlyEquals", strictlyEqualsList));
    result.append(generateCompareDef("lessThan", lessThanList));
    result.append(generateCompareDef("instanceOf", instanceOfList));

    result.append(generateCastDef(castStringList));
    result.append(generateCastDef(castSRealList));
    result.append(generateCastDef(castBoolList));
    result.append(generateCastDef(castInt32List));
    result.append(generateCastDef(castUInt32List));
    result.append(generateCastDef(castUInt16List));

    return result.join("\n");
}










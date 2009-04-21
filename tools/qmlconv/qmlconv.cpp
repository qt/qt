#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QStack>
#include <QtCore/QStringList>
#include <QtCore/QDebug>




class Reader
{
    QString outString;
    QTextStream out;
    QXmlStreamReader xml;
    int depth;
    bool supressIndent;

    QStringList knownListProperties;
    inline QString depthString() {if (supressIndent) { supressIndent = false; return QString(); }
        return QString(depth*4, QLatin1Char(' '));}

public:
    Reader(QIODevice *in)
        :xml(in) {

        knownListProperties << "states" << "transitions" << "children" << "resources"
                << "transform" << "notes";
        depth = 0;
        supressIndent = false;

        out.setString(&outString);

        loop();

        out.flush();
        QTextStream print(stdout);
        print << outString;
    }

    void comment()
    {
        if (xml.isComment()) {
            out << depthString() << "// "
                << xml.text().toString().trimmed().replace(QRegExp("\n\\s*"),"\n"+depthString()+"// ")
                << endl;
        }
    }

    void emptyLoop() {
        while (!xml.atEnd()) {
            xml.readNext();
            comment();
            if (xml.tokenType() == QXmlStreamReader::EndElement)
                return;
        }
    }

    void loop()
    {
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::EndElement)
                return;
            else if (xml.tokenType() == QXmlStreamReader::StartElement)
                startElement();
            else if (xml.tokenType() == QXmlStreamReader::ProcessingInstruction) {
                if (xml.processingInstructionTarget() == QLatin1String("qtfx")) {
                    QString data = xml.processingInstructionData().toString().trimmed();
                    if (data.startsWith(QLatin1String("namespacepath:="))) {
                        outString.prepend( QLatin1String("import \"") + data.mid(data.indexOf(QLatin1Char('='))+1) + QLatin1String("\"\n"));
                    }
                }
            }
            comment();
        }
    }

    void startElement() {

        if (!propertyChangeSet.isEmpty()
            && xml.name() != "SetProperties"
            && xml.name() != "SetProperty") {
            clearPropertyChangeSet();
        }

        if (xml.name() == "properties")
            startDeclareProperties();
        else if (xml.name() == "signals")
            startDeclareSignals();
        else if (xml.name() == "states")
            loop(); // ignore
        else if (xml.name() == "transitions")
            loop(); // ignore
        else if (knownListProperties.contains(xml.name().toString()))
            startList();
        else if (xml.name() == "SetProperties")
            startSetProperties();
        else if (xml.name() == "SetProperty")
            startSetProperty();
        else if (xml.name() == "ParentChange")
            startParentChange();
        else if (xml.name() == "Connection")
            startConnection();
        else if (xml.name() == "Script")
            startScript();
        else if (xml.name().at(0).isLower() && xml.attributes().isEmpty())
            startObjectProperty();
        else
            startItem();
    }

    static void possiblyRemoveBraces(QString *s) {
        if (s->startsWith('{') && s->endsWith('}'))
            *s = s->mid(1, s->length() - 2);
    }

    static bool isNumber(const QString &s) {
        bool ok = true;
        s.toFloat(&ok);
        return ok;
    }

    static bool isSignalHandler(const QString &s) {
        return s.size() > 3
                && s.startsWith("on")
                && s.at(2).isUpper();
    }

    static bool isEnum(const QString &property, const QString &value)  {
        return !value.contains(' ') && (property == "vAlign" || property == "hAlign"
                                        || property == "style");
    }

    static bool isIdentifier(const QString &s) {
        if (s.isEmpty())
            return false;
        if (!s.at(1).isLetter())
            return false;
        for (int i = 1; i < s.size(); ++i) {
            QChar c = s.at(i);
            if (c.isLetterOrNumber()
                || c == QLatin1Char('_')
                || c == QLatin1Char('-'))
                continue;
            return false;
        }
        return true;
    }


    void setProperty(const QString &property, const QString &value, bool newline = true) {
        QString v = value.trimmed();
        if (v.startsWith('{')) {
            possiblyRemoveBraces(&v);
        } else if (v == "true"
                   || v == "false"
                   || isNumber(v)
                   || property == "id"
                   || isEnum(property, value)
                   ) {
            ;
        } else if (isSignalHandler(property)) {
            // if not a function name, create an anonymous function
            if (!isIdentifier(v)) {
                v.prepend("{ ");
                v.append(" }");
            }
        } else

            //            if (property == "text" || property == "name" || value.contains(' ')
            //            || value.contains("/") || value.startsWith('#')
            //            || property == "filename" || property == "source" || property == "src"
            //                    || property == "title" || property == "movieTitle" || property == "movieDescription"
            //                    || property == "properties" || property == "fromState" || property == "toState"
            //                    )
        {
            v.prepend('\"');
            v.append('\"');
        }

//        QByteArray semiColon = ";";
//        if (v.endsWith(QLatin1Char('}')) || v.endsWith(QLatin1Char(';')))
//            semiColon.clear();

        if (!newline)
            out << property << ": " << v /* << semiColon.constData() */;
        else
            out << depthString() << property << ": " << v /* << semiColon.constData() */  << endl;
    }


    typedef QPair<QString,QString> StringPair;
    QList<StringPair> propertyChangeSet;
    void startItem(bool inList = false) {

        QString name = xml.name().toString();

        out << depthString() << name << " {" << endl;
        ++depth;

        foreach (QXmlStreamAttribute attribute, xml.attributes()) {
            setProperty(attribute.name().toString(), attribute.value().toString());
        }

        loop();

        if (name == "State")
            clearPropertyChangeSet();

        --depth;
        out << depthString() << "}";
        if (!inList)
            out << endl;
    }

    void clearPropertyChangeSet() {
        if (propertyChangeSet.isEmpty())
            return;

        out << depthString() << "PropertyChangeSet" << " {" << endl;
        ++depth;
        foreach(StringPair pair, propertyChangeSet)
            setProperty(pair.first, pair.second);
        --depth;
        out << depthString() << "}" << endl;
        propertyChangeSet.clear();
    }

    void startObjectProperty() {

        QString name = xml.name().toString();
        bool hasElements = false;
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::EndElement)
                break;
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                hasElements = true;
                out << depthString() << name << ": ";
                supressIndent = true;
                startElement();
            } else if (!hasElements && xml.tokenType() == QXmlStreamReader::Characters) {
                if (!xml.text().toString().trimmed().isEmpty()) {
                    setProperty(name, xml.text().toString());
                }
            }
            comment();
        }
    }

    void startDeclareProperty() {
        out << depthString() << "public property ";
        QString name = xml.attributes().value("name").toString();

        if (xml.attributes().hasAttribute("value"))
            setProperty(name, xml.attributes().value("value").toString(), false);
        else out << name;

        QMap<QString, QString> attributes;
        foreach (QXmlStreamAttribute attribute, xml.attributes()) {
            if (attribute.name() == "name" || attribute.name() == "value")
                continue;
            attributes.insert(attribute.name().toString(), attribute.value().toString());
        }
        if (attributes.isEmpty()) {
            out << endl;
        } else {
            out << " {" << endl;
            ++depth;
            foreach (QString key, attributes.keys())
                setProperty(key, attributes.value(key));
            --depth;
            out << depthString() << "}" << endl;
        }
        emptyLoop();
    }

    void startDeclareProperties() {
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::EndElement)
                return;
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                if (xml.name() == "Property")
                    startDeclareProperty();
            }
            comment();
        }
    }

    void startDeclareSignal() {
        out << depthString() << "public signal " << xml.attributes().value("name").toString() << endl;
        emptyLoop();
    }

    void startDeclareSignals() {
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::EndElement)
                return;
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                if (xml.name() == "Signal")
                    startDeclareSignal();
            }
            comment();
        }
    }


    void startSetProperties() {
        QString target = xml.attributes().value("target").toString();
        possiblyRemoveBraces(&target);
        foreach (QXmlStreamAttribute attribute, xml.attributes()) {
            if (attribute.name() == "target")
                continue;
            propertyChangeSet += StringPair(target + "." + attribute.name().toString(), attribute.value().toString());
        }
        emptyLoop();
    }

    void startSetProperty() {
        QString target = xml.attributes().value("target").toString();
        possiblyRemoveBraces(&target);
        propertyChangeSet += StringPair(target + "." + xml.attributes().value("property").toString(),
                                        xml.attributes().value("value").toString());

        emptyLoop();
    }

    void startParentChange() {
        QString target = xml.attributes().value("target").toString();
        possiblyRemoveBraces(&target);

        out << depthString() << "ParentChangeSet" << " {" << endl;
        ++depth;
        setProperty(target + ".parent", xml.attributes().value("parent").toString());
        --depth;
        out << depthString() << "}" << endl;

//        propertyChangeSet += StringPair(target + ".moveToParent", xml.attributes().value("parent").toString());

        emptyLoop();
    }

    void startConnection() {
        QString sender = xml.attributes().value("sender").toString();
        possiblyRemoveBraces(&sender);
        out << depthString() << "Connection {" << endl;
        ++depth;
        out << depthString() << "signal: " << sender + "." + xml.attributes().value("signal").toString() << endl;
        out << depthString() << "onSignal: { " << xml.attributes().value("script").toString() << " }" << endl;
        --depth;
        out << depthString() << "}" << endl;
        emptyLoop();
    }

    void startScript() {
        if (xml.attributes().hasAttribute(QLatin1String("src"))) {
            /*
            QString import;
            QTextStream ts(&import);
            ts << "import \"";
            ts << xml.attributes().value(QLatin1String("src")).toString();
            ts << "\"" << endl;
            ts.flush();
            outString.prepend(import);
            */
        }
        QString text = xml.readElementText();
        if (!text.trimmed().isEmpty()) {
            out << text << endl;
        }
        if (xml.tokenType() != QXmlStreamReader::EndElement)
            emptyLoop();
    }

    void startList()
    {
        out << depthString() << xml.name().toString() << ": [" << endl;
        ++depth;
        bool needComma = false;

        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::EndElement)
                break;
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                if (needComma)
                    out << "," << endl;
                startItem(true);
                needComma = true;
            }
            comment();
        }

        out << endl;
        --depth;
        out << depthString() << "]" << endl;
    }

};



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 2) {
        qWarning() << "Usage: qmlconf filename";
        exit(1);
    }

    QFile file(argv[1]);
    if (file.open(QIODevice::ReadOnly)) {
        Reader r(&file);
    }


    file.close();
    return 0;
}

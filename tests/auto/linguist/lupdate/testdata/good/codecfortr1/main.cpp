#include <QtCore>

class FooBar : QObject
{
    Q_OBJECT

public:
    void doFoo()
    {
        tr("random ascii only");
        tr("this contains an umlaut ü &uuml;");
        trUtf8("random ascii only in utf8");
        trUtf8("umlaut \xfc &uuml; in utf8");
    }
};

int main(int argc, char **argv)
{
    return 0;
}

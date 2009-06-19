//! [0]
class MyProxyStyle : public QProxyStyle
{
public:

    int styleHint(StyleHint hint, const QStyleOption *option = 0, 
                  const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const
    {
        if (hint == QStyle::SH_UnderlineShortcut)
            return 1;
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

//! [0]

//! [1]
#include "textedit.h"
#include <QApplication>
#include <QProxyStyle>
  
class MyProxyStyle : public QProxyStyle
{
  public:
    int styleHint(StyleHint hint, const QStyleOption *option = 0,
                  const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const
    {
        if (hint == QStyle::SH_UnderlineShortcut)
            return 0;
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};
    
int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(textedit);
    
    QApplication a(argc, argv);
    a.setStyle(new MyProxyStyle);
    TextEdit mw;
    mw.resize(700, 800);
    mw.show(); 
    //...
}
//! [1]

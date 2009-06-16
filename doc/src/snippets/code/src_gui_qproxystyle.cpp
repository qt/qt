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

#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include <QTime>

class WebView : public QWebView
{
    Q_OBJECT
public:
    WebView(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void newPageLoading();
    void pageLoaded(bool ok);

private:
    QTime loadingTime;
    bool inLoading;
};

#endif // WEBVIEW_H

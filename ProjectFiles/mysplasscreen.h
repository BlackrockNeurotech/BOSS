#ifndef MYSPLASHSCREEN_H
#define MYSPLASHSCREEN_H

#include <QSplashScreen>


class MySplashScreen : public QSplashScreen
{
    Q_OBJECT
    
public:
    MySplashScreen() : QSplashScreen() {}
    ~MySplashScreen(){}


private:
    void closeEvent(QCloseEvent * event);
    void hideEvent(QHideEvent * event);

signals:
    void closed();

};



#endif // MYSPLASHSCREEN_H
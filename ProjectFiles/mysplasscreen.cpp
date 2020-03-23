#include "../ProjectFiles/mysplasscreen.h"
#include <QCloseEvent>

void MySplashScreen::closeEvent(QCloseEvent *event)
{
    if (isHidden() == false)
    {
        event->accept();
        setVisible(false);
                
    }
    else
        event->ignore();

}

void MySplashScreen::hideEvent(QHideEvent * event)
{   
    Q_UNUSED(event);
    emit closed();
}
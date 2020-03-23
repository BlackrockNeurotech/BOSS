#ifndef OPTIONSSLIDER_H
#define OPTIONSSLIDER_H

#include <QPropertyAnimation>
#include <QWidget>
#include <QToolBar>
#include <qmainwindow.h>
#include "../widgets/optionsdetectframe.h"
#include "../widgets/optionssortframe.h"

#include "../model/channelconfigmodel.h"

namespace Ui {
class OptionsSlider;
}

class OptionsSlider : public QWidget
{
    Q_OBJECT

public:
    explicit OptionsSlider(QWidget *parent = 0);
    ~OptionsSlider();

    void initGeometry();    
    void hideAll();
    void changeState(WizardPage state);  // can't use the signal here, because this has to happen last
    void setToolbar(QToolBar *left) 
    { 
        m_left_toolbar = left;
    }

	OptionsDetectFrame  *m_detectOptions;
	OptionsSortFrame    *m_sortOptions;

protected:
    virtual void showEvent(QShowEvent *event);

private:
    Ui::OptionsSlider *ui;

    QPropertyAnimation *m_animation;
    bool m_bSliderClosed;
    bool m_bSliderDocked;
    QToolBar * m_left_toolbar;

    int m_width;

public slots:
    void slideRight();
    void slideLeft();

};

#endif // OPTIONSSLIDER_H

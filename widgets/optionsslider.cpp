#include <QBoxLayout>

#include "optionsslider.h"
#include "ui_optionsslider.h"


OptionsSlider::OptionsSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionsSlider),
    m_bSliderClosed(true),
    m_bSliderDocked(false)
{
    ui->setupUi(this);
	QVBoxLayout *layout = new QVBoxLayout(this);
	
	m_sortOptions = new OptionsSortFrame();
	m_detectOptions = new OptionsDetectFrame();

	layout->addWidget(m_detectOptions);
	layout->addWidget(m_sortOptions);
    layout->setContentsMargins(0,0,0,0);

	this->setLayout(layout);

}

OptionsSlider::~OptionsSlider() 
{
    delete ui;
	delete m_detectOptions;
	delete m_sortOptions;
//    delete m_animation;
}


void OptionsSlider::initGeometry()
{
       
    //VerticalLabel * label = findChild<VerticalLabel *>("optionsLabel");
    //int x0, y0, dx, dy;
    //x0 = label->geometry().x();
    //y0 = label->geometry().y();
    //dx = toolbarWidth;
    //dy = label->geometry().height();
    //label->setGeometry(x0, y0, dx, dy);

    //setGeometry(0, 0, width(), height());
    m_bSliderClosed = true;
    m_width = width();
}

void OptionsSlider::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}

void OptionsSlider::slideRight()
{
    //if (m_animation->state() == QAbstractAnimation::Stopped && m_bSliderClosed && (x() != 0))
    //{
    //    int optionswidth = this->width();
    //    qreal y = m_top_toolbar->y() + m_top_toolbar->height();
    //    QRect startRect(-optionswidth, y, m_width, height());
    //    QRect endRect(0, y, m_width, height());
    //    m_animation->setStartValue(startRect);
    //    m_animation->setEndValue(endRect);
    //    m_animation->start();
    //    m_bSliderClosed = false;
    //}
}


void OptionsSlider::slideLeft()
{
    //if (m_animation->state() == QAbstractAnimation::Stopped && !m_bSliderDocked)
    //{
    //    int optionswidth = this->width();
    //    qreal y = m_top_toolbar->y() + m_top_toolbar->height();
    //    QRect startRect(x(), y, m_width, height());
    //    QRect endRect(-optionswidth, y, m_width, height());
    //    m_animation->setStartValue(startRect);
    //    m_animation->setEndValue(endRect);
    //    m_animation->start();
    //    m_bSliderClosed = true;
    //}
    //else
    //{
    //    m_animation->stop();
    //    while (m_animation->state() != QAbstractAnimation::Stopped)
    //    {
    //        // loop here and wait till stopped
    //    }
    //    slideLeft();
    //}
}

void OptionsSlider::changeState(WizardPage state)
{
    hideAll();

    switch (state)
    {
	case WIZARDPAGE_DETECT:
        //        this->setGeometry(0, y, m_groupBoxFilterOptions->width(), m_groupBoxFilterOptions->height());
		m_detectOptions->show();
        break;
	case WIZARDPAGE_SORT:
        //        this->setGeometry(0, y, m_sortOptions->width(), m_sortOptions->height());
		m_sortOptions->show();
//		setMinimumHeight(m_sortOptions->height());
        //this->adjustSize();
        break;
    case WIZARDPAGE_EVALUATE:
        break;
    case WIZARDPAGE_END:
        break;
    }

    this->show();
    this->activateWindow();
    this->raise();
    
}

void OptionsSlider::hideAll()
{
	m_detectOptions->hide();
	m_sortOptions->hide();
    this->hide();
}
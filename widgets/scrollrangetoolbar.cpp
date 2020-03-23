#include "ScrollRangeToolBar.h"
#include "../model/channelconfigmodel.h"
#include "../backend/file/fileinterface.h"
#include "../model/configmodel.h"
#include "../backend/dataprocessing/common/Utils.h"

ScrollRangeToolBar::ScrollRangeToolBar(QWidget *parent) :
    QToolBar(parent)
{
    this->setStyleSheet("QToolBar {border-bottom: none; border-top: none;}");

    QWidget *w = new QWidget();
    m_layout = new QHBoxLayout();
    m_layout->setSpacing(1);
    m_layout->setMargin(0);
    m_layout->setSizeConstraint(QLayout::SetNoConstraint);

    // create the start and end time labels
    m_labelMin = new QLabel();
    m_labelMax = new QLabel();
    m_labelMax->setStyleSheet("color: #009FE3;");
    m_labelMin->setStyleSheet("color: #009FE3;");

    // make the scroll bar the height of the font
    QFont myFont;
    QFontMetrics fm(myFont);
    QRectF f = fm.boundingRect("123");
    m_nHeight = f.height() * 4;
    m_nGeom = f.height() + 2;

    // create the arrow buttons and set the dimensions
    m_arrowLeft = new ArrowButton(this, ArrowButton::left, m_nGeom);
    m_arrowRight = new ArrowButton(this, ArrowButton::right, m_nGeom);
    m_arrowLeft->setMaximumSize(m_nGeom, m_nGeom);
    m_arrowRight->setMaximumSize(m_nGeom, m_nGeom);
    m_arrowLeft->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_arrowRight->setAttribute(Qt::WA_LayoutUsesWidgetRect);

    m_scrollbar = new ScrollRangeGraphicsView(this);

    this->setMaximumHeight(m_nHeight);

    // add the widgets to the layout
    m_layout->addWidget(m_labelMin, 0, Qt::AlignVCenter|Qt::AlignLeft);
    m_layout->addSpacing(4);
    m_layout->addWidget(m_arrowLeft, 0, Qt::AlignVCenter|Qt::AlignLeft);
    //m_layout->addSpacing(4);
    m_layout->addWidget(m_scrollbar, 2, 0);
    //m_layout->addSpacing(4);
    m_layout->addWidget(m_arrowRight, 0, Qt::AlignVCenter|Qt::AlignRight);
    m_layout->addSpacing(4);
    m_layout->addWidget(m_labelMax, 0, Qt::AlignVCenter|Qt::AlignRight);

    w->setLayout(m_layout);
    this->addWidget(w);

    QObject::connect(m_arrowLeft, SIGNAL(clicked(bool)), this, SLOT(slotDecrement(bool)));
    QObject::connect(m_arrowRight, SIGNAL(clicked(bool)), this, SLOT(slotIncrement(bool)));

    // channel model signals
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(sliderChanged(int, SliderType)), this, SLOT(slotSliderChanged(int, SliderType)));
    connect(ccm, SIGNAL(timeseriesReady(quint32, RawDataAccum)), this, SLOT(slotDataAvailable(quint32, RawDataAccum)));
    connect(ccm, SIGNAL(featuresChanged(int)), this, SLOT(slotFeaturesChanged(int)));

    this->setStyleSheet("background-color: black;");

    this->setContentsMargins(0, 0, 0, 0);

    setRange(0);
}

ScrollRangeToolBar::~ScrollRangeToolBar()
{
}

void ScrollRangeToolBar::slotIncrement(bool value)
{
	Q_UNUSED(value);
    m_scrollbar->increment();
}

void ScrollRangeToolBar::slotDecrement(bool value)
{
	Q_UNUSED(value);
	m_scrollbar->decrement();
}

void ScrollRangeToolBar::slotSliderChanged(int nchannel, SliderType type)
{
    if (type == SLIDERTYPE_TIME)
    {
        showhide(nchannel);
        setRange(nchannel);
    }
}

void ScrollRangeToolBar::slotDataAvailable(quint32 nchannel, RawDataAccum accum)
{
    showhide(nchannel);
    m_scrollbar->updateSeparators();
}

void ScrollRangeToolBar::slotFeaturesChanged(int nchannel)
{
    showhide(nchannel);
}

void ScrollRangeToolBar::showhide(int nchannel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    WizardPage page = (WizardPage)ccm->data(ccm->index(nchannel, CC_PAGE), Qt::UserRole).toInt();

	ConfigModel *cm = ConfigModel::instance();
	quint64 nMaxSamples = cm->item(0, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES)->data(Qt::DisplayRole).toULongLong();

    if (page <= WIZARDPAGE_DETECT)
    {
        int nSamples = ccm->data(ccm->index(nchannel, CC_LAST_TIME), Qt::UserRole).toInt();

		if (nSamples > nMaxSamples)
            this->show();
        else
            this->hide();
    }
    else
    {
        double nWaveforms = ccm->data(ccm->index(nchannel, CC_NWAVEFORMS), Qt::UserRole).toInt();
        ConfigModel *cm = ConfigModel::instance();
        quint64 nMaxWaveforms = cm->item(0, OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS)->data(Qt::DisplayRole).toULongLong();
        if (nWaveforms > nMaxWaveforms)
            this->show();
        else
            this->hide();
    }
}

void ScrollRangeToolBar::setRange(int nchannel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();

    // get the file start and end times
    double sampleRate = ccm->data(ccm->index(nchannel, CC_SAMPLERATE), Qt::UserRole).toInt();
    double startTime = ccm->data(ccm->index(nchannel, CC_FIRST_TIME), Qt::UserRole).toInt();
    double endTime = ccm->data(ccm->index(nchannel, CC_LAST_TIME), Qt::UserRole).toInt();
    double factor = double(1000.0) / sampleRate;
    m_nStartMs = startTime * factor;
    m_nEndMs = endTime * factor;

    // create the strings for the start and end time labels
    QString sMin = msToString(m_nStartMs);
    QString sMax = msToString(m_nEndMs);

    m_labelMin->setText(sMin);
    m_labelMax->setText(sMax);

    m_scrollbar->setRange(startTime, endTime, m_nGeom);
}


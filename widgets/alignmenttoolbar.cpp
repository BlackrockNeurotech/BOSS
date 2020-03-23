#include "alignmenttoolbar.h"
#include "../model/channelconfigmodel.h"
#include "../model/configmodel.h"
#include "../backend/dataprocessing/spike/spikeproxymodel.h"
#include "../model/configmodel.h"

AlignmentToolBar::AlignmentToolBar(QWidget *parent) :
QToolBar(parent)
{
    this->setStyleSheet("QToolBar {border-bottom: none; border-top: none;}");

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    int nWaveformLength = ccm->data(ccm->index(channel, CC_NBR_SAMPLES_IN_WAVEFORM), Qt::UserRole).toInt();

    m_comboAlignTo = new ComboWidgetAction(this, CC_WAVEFORM_ALIGNMENT, ChannelData::AlignModeNames, QString("Align to"), "");
    m_spinMaxShift = new SpinWidgetAction(this, CC_MAX_SHIFT, 1, nWaveformLength, QString("Maximum Samples to Shift Waveforms"));

    m_widgetUnits = new UnitSelectionAction(this, true, CC_UNITS_ALIGN);
    m_widgetUnits->setToolTip(QString("Select Units to Align"));
    m_alignButton = new ToolbarWidgetAction(this, "Align", ACTIONWIDGETTYPE_BLACKROCKBUTTON);

    QStringList images = QStringList() << ":/new/Bitmaps/Bitmaps/Edit.png" << ":/new/Bitmaps/Bitmaps/Lines.png";
    QStringList tooltips = QStringList() << "Edit" << "Edit with Lines";
    m_actionMode = new ModeWidgetAction(this, CC_EDIT_MODE, ChannelData::EditModeNames, tooltips, images);

    this->addAction(m_alignButton);
    this->addSeparator();
    this->addAction(m_comboAlignTo);
    this->addSeparator();
    this->addAction(m_spinMaxShift);
    this->addSeparator();
    this->addAction(m_widgetUnits);
    this->addSeparator();
    this->addAction(m_actionMode);

    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(ccm, SIGNAL(featuresChanged(int)), this, SLOT(slotFeaturesChanged(int)));

    connect(m_alignButton, SIGNAL(pressed()), this, SLOT(slotAlign()));

    // get the color configuration from the model
    ConfigModel *cm = ConfigModel::instance();
    m_unitColors = cm->getColors();
    this->setContentsMargins(0, 0, 0, 0);
}

AlignmentToolBar::~AlignmentToolBar()
{
}

void AlignmentToolBar::slotFeaturesChanged(int channel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int nWaveformLength = ccm->data(ccm->index(channel, CC_NBR_SAMPLES_IN_WAVEFORM), Qt::UserRole).toInt();

    m_spinMaxShift->setRange(1, nWaveformLength);
}

void AlignmentToolBar::slotChannelChanged(int channel)
{
}
void AlignmentToolBar::slotAlign()
{
    SpikeProxyModel::instance()->slotAlignWaveforms(ChannelConfigModel::instance()->CurrentChannel());
}


QVector<int> AlignmentToolBar::getAlignUnits()
{
    QVector<int> alignUnits;

    // get the visible unit list from the channel model
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();

    QStringList units = ccm->data(ccm->index(channel, CC_UNITS_ALIGN), Qt::UserRole).toStringList();

    foreach (QString c, units)
    {
        alignUnits.push_back(c.toInt());
    }
    return alignUnits;
}


#include "optionsdetectframe.h"
#include "ui_optionsdetectframe.h"
#include "../model/configmodel.h"

OptionsDetectFrame::OptionsDetectFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OptionsDetectFrame)
{
    ui->setupUi(this); 

    // Change uV to \muV
    ui->labelMicroVolts->setText(QString(QChar(0x03BC)) + "V");
    ui->labeluVSquared->setText(("\x28" + QString(QChar(0x03BC)) + "V" + "\x29" + QString(QChar(0x00B2))));

	ChannelConfigModel *ccm = ChannelConfigModel::instance();

	// create the CheckRadioGroup to wrap the filters radio buttons
    m_filters = new CheckRadioGroup(this, CC_HIGHPASS_FILTER);
	m_filters->addRadioButton(ui->radioButtonButterworth, ChannelData::HighPassFilterNames.at(HIGHPASS_BUTTERWORTH));
	m_filters->addRadioButton(ui->radioButtonElliptic, ChannelData::HighPassFilterNames.at(HIGHPASS_CHEBYSHEV1));
	m_filters->addRadioButton(ui->radioButtonBessel, ChannelData::HighPassFilterNames.at(HIGHPASS_BESSEL));
	m_filters->setCheckBox(ui->checkBoxUseHighPassFilter, ChannelData::HighPassFilterNames.at(HIGHPASS_BUTTERWORTH), ChannelData::HighPassFilterNames.at(HIGHPASS_FILTER_NONE));

	// create the CheckRadioGroup to wrap the additional processing radio buttons
    m_signalEnergy = new CheckRadioGroup(this, CC_SIGNAL_ENERGY);
	m_signalEnergy->addRadioButton(ui->radioButtonSignalEnergy, ChannelData::SignalEnergyNames.at(ADDITIONAL_SIGNAL_ENERGY));
	m_signalEnergy->addRadioButton(ui->radioButtonNonlinearSignalenergy, ChannelData::SignalEnergyNames.at(ADDITIONAL_NONLINEAR_SIGNAL_ENERGY));
	m_signalEnergy->addRadioButton(ui->radioButtonSignedSignalEnergy, ChannelData::SignalEnergyNames.at(ADDITIONAL_SIGNED_SIGNAL_ENERGY));
	m_signalEnergy->setCheckBox(ui->checkBoxUseSignalEnergy, ChannelData::SignalEnergyNames.at(ADDITIONAL_SIGNAL_ENERGY), ChannelData::SignalEnergyNames.at(ADDITIONAL_NONE));

	// create the RadioGroup to wrap the threshold radio buttons
    m_threshold = new RadioGroup(this, CC_THRESHOLD);
	m_threshold->addRadioButton(ui->radioButtonSingle, ChannelData::ThresholdTypeNames.at(THRESHOLDTYPE_SINGLE));
	m_threshold->addRadioButton(ui->radioButtonDual, ChannelData::ThresholdTypeNames.at(THRESHOLDTYPE_DUAL));
	m_threshold->addRadioButton(ui->radioButtonNoiseRMS, ChannelData::ThresholdTypeNames.at(THRESHOLDTYPE_NOISE_RMS));
	m_threshold->addRadioButton(ui->radioButtonSE, ChannelData::ThresholdTypeNames.at(THRESHOLDTYPE_ENERGY));

	// create the SpinBoxGroup to wrap Threshold 1 spinboxes
    m_t1 = new SpinBoxGroup(this, CC_THRESHOLD1);
	m_t1->addSpinBox(ui->spinBoxMicroVolts1, SpinBoxFunction::fxnMultiply, 0.25);
	m_t1->addSpinBox(ui->spinBoxADCCounts1, SpinBoxFunction::fxnBase);
	m_t1->addSpinBox(ui->doubleSpinBoxADCRange1, SpinBoxFunction::fxnPercent, 8191.0);
    ui->doubleSpinBoxADCRange1->setSingleStep(0.02);

	// create the SpinBoxGroup to wrap Threshold 1 spinboxes
    m_t2 = new SpinBoxGroup(this, CC_THRESHOLD2);
	m_t2->addSpinBox(ui->spinBoxMicroVolts2, SpinBoxFunction::fxnMultiply, 0.25);
	m_t2->addSpinBox(ui->spinBoxADCCounts2, SpinBoxFunction::fxnBase);
	m_t2->addSpinBox(ui->doubleSpinBoxADCRange2, SpinBoxFunction::fxnPercent, 8191.0);
    ui->doubleSpinBoxADCRange2->setSingleStep(0.02);

	// create the SpinBoxGroup to wrap Waveform length spinboxes
    m_waveformLength = new SpinBoxGroup(this, CC_WF_LENGTH);
	m_waveformLength->addSpinBox(ui->spinBoxLength, SpinBoxFunction::fxnBase);
	m_waveformLength->addSpinBox(ui->spinBoxLengthSamples, SpinBoxFunction::fxnColumnMultiply, 0.000001, CC_SAMPLERATE);
    m_waveformLength->updateOnEditingFinished(true);

	// create the SpinBoxGroup to wrap Waveform prethreshold spinboxes
    m_waveformPrethreshold = new SpinBoxGroup(this, CC_WF_PRETHRESHOLD);
	m_waveformPrethreshold->addSpinBox(ui->spinBoxPreThreshold, SpinBoxFunction::fxnBase);
	m_waveformPrethreshold->addSpinBox(ui->spinBoxPreThresholdSamples, SpinBoxFunction::fxnColumnMultiply, 0.000001, CC_SAMPLERATE);
    m_waveformPrethreshold->updateOnEditingFinished(true);

    // create the SpinBoxGroup to wrap deadtime spinboxes
    m_waveformPeriod = new SpinBoxGroup(this, CC_DEAD_TIME);
    m_waveformPeriod->addSpinBox(ui->spinBoxDeadTime, SpinBoxFunction::fxnBase);
    m_waveformPeriod->addSpinBox(ui->spinBoxDeadTimeSamples, SpinBoxFunction::fxnColumnMultiply, 0.000001, CC_SAMPLERATE);
    m_waveformPeriod->updateOnEditingFinished(true);

    // map the SpinBoxes to the model
    ui->spinBoxCutOffFreq->setChannelColumn(CC_CUTOFF_FREQ);
    ui->spinBoxFilterOrder->setChannelColumn(CC_FILTER_ORDER);
    ui->spinBoxSignalEnergy->setChannelColumn(CC_SE_WIDTH);
    ui->spinBoxMultiplier->setChannelColumn(CC_RMS_MULTIPLIER);
    ui->spinBoxSignalEnergyThreshold->setChannelColumn(CC_ENERGY_THRESHOLD);
    ui->spinBoxCutOffFreq->setMinimum(1);

    // map the Symetrical CheckBox to the model
    ui->checkBoxSymetrical->setChannelColumn(CC_SYMETRICAL_THRESHOLD);

    connect(ui->pushButtonDetect, SIGNAL(clicked()), this, SLOT(slotDetect()));
    ui->pushButtonDetect->setCheckable(true);

    connect(ui->pushButtonCancelDetect, SIGNAL(clicked()), this, SLOT(slotCancelDetect()));

    ui->checkBoxFilters->setChecked(false);
    on_checkBoxFilters_toggled(false);
    ui->checkBoxThreshold->setChecked(false);
    on_checkBoxThreshold_toggled(false);
    ui->checkBoxWaveform->setChecked(false);
    on_checkBoxWaveform_toggled(false);

    ui->pushButtonDetect->setEnabled(true);
    ui->pushButtonDetect->setChecked(true);
    ui->pushButtonCancelDetect->setEnabled(false);

	connect(ChannelConfigModel::instance(), SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
	connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
	connect(ccm, SIGNAL(thresholdTypeChanged(int)), this, SLOT(slotThresholdTypeChanged(int)));
    connect(ccm, SIGNAL(channelStateChanged(int, ChannelState)), this, SLOT(slotChannelStateChanged(int, ChannelState)));
    connect(ccm, SIGNAL(sliderChanged(int, SliderType)), this, SLOT(slotSliderChanged(int, SliderType)));
	connect(ccm, SIGNAL(filteringChanged(int)), this, SLOT(slotFilteringChanged(int)));
    connect(ccm, SIGNAL(signalEnergyTypeChanged(int)), this, SLOT(slotSignalEnergyTypeChanged(int)));
    connect(ccm, SIGNAL(channelSelectionChanged()), this, SLOT(slotChannelSelectionChanged()));

    connect(m_filters, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    connect(m_signalEnergy, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    connect(m_threshold, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    connect(m_t1, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    connect(m_t2, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    connect(m_waveformLength, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    connect(m_waveformPrethreshold, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    connect(m_waveformPeriod, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    //connect(m_cutOffFreq, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    //connect(m_filterOrder, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    //connect(m_signalEnergyWidth, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    //connect(m_multiplier, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));
    //connect(m_signalEnergyThreshold, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));

    ui->checkBoxFilters->raise();
}

OptionsDetectFrame::~OptionsDetectFrame()
{
	delete m_filters;
	delete m_signalEnergy;
	delete m_threshold;
	delete m_t1;
	delete m_t2;
	delete m_waveformLength;
	delete m_waveformPrethreshold;
    delete m_waveformPeriod;

	delete ui;
}

void OptionsDetectFrame::showEvent(QShowEvent *event)
{
	Q_UNUSED(event);

	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	slotThresholdTypeChanged(ccm->CurrentChannel());
	slotSignalEnergyTypeChanged(ccm->CurrentChannel());

	enableHighPassFilters(ui->checkBoxUseHighPassFilter->checkState() == Qt::Checked);
	enableSignalEnergy(ui->checkBoxUseSignalEnergy->checkState() == Qt::Checked);
}

void OptionsDetectFrame::slotChannelChanged(int channel)
{
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	QModelIndex index = ccm->index(channel, 0);

	slotThresholdTypeChanged(channel);
	slotSignalEnergyTypeChanged(channel);

    ChannelState state = (ChannelState)ccm->data(ccm->index(channel, CC_STATE), Qt::UserRole).toInt();
    slotChannelStateChanged(channel, state);
    ui->pushButtonDetect->setEnabled(!(state == CHANNELSTATE_DETECTING));
    ui->pushButtonDetect->setChecked(true);
}

void OptionsDetectFrame::on_checkBoxUseHighPassFilter_toggled(bool checked)
{
	enableHighPassFilters(checked);
}

void OptionsDetectFrame::on_checkBoxUseSignalEnergy_toggled(bool checked)
{
    enableSignalEnergy(checked);
}

// Purpose: Enable/disable high pass filter radio buttons, spin box and label
void OptionsDetectFrame::enableHighPassFilters(bool visible)
{
    ui->labelCutOffFreq->setEnabled(visible);
    ui->spinBoxCutOffFreq->setEnabled(visible);

	ui->labelFilterOrder->setEnabled(visible);
    ui->spinBoxFilterOrder->setEnabled(visible);
}

/// @author Holly Ausbeck
/// @date   May 18, 2015
/// @brief  This function enables/disables the detection options depending on the threshold type
/// @param [in] channel
void OptionsDetectFrame::slotThresholdTypeChanged(int channel)
{
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
    QString stype = ccm->data(ccm->index(channel, CC_THRESHOLD), Qt::DisplayRole).toString();

    ui->pushButtonDetect->setEnabled(true);
    ui->pushButtonDetect->setChecked(true);

    // this is a hack - fix it
    if (stype == "Single")
    {
        //case THRESHOLDTYPE_SINGLE:
        enableThreshold1(true);
        enableThreshold2(false);
        enableNoiseRMS(false);
        enableSignalEnergy(false);
        // enable everything but signal energy
        m_threshold->enableOne(ui->radioButtonSE, false);
    }
    else if (stype == "Dual")
    {
        //case THRESHOLDTYPE_DUAL:
        enableThreshold1(true);
        enableThreshold2(true);
        enableNoiseRMS(false);
        enableSignalEnergy(false);
        // enable everything but signal energy
        m_threshold->enableOne(ui->radioButtonSE, false);
    }
    else if (stype == "NoiseRMS")
    {
        //case THRESHOLDTYPE_NOISE_RMS:
        enableThreshold1(false);
        enableThreshold2(false);
        enableNoiseRMS(true);
        enableSignalEnergy(false);
        // enable everything but signal energy
        m_threshold->enableOne(ui->radioButtonSE, false);
    }
    else if (stype == "Energy")
    {
        //case THRESHOLDTYPE_ENERGY:
        enableThreshold1(false);
        enableThreshold2(false);
        enableNoiseRMS(false);
        enableSignalEnergy(true);
        // enable signal energy and nothing else
        m_threshold->enableOne(ui->radioButtonSE, true);
    }
    else if ((stype == "None") || stype.isEmpty())
    {
        //case THRESHOLDTYPE_NONE:
        enableThreshold1(false);
        enableThreshold2(false);
        enableNoiseRMS(false);
        enableSignalEnergy(false);
        m_threshold->enableOne(ui->radioButtonSE, false);
    }
	update();
}

/// @author Holly Ausbeck
/// @date   May 6, 2015
/// @brief  This function is called when a slider value in the model changes, if it is our slider, update
/// @param [in] channel
/// @param [in] type of slider that changed
void OptionsDetectFrame::slotSliderChanged(int channel, SliderType type)
{
	switch (type)
	{
	case SLIDERTYPE_THRESHOLD_DUAL:
	case SLIDERTYPE_THRESHOLD1:
	case SLIDERTYPE_THRESHOLD2:
	case SLIDERTYPE_THRESHOLD_ENERGY:
	case SLIDERTYPE_CUTOFF_FREQ:
        ui->pushButtonDetect->setEnabled(true);
        ui->pushButtonDetect->setChecked(true);
        break;
	}
}

/// @author Holly Ausbeck
/// @date   May 6, 2015
/// @brief  This function is called when a spinbox value changes
/// @param [in] new value
void OptionsDetectFrame::slotSelectionChanged(QString sValue)
{
    Q_UNUSED(sValue);
    ui->pushButtonDetect->setEnabled(true);
    ui->pushButtonDetect->setChecked(true);
}

/// @author Holly Ausbeck
/// @date   May 18, 2015
/// @brief  This function enables/disables the threshold1 options
/// @param [in] enable = true if the user can change them
void OptionsDetectFrame::enableThreshold1(bool enable)
{
	// column labels
	ui->labelADCCounts->setEnabled(enable);
	ui->labelPercentADCRange->setEnabled(enable);
	ui->labelMicroVolts->setEnabled(enable);

	// threshold 1
	m_t1->enable(enable);
}

/// @author Holly Ausbeck
/// @date   May 18, 2015
/// @brief  This function enables/disables the threshold2 options
/// @param [in] enable = true if the user can change them
void OptionsDetectFrame::enableThreshold2(bool enable)
{
	ui->checkBoxSymetrical->setEnabled(enable);
    m_t2->enable(enable);

    if (enable)
    {
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        int threshold1 = ccm->data(ccm->index(ccm->CurrentChannel(), CC_THRESHOLD1), Qt::UserRole).toInt();

        if (threshold1 >= 0)
        {
            ui->spinBoxMicroVolts1->setMaximum(750);
            ui->spinBoxMicroVolts1->setMinimum(0);
            ui->spinBoxMicroVolts2->setMaximum(0);
            ui->spinBoxMicroVolts2->setMinimum(-750);

            ui->spinBoxADCCounts1->setMaximum(750);
            ui->spinBoxADCCounts1->setMinimum(0);
            ui->spinBoxADCCounts2->setMaximum(0);
            ui->spinBoxADCCounts2->setMinimum(-750);

            ui->doubleSpinBoxADCRange1->setMaximum(750);
            ui->doubleSpinBoxADCRange1->setMinimum(0);
            ui->doubleSpinBoxADCRange2->setMaximum(0);
            ui->doubleSpinBoxADCRange2->setMinimum(-750);
        }
        else
        {
            ui->spinBoxMicroVolts2->setMaximum(750);
            ui->spinBoxMicroVolts2->setMinimum(0);
            ui->spinBoxMicroVolts1->setMaximum(0);
            ui->spinBoxMicroVolts1->setMinimum(-750);

            ui->spinBoxADCCounts2->setMaximum(750);
            ui->spinBoxADCCounts2->setMinimum(0);
            ui->spinBoxADCCounts1->setMaximum(0);
            ui->spinBoxADCCounts1->setMinimum(-750);

            ui->doubleSpinBoxADCRange2->setMaximum(750);
            ui->doubleSpinBoxADCRange2->setMinimum(0);
            ui->doubleSpinBoxADCRange1->setMaximum(0);
            ui->doubleSpinBoxADCRange1->setMinimum(-750);
        }
    }
    else
    {
        ui->spinBoxMicroVolts1->setMaximum(750);
        ui->spinBoxMicroVolts1->setMinimum(-750);
        ui->spinBoxMicroVolts2->setMaximum(750);
        ui->spinBoxMicroVolts2->setMinimum(-750);

        ui->spinBoxADCCounts1->setMaximum(750);
        ui->spinBoxADCCounts1->setMinimum(-750);
        ui->spinBoxADCCounts2->setMaximum(750);
        ui->spinBoxADCCounts2->setMinimum(-750);

        ui->doubleSpinBoxADCRange1->setMaximum(750);
        ui->doubleSpinBoxADCRange1->setMinimum(-750);
        ui->doubleSpinBoxADCRange2->setMaximum(750);
        ui->doubleSpinBoxADCRange2->setMinimum(-750);
    }
}

/// @author Holly Ausbeck
/// @date   May 18, 2015
/// @brief  This function enables/disables the symetrical checkbox
/// @param [in] enable = true if the user can change it
void OptionsDetectFrame::enableNoiseRMS(bool enable)
{
	ui->labelMultiplier->setEnabled(enable);
    ui->spinBoxMultiplier->setEnabled(enable);
}

/// @author Holly Ausbeck
/// @date   May 18, 2015
/// @brief  This function enables/disables the signal energy radio button and the signal energy threshold combo box
/// @param [in] enable = true if the user can change them
void OptionsDetectFrame::enableSignalEnergy(bool enable)
{
    if (!enable)
    {
        ui->checkBoxUseSignalEnergy->setChecked(false);
    }
	ui->labelEnergyWindow->setEnabled(enable);
	ui->labeluVSquared->setEnabled(enable);
    ui->spinBoxSignalEnergy->setEnabled(enable);
    ui->spinBoxSignalEnergyThreshold->setEnabled(enable);
}

/// @author Holly Ausbeck
/// @date   May 6, 2015
/// @brief  This function is called when a filtering option changed. Enable the detect button.
/// @param [in] channel
void OptionsDetectFrame::slotFilteringChanged(int channel)
{
    ui->pushButtonDetect->setEnabled(true);
    ui->pushButtonDetect->setChecked(true);
}

/// @author Holly Ausbeck
/// @date   May 6, 2015
/// @brief  This function is called when a filtering option changed. Enable the detect button.
/// @param [in] channel
void OptionsDetectFrame::slotSignalEnergyTypeChanged(int channel)
{
    ui->pushButtonDetect->setEnabled(true);
    ui->pushButtonDetect->setChecked(true);
}


/// @author Holly Ausbeck
/// @date   Oct 10, 2016
/// @brief  This function is called when a channel selection state changes used to update on multiple selection
void OptionsDetectFrame::slotChannelSelectionChanged()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int nChannel = ccm->CurrentChannel();

    slotThresholdTypeChanged(nChannel);
    slotSignalEnergyTypeChanged(nChannel);
}

/// @author Holly Ausbeck
/// @date   May 6, 2015
/// @brief  This function is called when a waveform detection is started or stopped
/// @param [in] channel
/// @param [in] true = detecting
void OptionsDetectFrame::slotChannelStateChanged(int channel, ChannelState state)
{
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	if (channel == ccm->CurrentChannel())
	{
        enableDetect(state != CHANNELSTATE_DETECTING);

        if (state == CHANNELSTATE_DETECTING)
        {
            ui->pushButtonDetect->setText(QString("Detecting..."));
            ui->labelDetected->setText("");
            ui->pushButtonDetect->setEnabled(false);
        }
        else
		{
			quint64 nTotalNumWaveforms = ccm->data(ccm->index(ccm->CurrentChannel(), CC_NWAVEFORMS), Qt::UserRole).toLongLong();
			QString sDetected("No Waveforms Detected");

			if (nTotalNumWaveforms > 0)
			{
				QString sWaveforms(QString::number(nTotalNumWaveforms) + " Waveforms Detected");
				sDetected = sWaveforms;
			}
			ui->labelDetected->setText(sDetected);
            ui->pushButtonDetect->setText(QString("Detect"));
            ui->pushButtonDetect->setEnabled(true);
        }
	}
    if (ccm->detectionInProgress())
    {
        ui->pushButtonCancelDetect->setEnabled(true);
    }
    else
    {
        ui->pushButtonCancelDetect->setEnabled(false);
    }
}

/// @author Holly Ausbeck
/// @date   May 18, 2015
/// @brief  This function is called when the detect button is pushed
void OptionsDetectFrame::slotDetect()
{
    ui->pushButtonDetect->setFocus();
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ccm->QBatch(REQUESTTYPE_DETECT);
}

/// @author Holly Ausbeck
/// @date   Oct 14, 2016
/// @brief  This function is called when the cancel detect button is pushed
void OptionsDetectFrame::slotCancelDetect()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ccm->cancelDetect();
}

void OptionsDetectFrame::on_checkBoxFilters_toggled(bool checked)
{
	ui->widgetFilters->setVisible(!checked);
}

void OptionsDetectFrame::on_checkBoxThreshold_toggled(bool checked)
{
	ui->widgetThreshold->setVisible(!checked);
}

void OptionsDetectFrame::on_checkBoxWaveform_toggled(bool checked)
{
    ui->widgetWaveforms->setVisible(!checked);
}

void OptionsDetectFrame::enableDetect(bool enable)
{
    ui->widgetFilters->setEnabled(enable);
    ui->widgetThreshold->setEnabled(enable);
    ui->widgetWaveforms->setEnabled(enable);
}


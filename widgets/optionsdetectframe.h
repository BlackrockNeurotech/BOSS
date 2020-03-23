#ifndef OPTIONSDETECTFRAME_H
#define OPTIONSDETECTFRAME_H

#include <QFrame>

#include "../model/channelconfigmodel.h"
#include "../model/mapperwidgets/checkradiogroup.h"
#include "../model/mapperwidgets/radiogroup.h"
#include "../model/mapperwidgets/spinBoxGroup.h"
#include "../widgets/animatedbutton.h"

namespace Ui {
class OptionsDetectFrame;
}

class OptionsDetectFrame : public QFrame
{
    Q_OBJECT

public:
    explicit OptionsDetectFrame(QWidget *parent = 0);
    ~OptionsDetectFrame();

	CheckRadioGroup   *m_filters;
	CheckRadioGroup   *m_signalEnergy;
	RadioGroup        *m_threshold;
    SpinBoxGroup      *m_t1;
    SpinBoxGroup      *m_t2;
    SpinBoxGroup      *m_waveformLength;
    SpinBoxGroup      *m_waveformPrethreshold;
    SpinBoxGroup      *m_waveformPeriod;

protected:
	virtual void showEvent(QShowEvent *event);

public slots:
	void slotChannelChanged(int channel);
	void slotThresholdTypeChanged(int channel);
	void slotSliderChanged(int channel, SliderType type);
    void slotChannelStateChanged(int channel, ChannelState state);
    void slotFilteringChanged(int channel);
	void slotSignalEnergyTypeChanged(int channel);
    void slotSelectionChanged(QString sValue); 
    void slotChannelSelectionChanged();

private slots:
	void on_checkBoxUseHighPassFilter_toggled(bool checked);
	void on_checkBoxUseSignalEnergy_toggled(bool checked);
    void slotDetect();
    void slotCancelDetect();

	void on_checkBoxFilters_toggled(bool checked);
	void on_checkBoxThreshold_toggled(bool checked);
    void on_checkBoxWaveform_toggled(bool checked);

private:
	QString m_filterString;
	QString m_energyString;

	void enableHighPassFilters(bool visible);
	void enableThreshold1(bool enable);
	void enableThreshold2(bool enable);
	void enableNoiseRMS(bool enable);
    void enableSignalEnergy(bool enable);
    void enableDetect(bool enable);

	Ui::OptionsDetectFrame *ui;
};

#endif // OPTIONSDETECTFRAME_H

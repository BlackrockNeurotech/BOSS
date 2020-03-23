#ifndef OPTIONSSORTFRAME_H
#define OPTIONSSORTFRAME_H

#include <QFrame>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"
#include "../model/mapperwidgets/radiogroup.h"

namespace Ui {
class OptionsSortFrame;
}

class OptionsSortFrame : public QFrame
{
    Q_OBJECT

public:
    explicit OptionsSortFrame(QWidget *parent = 0);
    ~OptionsSortFrame();

	RadioGroup *m_sortMethod;
	
private slots:
    void on_checkBoxFeatures_toggled(bool checked);
    void on_checkBoxSort_toggled(bool checked);
	void on_radioButtonKMeans_toggled(bool checked);
	void on_pushButtonDefineClusters_toggled(bool checked);
	void on_pushButtonSort_clicked();
	void on_radioButtonTDistributionEM_toggled(bool checked);

public slots:
	void slotChannelChanged(int channel);
    void slotChannelStateChanged(int channel, ChannelState state);
    void slotSortFeaturesChanged(int channel);
    void slotGlEditStateChanged(int channel, GLWidgetState editState);

protected:
	virtual void showEvent(QShowEvent *event);

private:
	void EnableKMeans(bool visible);
	void EnableTDist(bool visible);
	void sortButtonGreen(bool green);

    GLWidgetState m_3DStateSave;

	Ui::OptionsSortFrame *ui;
};

#endif // OPTIONSSORTFRAME_H

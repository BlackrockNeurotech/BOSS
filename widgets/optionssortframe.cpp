#include "optionssortframe.h"
#include "ui_optionssortframe.h"
#include "../widgets/featurecorrelation.h"
#include "../model/configmodel.h"

OptionsSortFrame::OptionsSortFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OptionsSortFrame)
{
    ui->setupUi(this);

    m_sortMethod = new RadioGroup(this, CC_SORT_METHOD);
	m_sortMethod->addRadioButton(ui->radioButtonKMeans, ChannelData::SortMethodNames.at(SORTMETHOD_KMEANS));
	m_sortMethod->addRadioButton(ui->radioButtonTDistributionEM, ChannelData::SortMethodNames.at(SORTMETHOD_TDIST));
//	m_sortMethod->addRadioButton(ui->radioButtonTemplate, ChannelData::SortMethodNames.at(SORTMETHOD_TEMPLATE));

    m_3DStateSave = GLWIDGETSTATE_ROTATE;

    QIcon icon(":/new/Bitmaps/Bitmaps/Clusters.png");
    ui->pushButtonDefineClusters->setImage(":/new/Bitmaps/Bitmaps/Clusters.png");
    ui->pushButtonDefineClusters->setCheckable(true);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0); 

    // initialize the sort button
    ui->pushButtonSort->setCheckable(true);
    ui->pushButtonSort->setEnabled(true);
    ui->pushButtonSort->setChecked(true);

    //FeaturePushButton *gradient = new FeaturePushButton(this, -1, -1, Qt::white, false);
    //gradient->setMaximumHeight(ui->pushButtonDefineClusters->height() / 2);
    //ui->gridLayout_2->addWidget(gradient, 2, 0, 1, -1);

    ui->checkBoxFeatures->setChecked(false);
    ui->checkBoxFeatures->raise();
    on_checkBoxFeatures_toggled(false);
    ui->checkBoxSort->setChecked(false);
    on_checkBoxSort_toggled(false);

    ui->widgetFeatureCorrelation->raise();
    
    // map the SpinBoxes to the model
    ui->spinBoxClusters->setChannelColumn(CC_NBR_SORT_UNITS);
    ui->doubleSpinBoxPenaltyFactor->setChannelColumn(CC_PENALTY_FACTOR);

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
	connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(ccm, SIGNAL(channelStateChanged(int, ChannelState)), this, SLOT(slotChannelStateChanged(int, ChannelState)));
    connect(ccm, SIGNAL(sortFeaturesChanged(int)), this, SLOT(slotSortFeaturesChanged(int)));
    connect(ccm, SIGNAL(glEditStateChanged(int, GLWidgetState)), this, SLOT(slotGlEditStateChanged(int, GLWidgetState)));
}

OptionsSortFrame::~OptionsSortFrame()
{
	delete m_sortMethod;
	delete ui;
}

void OptionsSortFrame::showEvent(QShowEvent *event)
{
	EnableKMeans(ui->radioButtonKMeans->isChecked());
	EnableTDist(ui->radioButtonTDistributionEM->isChecked());

}

void OptionsSortFrame::slotChannelChanged(int channel)
{
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	QModelIndex index = ccm->index(channel, 0);

    GLWidgetState editState = (GLWidgetState)ccm->data(ccm->index(channel, CC_SORT_METHOD), Qt::UserRole).toInt();
    slotGlEditStateChanged(channel, editState);
    slotSortFeaturesChanged(channel);

    // Reset the status of the sort button
    ChannelState state = (ChannelState)ccm->data(ccm->index(channel, CC_STATE), Qt::UserRole).toInt();
    slotChannelStateChanged(channel, state);
    sortButtonGreen(true);
}

void OptionsSortFrame::slotSortFeaturesChanged(int channel)
{
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	unsigned int selectedFeatures = ccm->data(ccm->index(channel, CC_FEATURE), Qt::UserRole).toInt();
	
	if (selectedFeatures == 0)
	{
		sortButtonGreen(false);
		ui->radioButtonTDistributionEM->setEnabled(false);
		EnableTDist(false);
		ui->radioButtonKMeans->setEnabled(false);
		EnableKMeans(false);
	}
	else
	{
		sortButtonGreen(true);
		ui->radioButtonTDistributionEM->setEnabled(true);
		EnableTDist(ui->radioButtonTDistributionEM->isChecked());
		ui->radioButtonKMeans->setEnabled(true);
		EnableKMeans(ui->radioButtonKMeans->isChecked());
	}
}

void OptionsSortFrame::slotChannelStateChanged(int channel, ChannelState state)
{
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	if (channel == ccm->CurrentChannel())
	{
        this->setDisabled(state == CHANNELSTATE_SORTING);

        if (state == CHANNELSTATE_SORTING)
        {
            ui->pushButtonSort->setText(QString("Sorting..."));
            sortButtonGreen(false);
        }
		else
        {
            ui->pushButtonSort->setText(QString("Sort"));
            sortButtonGreen(true);
        }
	}
}

void OptionsSortFrame::slotGlEditStateChanged(int channel, GLWidgetState editState)
{
    ui->pushButtonDefineClusters->blockSignals(true);

    if (editState == GLWIDGETSTATE_CENTROIDS)
    {
        ui->spinBoxClusters->setEnabled(false);
        ui->pushButtonDefineClusters->setChecked(true);
    }
    else
    {
        if (ui->radioButtonKMeans->isChecked())
            ui->spinBoxClusters->setEnabled(true);

        ui->pushButtonDefineClusters->setChecked(false);
    }
    ui->pushButtonDefineClusters->blockSignals(false);
}


void OptionsSortFrame::on_radioButtonKMeans_toggled(bool checked)
{
	EnableKMeans(checked);
    sortButtonGreen(true);
}

void OptionsSortFrame::on_radioButtonTDistributionEM_toggled(bool checked)
{
	EnableTDist(checked);
    sortButtonGreen(true);
}

// Purpose: Enable/disable Number of clusters and define centroids
void OptionsSortFrame::EnableKMeans(bool visible)
{
	ui->labelClusters->setEnabled(visible);
	ui->spinBoxClusters->setEnabled(visible);
	ui->pushButtonDefineClusters->setEnabled(visible);
}

// Purpose: Enable/disable Number of clusters and define centroids
void OptionsSortFrame::EnableTDist(bool visible)
{
	ui->labelPenaltyFactor->setEnabled(visible);
	ui->doubleSpinBoxPenaltyFactor->setEnabled(visible);
}

/// @author Holly Ausbeck
/// @date   May 18, 2015
/// @brief  This function is called when a value that impacts detection changes, change the detect button to green
/// @param [in] green is true if the detected waveforms would change if detection is run now
void OptionsSortFrame::sortButtonGreen(bool green)
{
	if (green)
	{
        ui->pushButtonSort->setEnabled(true);
        ui->pushButtonSort->setChecked(true);
    }
	else
	{
        ui->pushButtonSort->setEnabled(false);
        ui->pushButtonSort->setChecked(false);
	}
	ui->pushButtonSort->update();
}

void OptionsSortFrame::on_pushButtonDefineClusters_toggled(bool checked)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    
    if (checked)
	{
        m_3DStateSave = (GLWidgetState)ccm->data(ccm->index(channel, CC_3D_EDIT_STATE), Qt::UserRole).toInt();
		ccm->setData(ccm->index(channel, CC_3D_EDIT_STATE), QVariant(GLWIDGETSTATE_CENTROIDS), Qt::UserRole);
	}
	else
	{
        ccm->setData(ccm->index(channel, CC_3D_EDIT_STATE), QVariant(m_3DStateSave), Qt::UserRole);
	}
    sortButtonGreen(true);
}

void OptionsSortFrame::on_pushButtonSort_clicked()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();

    ccm->QBatch(REQUESTTYPE_SORT);
}

void OptionsSortFrame::on_checkBoxFeatures_toggled(bool checked)
{
    ui->widgetClusteringTendency->setVisible(!checked);
	ui->widgetFeatureCorrelation->setVisible(!checked);
}

void OptionsSortFrame::on_checkBoxSort_toggled(bool checked)
{
    ui->widgetSort->setVisible(!checked);
}


#include "feature3dtoolbar.h"


Feature3DToolBar::Feature3DToolBar(QWidget *parent) :
QToolBar(parent)
{
    this->setStyleSheet("QToolBar {border-bottom: none; border-top: none;}");

    m_comboX = new ComboWidgetAction(this, CC_X_3DAXIS, ChannelData::FeatureIDNames, QString("x-axis"), "color: red;");
    m_comboY = new ComboWidgetAction(this, CC_Y_3DAXIS, ChannelData::FeatureIDNames, QString("y-axis"), "color: green;");
    m_comboZ = new ComboWidgetAction(this, CC_Z_3DAXIS, ChannelData::FeatureIDNames, QString("z-axis"), "color: blue;");

    m_actionReset = new ToolbarWidgetAction(this, "", ACTIONWIDGETTYPE_BLACKROCKBUTTON);
    m_actionReset->setToolTip(QString("Reset Rotation"));
    m_actionReset->setImage(":/new/Bitmaps/Bitmaps/Reset.png");

    QStringList images = QStringList() << ":/new/Bitmaps/Bitmaps/Rotate.png" << ":/new/Bitmaps/Bitmaps/Select.png" << ":/new/Bitmaps/Bitmaps/Clusters.png";
    QStringList tooltips = QStringList() << "Rotate" << "Edit" << "Select Clusters";
    m_actionMode = new ModeWidgetAction(this, CC_3D_EDIT_STATE, ChannelData::GLWidgetStateNames, tooltips, images);

    this->addAction(m_comboX);
    this->addAction(m_comboY);
    this->addAction(m_comboZ);
    this->addSeparator();
    this->addAction(m_actionReset);
    this->addSeparator();
    this->addAction(m_actionMode);
    
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
	connect(ccm, SIGNAL(glEditStateChanged(int, GLWidgetState)), this, SLOT(slotGlEditStateChanged(int, GLWidgetState)));
    
    this->setContentsMargins(0, 0, 0, 0);
}

Feature3DToolBar::~Feature3DToolBar()
{
}

void Feature3DToolBar::slotChannelChanged(int channel)
{
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	GLWidgetState editState = (GLWidgetState)ccm->data(ccm->index(channel, CC_SORT_METHOD), Qt::UserRole).toInt();
	
    slotGlEditStateChanged(channel, editState);
}

void Feature3DToolBar::slotGlEditStateChanged(int channel, GLWidgetState editState)
{
	Q_UNUSED(editState);

	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	SortMethod method = (SortMethod)ccm->data(ccm->index(channel, CC_SORT_METHOD), Qt::UserRole).toInt();

	if (method == SORTMETHOD_KMEANS)
		m_actionMode->enable(GLWIDGETSTATE_CENTROIDS, true);
	else
		m_actionMode->enable(GLWIDGETSTATE_CENTROIDS, false);
}


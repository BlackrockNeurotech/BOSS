#include "feature2dtoolbar.h"

Feature2DToolBar::Feature2DToolBar(QWidget *parent) :
QToolBar(parent)
{
    this->setStyleSheet("QToolBar {border-bottom: none; border-top: none;}");
    m_comboX = new ComboWidgetAction(this, CC_X_2DAXIS, ChannelData::FeatureIDNames, QString("x-axis"), "color: red;");
    m_comboY = new ComboWidgetAction(this, CC_Y_2DAXIS, ChannelData::FeatureIDNames, QString("y-axis"), "color: green;");

    this->addAction(m_comboX);
    this->addAction(m_comboY);

    this->setContentsMargins(0, 0, 0, 0);
}

Feature2DToolBar::~Feature2DToolBar()
{
}


#include "../widgets/MainWidgetToolBar.h"


MainWidgetToolBar::MainWidgetToolBar(QWidget *parent):
    QToolBar(parent)
{
    this->setStyleSheet("QToolBar {border-bottom: none; border-top: none; }");
    m_actionUnits = 0;
    m_actionPointSize = 0;
    m_actionUpdatePCA = 0;
}

MainWidgetToolBar::~MainWidgetToolBar()
{
    if (m_actionUnits)
    {
        delete m_actionUnits;
        m_actionUnits = NULL;
    }
    if (m_actionPointSize)
    {
        delete m_actionPointSize;
        m_actionPointSize = NULL;
    }
    if (m_actionUpdatePCA)
    {
        delete m_actionUpdatePCA;
        m_actionUpdatePCA = NULL;
    }
}

void MainWidgetToolBar::initialize()
{
    if (m_actionUnits == 0)
    {
        m_actionUnits = new UnitSelectionAction(this, false, CC_UNITS_VISIBLE);
		m_actionUnits->setToolTip("Show or Hide Units");

        m_actionPointSize = new PointSizeWidgetAction(this);
		m_actionPointSize->setToolTip("Change Feature Point Size");

        m_actionUpdatePCA = new ToolbarWidgetAction(this, QString(), ACTIONWIDGETTYPE_BLACKROCKBUTTON);
        m_actionUpdatePCA->setImage(":/new/Bitmaps/Bitmaps/pca_PB.png");
        m_actionUpdatePCA->setToolTip("Update PCA for current channel");
        

        connect(m_actionUpdatePCA, SIGNAL(pressed()), ChannelConfigModel::instance(), SLOT(slotUpdatePCA()));


        this->addAction(m_actionUnits);
        this->addSeparator();
        this->addAction(m_actionPointSize);
        this->addSeparator();
        this->addAction(m_actionUpdatePCA);

        BlackrockButton * pb = (BlackrockButton *)m_actionUpdatePCA->getWidget();
        pb->setEnabled(true);
        pb->setCheckable(true);

        this->setContentsMargins(0, 0, 0, 0);
    }
}


#include "featurecorrelation.h"
#include "../graphics/featuregraphicsview.h"
#include "../widgets/optionssortframe.h"


// button
FeaturePushButton::FeaturePushButton(QWidget *parent, int row, int col, QColor color, bool bSquare) :
    QPushButton(parent)
{
    m_row = row;
    m_col = col;
    this->setCheckable(true);
    setColor(color);
    setAttribute(Qt::WA_LayoutUsesWidgetRect);
	m_bSquare = bSquare;

    setMinimumWidth(5);
    setMinimumHeight(0);
    //setMaximumWidth(20); 

	QSizePolicy policy;
	policy.setControlType(QSizePolicy::PushButton);
	policy.setHeightForWidth(false);
	policy.setRetainSizeWhenHidden(true);

	if (bSquare)
	{
        policy.setHorizontalPolicy(QSizePolicy::Maximum);
		policy.setVerticalPolicy(QSizePolicy::Maximum);
	}
	else
	{
        policy.setHorizontalPolicy(QSizePolicy::Minimum);
        policy.setVerticalPolicy(QSizePolicy::Minimum);
	}
    setSizePolicy(policy);

	QObject::connect(this, SIGNAL(clicked(bool)), this, SLOT(slotTriggered(bool)));
}



void FeaturePushButton::setColor(QColor color)
{
    m_color = color.lighter(125);
    m_colorDark = m_color.darker(125);

    m_colorHover = m_color.lighter(125);
    m_colorHoverDark = m_colorDark.lighter(125);

    m_colorGreen = m_color;
    m_colorGreen.setRed(0);
    m_colorGreen.setBlue(0);
    m_colorGreenDark = m_colorGreen.darker(125);

    m_colorHoverSelected = m_colorGreen.lighter(125);
    m_colorHoverSelectedDark = m_colorGreenDark.lighter(125);

    this->update();
}

void FeaturePushButton::paintEvent(QPaintEvent *event)
{
    if (m_row == 0)
        return;

    QPainter painter(this);
    QRect r(this->geometry());
    r.moveTo(0, 0);

    if (m_row == -1)
    {
        QLinearGradient gradient(0, 0, r.width(), 0);
        gradient.setColorAt(0, QColor(Qt::white));
        gradient.setColorAt(1, QColor(Qt::black));
        painter.fillRect(r, gradient);
        return;
    }

    QLinearGradient gradient(0, 0, 0, r.height());
    gradient.setColorAt(0, m_color);
    gradient.setColorAt(1, m_colorDark);

    if (this->isDown())
    {
        gradient.setColorAt(0, m_colorDark);
        gradient.setColorAt(1, m_color);
    }
    else if (this->underMouse())
    {
//        if (this->isChecked())
//        {
//            gradient.setColorAt(0, m_colorHoverSelected);
//            gradient.setColorAt(1, m_colorHoverSelectedDark);
//        }
//        else
//        {
            gradient.setColorAt(0, m_colorHover);
            gradient.setColorAt(1, m_colorHoverDark);
//        }
    }
//    else if (this->isChecked())
//    {
//        gradient.setColorAt(0, m_colorGreen);
//        gradient.setColorAt(1, m_colorGreenDark);
//    }
    painter.fillRect(r, gradient);
    QPen pen(Qt::white);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(r);
//    int width = r.width() / 5;
//    if (!this->isChecked())
//    {
//        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
//        painter.fillRect(r.left() + 5, r.top() + 5, 4, 4, QColor(Qt::white));
//    }
    if (this->isChecked())
    {
        QPen penSelected("#009FE3");
        penSelected.setWidth(2);
        QRect rCenter(r.left() + 1, r.top() + 1, r.width() - 2, r.height() - 2);
        painter.setPen(penSelected);
        painter.drawRect(rCenter);
    }
}

FeaturePushButton::~FeaturePushButton()
{
}

void FeaturePushButton::slotTriggered(bool checked)
{
    emit triggered(checked, m_row, m_col);
}

/*////////////////////////////////////////////////////////////////////////////////////
///  FeatureCorrelation::FeatureCorrelation
///
/////////////////////////////////////////////////////////////////////////////////////*/

FeatureCorrelation::FeatureCorrelation(QWidget *parent):
    QWidget(parent)
{
	setAttribute(Qt::WA_LayoutUsesWidgetRect, true);

	QSizePolicy policy;
	policy.setControlType(QSizePolicy::DefaultType);
    policy.setHeightForWidth(false);
	policy.setRetainSizeWhenHidden(true);
    policy.setHorizontalPolicy(QSizePolicy::Preferred);
    policy.setVerticalPolicy(QSizePolicy::Preferred);
    setSizePolicy(policy);


    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(ccm, SIGNAL(clusteringTendencyChanged(int)), this, SLOT(slotCorrMatrixReady(int)));

}

int FeatureCorrelation::heightForWidth(int width) const
{
    return width;
}

void FeatureCorrelation::showEvent(QShowEvent *event)
{
//	initializeSizes();
}

void FeatureCorrelation::resizeEvent(QResizeEvent *event)
{
//    initializeSizes();
}

void FeatureCorrelation::initializeSizes()
{
	if (m_features.size() == 0)
    {
        m_mainLayout = (QGridLayout *)layout();
        m_mainLayout->setVerticalSpacing(0);
        m_mainLayout->setHorizontalSpacing(0);

		initializeFeatures();
		initializeSelections();
		populateFeatures();
        populateSelections();

    }

}

void FeatureCorrelation::initializeFeatures()
{
    // set up the CheckBoxGroup to wrap the check boxes
    m_checkboxGroup = new CheckBoxGroup(this, CC_FEATURE);
    uint bit = 1;

	for (int i = 0; i < FEATUREID_END -1; i++)
    {
        QLayoutItem * item = m_mainLayout->itemAtPosition(i, 0);
        QCheckBox *feature = (QCheckBox *) item->widget();
        feature->setMinimumWidth(feature->sizeHint().width() + 4);
        feature->hide();
        m_features.push_back(feature);
        m_checkboxGroup->setSelectionId((QCheckBox*)feature, bit);
        bit = bit << 1;
    }
}

void FeatureCorrelation::populateFeatures()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    unsigned int selectedFeatures = ccm->data(ccm->index(channel, CC_FEATURE), Qt::UserRole).toInt();
    int mask = FEATURE_PCA_1;

    for (int i = FEATUREID_PCA1; i < FEATUREID_END-1; i++)
    {
        QCheckBox *feature = m_features.at(i);

        if (selectedFeatures & mask)
            feature->setChecked(true);
        else
            feature->setChecked(false);

        feature->show();


        mask = mask << 1;
    }

}

void FeatureCorrelation::initializeSelections()
{
    for (int row = 0; row < m_features.size(); row++)
    {
        for (int col = 0; col < m_features.size(); col++)
        {
            if (col == row)
                break;

            FeaturePushButton *selection = new FeaturePushButton(this, row, col, Qt::gray);
            selection->hide();
            QString sX = m_features.at(col)->text();
            QString sY = m_features.at(row)->text();
            selection->setToolTip(sY + " vs " + sX);
            m_selections.push_back(selection);
            connect(selection, SIGNAL(triggered(bool, int, int)), this, SLOT(slotTriggered(bool, int, int)));
        }
    }
}

void FeatureCorrelation::populateSelections()
{
    FeaturePushButton *blank = new FeaturePushButton(this, 0, 0, Qt::white);
	m_mainLayout->addWidget(blank, 0, 1, 1, 1);
    blank->show();
    int index = 0;

    for (int row = 0; row < m_features.size(); row++)
    {
        for (int col = 0; col < m_features.size(); col++)
        {
            if (col == row)
                break;

            FeaturePushButton *selection = m_selections.at(index);
            m_mainLayout->addWidget(selection, row, col + 1, 1, 1);
            selection->show();
            index++;
        }
    }

    for (int row = 0; row < m_features.size(); row++)
    {
        QLayoutItem * item;
        QWidget * w;

        item = m_mainLayout->itemAtPosition(row, 0);
        if(item)
        {
            item->setAlignment(Qt::AlignLeft);
            w = item->widget();
            if(w)
            {
                w->raise();
            }
        }
        if (row > 2)
            break;
    }
//    QLabel *clustering = new QLabel("Clustering Tendency", this);
//    m_mainLayout->addWidget(clustering, 2, 10, 1, 9);

//    QLabel *poor = new QLabel("poor", this);
//    m_mainLayout->addWidget(poor, 3, 10, 1, 9);

//    QLabel *good = new QLabel(" good", this);
//    m_mainLayout->addWidget(good, 3, 15, 1, 5);

//    FeaturePushButton *gradient = new FeaturePushButton(this, -1, -1, Qt::white, false);
//    m_mainLayout->addWidget(gradient, 4, 10, 1, 7);

    m_mainLayout->setColumnStretch(0, 2);
	for (int col = 1; col < m_features.size(); col++)
        m_mainLayout->setColumnStretch(col, 1);
}

void FeatureCorrelation::selectCurrentSelections()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();

    QMap<QString, QVariant> selection = ccm->data(ccm->index(channel, CC_SELECTED_FEATURES), Qt::UserRole).toMap();
    QMap<QString, QVariant>::iterator it = selection.begin();
    QMultiMap<int, int> sel;

    // convert from QMap<QString, QVariant> to QMultiMap<int, int>
    while (it != selection.end())
    {
        sel.insert(it.key().toInt(), it.value().toInt());
        it++;
    }
    foreach (FeaturePushButton *selection, m_selections)
    {
        if (sel.contains(selection->row(), selection->col()))
            selection->setChecked(true);
        else
            selection->setChecked(false);
    }
}

void FeatureCorrelation::saveCurrentSelections()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();

    QMap<QString, QVariant> selectionMap;

    foreach (FeaturePushButton *selection, m_selections)
    {
        if (selection->isChecked())
            selectionMap.insertMulti(QString::number(selection->row()), QString::number(selection->col()));
    }
    ccm->setData(ccm->index(channel, CC_SELECTED_FEATURES), QVariant(selectionMap), Qt::UserRole);
}

void FeatureCorrelation::slotTriggered(bool checkState, int row, int col)
{
    saveCurrentSelections();
}

void FeatureCorrelation::slotCorrMatrixReady(int channel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ClusteringTendencyAccum accum = ccm->data(ccm->index(channel, CC_SPIKEDATA_CLUSTERING_MATRIX)).value<ClusteringTendencyAccum>();
    
    if (accum)
    {
		initializeSizes();
        QMap<double, int> best;

        arma::mat colorMatrix = accum->matrix;
        int index = 0;

        for (int row = 0; row < m_features.size(); row++)
        {
            for (int col = 0; col < m_features.size(); col++)
            {
                if (col == row)
                    break;

                FeaturePushButton *selection = m_selections.at(index);
                double value = colorMatrix.at(row, col);
                value = 1.0 - value;
                if (value < 0.5)
                    value += (0.5 - value) * 0.2;
                selection->setColor(QColor::fromRgbF(value, value, value));
				selection->show();
                best.insert(value, index);
                index++;
            }
        }
        // if no features are selected, select the best 4    
        QMap<QString, QVariant> selection = ccm->data(ccm->index(channel, CC_SELECTED_FEATURES), Qt::UserRole).toMap();

        if (selection.isEmpty())
        {
            QMapIterator<double, int> it(best);
            int count = 0;

            while (it.hasNext())
            {
                it.next();

                FeaturePushButton *sel = m_selections.at(it.value());
                selection.insertMulti(QString::number(sel->row()), QString::number(sel->col()));

                if (count++ >= 3)
                    break;
            }
            ccm->setData(ccm->index(channel, CC_SELECTED_FEATURES), QVariant(selection), Qt::UserRole);
        }
        this->selectCurrentSelections();
    }
}

void FeatureCorrelation::slotChannelChanged(int channel)
{

	initializeSizes();

	ChannelConfigModel *ccm = ChannelConfigModel::instance();
    QModelIndex index = ccm->index(channel, 0);

    //m_checkboxGroup->blockSignals(true);
    //m_checkboxGroup->blockSignals(false);

    selectCurrentSelections();

}

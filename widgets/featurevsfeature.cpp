#include <QSizePolicy>

#include "featurevsfeature.h"


FeatureVSFeature::FeatureVSFeature(QWidget *parent):
    QWidget(parent)
{
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);
    this->setLayout(m_layout);
    this->setContentsMargins(0, 0, 0, 0);

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(ccm, SIGNAL(viewFeaturesChanged(int)), this, SLOT(slotViewFeaturesChanged(int)));
    connect(ccm, SIGNAL(featuresChanged(int)), this, SLOT(slotFeaturesChanged(int)));
    m_channelChange = false;
}

void FeatureVSFeature::layoutWidgets()
{
    foreach (QWidget *widget, m_widgets)
        m_layout->removeWidget(widget);
    delete m_layout;
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);
    this->setLayout(m_layout);

    double sq = sqrt((double)m_widgets.size());
    int ncol = (int)sq;

    if ((double)ncol != sq)
        ncol++;

    int row = 0;
    int col = 0;

    foreach (QWidget *widget, m_widgets)
    {
        m_layout->addWidget(widget, row, col, 1, 1);
        col++;

        if (col == ncol)
        {
            col = 0;
            row++;
        }
    }
    for (int i = 0; i < m_layout->columnCount(); i++)
    {
        m_layout->setColumnStretch(i, 5);
    }
    for (int i = 0; i < m_layout->rowCount(); i++)
    {
        m_layout->setRowStretch(i, 5);
    }
}

FeatureGraphicsView* FeatureVSFeature::findWidget(FeatureID idX, FeatureID idY)
{
    FeatureGraphicsView *view = 0;

    foreach (FeatureGraphicsView *widget, m_widgets)
    {
        if (widget->featuresMatch(idX, idY))
        {
            view = widget;
            break;
        }
    }
    return view;
}

void FeatureVSFeature::removeFeature(FeatureID id)
{
    foreach (FeatureGraphicsView *widget, m_widgets)
    {
        if (widget->featureMatches(id))
        {
            m_widgets.removeOne(widget);
            delete widget;
        }
    }
}

void FeatureVSFeature::slotViewFeaturesChanged(int channel)
{
    Q_UNUSED(channel);

	this->parentWidget()->raise();
	updateFeatures(channel);
}

void FeatureVSFeature::newFeatures(int channel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();

    QMultiMap<QString, QVariant> selection = ccm->data(ccm->index(channel, CC_SELECTED_FEATURES), Qt::UserRole).toMap();
    QMultiMap<QString, QVariant>::iterator it = selection.begin();
    QMultiMap<int, int> sel;

    while (it != selection.end())
    {
        sel.insert(it.key().toInt(), it.value().toInt());
        it++;
    }
    // remove widgets that are no longer selected
    while (sel.size() < m_widgets.size())
    {
        FeatureGraphicsView *widget = m_widgets.last();
        m_widgets.pop_back();
        delete widget;
    }
    // add widgets that are selected but not visible
    QMultiMap<int, int>::iterator itt = sel.begin();
    int i = 0;

    while (itt != sel.end())
    {
        if (i >= m_widgets.size())
        {
            FeatureGraphicsView *widget = new FeatureGraphicsView((FeatureID)itt.key(), (FeatureID)itt.value(), 0, true);
            m_widgets.push_back(widget);
        }
        else
            m_widgets[i]->setFeatures((FeatureID)itt.key(), (FeatureID)itt.value());

        itt++;
        i++;
    }
    layoutWidgets();
}

void FeatureVSFeature::updateFeatures(int channel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();

    QMultiMap<QString, QVariant> selection = ccm->data(ccm->index(channel, CC_SELECTED_FEATURES), Qt::UserRole).toMap();
    QMultiMap<QString, QVariant>::iterator it = selection.begin();
    QMultiMap<int, int> sel;

    while (it != selection.end())
    {
        sel.insert(it.key().toInt(), it.value().toInt());
        it++;
    }
    // remove widgets that are no longer selected
    foreach(FeatureGraphicsView *widget, m_widgets)
    {
        int row = (int)widget->featureX();
        int col = (int)widget->featureY();

        if (!sel.contains(row, col))
        {
            this->RemoveFeatures((FeatureID)row, (FeatureID)col);
        }
    }
    // add widgets that are selected but not visible
    QMap<int, int>::iterator i = sel.begin();

    while (i != sel.end())
    {
        if (findWidget((FeatureID)i.key(), (FeatureID)i.value()) == 0)
        {
            AddFeatures((FeatureID)i.key(), (FeatureID)i.value());
        }
        i++;
    }
}

void FeatureVSFeature::AddFeatures(FeatureID idX, FeatureID idY)
{
    if (findWidget(idX, idY) == 0)
    {
        FeatureGraphicsView *widget = new FeatureGraphicsView(idX, idY, 0, true);
        m_widgets.push_back(widget);
        layoutWidgets();
    }
}

void FeatureVSFeature::RemoveFeatures(FeatureID idX, FeatureID idY)
{
    FeatureGraphicsView *widget = findWidget(idX, idY);

    if (widget != 0)
    {
        m_widgets.removeOne(widget);
        delete widget;
        layoutWidgets();
    }
}

void FeatureVSFeature::slotChannelChanged(int channel)
{
    m_channelChange = true;
}

void FeatureVSFeature::slotFeaturesChanged(int channel)
{
    if (m_channelChange == true)
    {
        m_channelChange = false;
        newFeatures(channel);

    }
    else
        updateFeatures(channel);
}


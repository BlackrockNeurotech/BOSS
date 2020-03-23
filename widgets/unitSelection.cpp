#include "unitSelection.h"
#include "../graphics/featuregraphicsview.h"


// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose: Checkbox for unit selection
//          The box is the color of it's assigned unit. If unselected, the middle is empty
// Inputs:
//          parent - parent widget
//          unit - unit number
//          color - unit color

UnitCheckBox::UnitCheckBox(QWidget *parent, int unit, QColor color) :
    QPushButton(parent)
{
    m_unit = unit;
    this->setCheckable(true);
    setColor(color);
    this->setMaximumHeight(16);
    this->setMaximumWidth(16);
    setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

void UnitCheckBox::setColor(QColor color)
{
    m_color = color.lighter();
    m_colorDark = color;
    m_colorLight = m_color.lighter();
    this->update();
}

void UnitCheckBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect r(this->geometry());
    r.moveTo(0, 0);
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
        gradient.setColorAt(0, m_colorLight);
        gradient.setColorAt(1, m_color);
    }
    painter.fillRect(r, gradient);
    painter.setPen(QPen(Qt::black));
    painter.drawRect(r);

    if (!this->isChecked())
    {
        //QPen pen(Qt::black);
        //pen.setWidth(2);
        //painter.setPen(pen);
        QRectF ellipseRect(r.left() + 3, r.top() + 3, r.width() - 6, r.height() - 6);
        painter.drawEllipse(ellipseRect);
        ellipseRect.setRect(ellipseRect.left() + 2, ellipseRect.top() + 2, ellipseRect.width() - 4, ellipseRect.height() - 4);
        painter.drawLine(ellipseRect.bottomLeft(), ellipseRect.topRight());
        //r.setRect(r.left() + 3, r.top() + 3, r.width() - 6, r.height() - 6);
        //painter.fillRect(r, Qt::white);
    }
}

void UnitCheckBox::mousePressEvent(QMouseEvent *event)
{
    bool bMenu = !((unitSelection *)parent())->usesVisibleUnits();      // If it doesn't use visible units, this is the visibility toolbar
    if (event->buttons() & Qt::RightButton && bMenu)
    {
        UnitEditMenu *menu = new UnitEditMenu(this, m_unit);
        menu->exec(QCursor::pos());

        menu->deleteLater();
    }
    
    QPushButton::mousePressEvent(event);

}

UnitCheckBox::~UnitCheckBox()
{
}


// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose: Unit Selection Widget
//          This widget contains a UnitCheckBox for each sorted unit
//          The user may select or unselect each unit
//          This widget connects to signals: unitChanged, sortResultsReady, unitsReady
//          so the widget updates it's unit display as units change
//          This widget is intended to work with a model, and may be mapped to a model column
// Inputs:
//          parent - parent widget
//          useVisibleUnits - true means only show visible units
//          mainWidget - true if this is not a right click tool bar widget
//                       which must manually resize the parent widget when the number of units change

unitSelection::unitSelection(QWidget *parent, bool useVisibleUnits, bool mainWidget):
    QWidget(parent)
{
    m_mainWidget = mainWidget;
    m_useVisibleUnits = useVisibleUnits;

    m_layout = new QHBoxLayout();
    m_layout->setSpacing(0);
    this->setLayout(m_layout);
    this->setContentsMargins(0, 0, 0, 0);
    m_layout->setContentsMargins(1, 0, 1, 0);

    // get the color configuration from the model
    ConfigModel *cm = ConfigModel::instance();
    m_unitColors = cm->getColors();

    ChannelConfigModel *ccm = ChannelConfigModel::instance();

    connect(ChannelConfigModel::instance(), SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(ccm, SIGNAL(channelStateChanged(int, ChannelState)), this, SLOT(slotChannelStateChanged(int, ChannelState)));

    if (m_useVisibleUnits)
    {
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        int channel = ccm->CurrentChannel();

        QStringList visibleUnits = ccm->data(ccm->index(channel, CC_UNITS_VISIBLE), Qt::DisplayRole).toStringList();

        foreach (QString s, visibleUnits)
            m_visibleUnits.push_back(s.toInt());

        // stay informed if visible units change
        connect(ccm, SIGNAL(visibleUnitsChanged(int, QStringList)), this, SLOT(slotVisibleUnitsChanged(int, QStringList)));

        // intiialize the model value so a signal isn't sent on the first update
        setSelection(ccm->data(ccm->index(channel, CC_UNITS_ALIGN), Qt::DisplayRole).toStringList());
    }
}


// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose: loop through the check box widgets and return a vector of the unit numbers selected

QVector<int> unitSelection::getCheckedUnits()
{
    QVector<int> checkedUnits;

    foreach (UnitCheckBox *cb, m_checkboxes)
    {
        if (cb->isChecked())
        {
            checkedUnits.push_back(cb->unit());
        }
    }
    return checkedUnits;
}

// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose:  Remove all existing unit buttons from the layout and delete them
void unitSelection::clearUnits()
{
    foreach (UnitCheckBox *check, m_checkboxes)
    {
        m_layout->removeWidget(check);
        m_checkboxes.removeAt(m_checkboxes.indexOf(check));
        delete check;
    }
}

// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose:  Get the current number of units from the channel model and display them
void unitSelection::setUnits(bool useVisibleUnits)
{
    ChannelConfigModel * ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    UnitAccum unitAccum = ccm->data(ccm->index(channel, CC_SPIKEDATA_UNITS), Qt::UserRole).value<UnitAccum>();

    if (useVisibleUnits)
    {
        // update the visible unit list
        QStringList visibleUnits = ccm->data(ccm->index(channel, CC_UNITS_VISIBLE), Qt::DisplayRole).toStringList();
        m_visibleUnits.clear();

        foreach(QString s, visibleUnits)
            m_visibleUnits.push_back(s.toInt());
    }
    if (unitAccum)
    {
        QVector<int> usedUnits = unitAccum->getUnitNumbers(false);

        // the units aren't ordered, and remove unit zero which is unsorted if there are any
        if (usedUnits.size() > 0)
        {
            std::sort(usedUnits.begin(), usedUnits.end());
        }
        int nUnits = 0;

        if (useVisibleUnits)
            nUnits = m_visibleUnits.size();
        else
            nUnits = usedUnits.size();

        // the number of units shrunk, remove extra widgets
        while (nUnits < m_checkboxes.size())
        {
            UnitCheckBox *check = m_checkboxes.last();
            m_checkboxes.remove(m_checkboxes.indexOf(check));
            m_layout->removeWidget(check);
            check->deleteLater();
        }
        int j = 0;
        // update the units
        for (int i = 0; i < usedUnits.size(); ++i)
        {
            UnitData *unitData = unitAccum->unit(usedUnits[i]);

            if (unitData != 0)
            {
                int u = unitData->unitNumber;
                QColor color = getColor(u);

                if (useVisibleUnits && !m_visibleUnits.contains(u))
                    continue;

                if (j < m_checkboxes.size())
                {
                    UnitCheckBox *check = m_checkboxes.at(j);
                    check->setColor(color);
                    check->setUnit(u);
                    check->setChecked(m_checkedUnits.contains(u));
                    check->update();
                }
                else
                {
                    UnitCheckBox *check = new UnitCheckBox(this, u, color);
                    connect(check, SIGNAL(clicked(bool)), this, SLOT(slotTriggered(bool)));
                    m_checkboxes.push_back(check);
                    check->setChecked(m_checkedUnits.contains(u));

                    m_layout->addWidget(check, 0);
                }
                j++;
            }
        }
    }
}

QColor unitSelection::getColor(int unit)
{
    QColor color(Qt::red);

    if ((unit != INVALID_WF) && (unit < m_unitColors.size()))
       color = m_unitColors.at(unit);

    return color;
}

// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose:  this slot is called when the user clicks one of the unit checkbox buttons
//           update the model.

void unitSelection::slotTriggered(bool checkState)
{
    m_checkedUnits.clear();
    m_checkedUnits = getCheckedUnits();
    updateModel();
}

// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose:  this slot is called when the user edits one of the units in another window
//           update the display and the model.

void unitSelection::slotChannelChanged(int channel)
{
    slotUnitsChanged(channel);
}

// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose:  this slot is called when the average units for the file changed
//           update the display.

void unitSelection::slotUnitsChanged(int channel)
{
    setUnits(m_useVisibleUnits);
}

// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose:  this slot is called when automatic sorting is finished
//           update the display and the model, and check and show all units

void unitSelection::slotChannelStateChanged(int channel, ChannelState state)
{
    if (state == CHANNELSTATE_INACTIVE)
    {
        setUnits(false);
        updateModel();
    }
}

// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose:  this slot is only connected if useVisibleUnits is set
//           when the visibility of a unit changes, update the display and the model
void unitSelection::slotVisibleUnitsChanged(int channel, QStringList visibleUnits)
{
    m_visibleUnits.clear();

    foreach (QString s, visibleUnits)
        m_visibleUnits.push_back(s.toInt());

    setUnits(m_useVisibleUnits);
    updateModel();
}

////////////////////////////////////////////////////////////////////////////////////////
// Q_PROPERTY methods - for interaction with the model

// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose:  The model uses this slot to get the current selection string list

QStringList unitSelection::currentSelection() const
{
    // build the string list
    QStringList selection;

    foreach (UnitCheckBox *cb, m_checkboxes)
    {
        if (cb->isChecked())
            selection.push_back(QString::number(cb->unit()));
    }
    return selection;
}

// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose:  If the selection state has changed, emit the selectionChanged signal for the model

void unitSelection::updateModel()
{
    // build the string list
    QStringList selection;

    foreach (UnitCheckBox *cb, m_checkboxes)
    {
        if (cb->isChecked())
        {
            selection.push_back(QString::number(cb->unit()));
        }
    }
    if (m_selection != selection)
    {
        m_selection = selection;
        emit selectionChanged(selection);
    }
}

// Author & Date:       Holly Ausbeck         9 Jan 2015
// Purpose: The model uses this to update the widget
//          Update the widget, but don't reflect the change back to the model
// Inputs:
//          selection - list of selected units for the current channel
void unitSelection::setCurrentSelection(QStringList selection)
{
    setSelection(selection);
    setUnits(m_useVisibleUnits);
}

// Author & Date:       Holly Ausbeck        4 Feb 2016
// Purpose: this function insures m_selection matches m_checkedUnits
void unitSelection::setSelection(QStringList selection)
{
    m_selection = selection;
    m_checkedUnits.clear();

    foreach(QString sel, selection)
    {
        m_checkedUnits.push_back(sel.toInt());
    }
}


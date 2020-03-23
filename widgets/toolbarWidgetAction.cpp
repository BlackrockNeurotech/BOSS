#include "toolbarWidgetAction.h"
#include "../model/configmodel.h"


ToolbarWidgetAction::ToolbarWidgetAction(QObject *parent, QString label, ActionWidgetType type) :
QWidgetAction(parent)
{
    m_label = label;
    m_image = "";

    m_color = ConfigModel::ColorGradientLight;
    m_colorDark = ConfigModel::ColorGradientDark;
    m_colorHover = ConfigModel::ColorGradientLightHover;
    m_colorHoverDark = ConfigModel::ColorGradientDarkHover;

    m_selection = "";
    m_styleSheet = "";
    m_tooltip = "";
    m_type = type;
    m_widget = 0;
}

ToolbarWidgetAction::ToolbarWidgetAction(QObject *parent, QStringList items, QString selection, ActionWidgetType type) :
QWidgetAction(parent)
{
    m_items = items;
    m_image = "";

    m_color = ConfigModel::ColorGradientLight;
    m_colorDark = ConfigModel::ColorGradientDark;
    m_colorHover = ConfigModel::ColorGradientLightHover;
    m_colorHoverDark = ConfigModel::ColorGradientDarkHover;

    m_selection = selection;
    m_styleSheet = "";
    m_tooltip = "";
    m_type = type;
    m_widget = 0;
}

void ToolbarWidgetAction::setImage(QString icon)
{
    m_image = icon;
}

void ToolbarWidgetAction::setStyleSheet(QString styleSheet)
{
    m_styleSheet = styleSheet;

    //    // if the widget has already been created, set the style sheet for it
    //    QWidget *widget = this->requestWidget(this->parentWidget());

    if (m_widget != 0)
        m_widget->setStyleSheet(m_styleSheet);
}

void ToolbarWidgetAction::setToolTip(QString tooltip)
{
    m_tooltip = tooltip;

    //    // if the widget has already been created, set the style sheet for it
    //    QWidget *widget = this->requestWidget(this->parentWidget());

    if (m_widget != 0)
        m_widget->setToolTip(m_tooltip);
}

QWidget* ToolbarWidgetAction::createWidget(QWidget * parent)
{
    QWidget *w;

    switch (m_type)
    {
    case ACTIONWIDGETTYPE_LABEL:
        w = new QLabel(m_label, parent);
        break;
    case ACTIONWIDGETTYPE_VERTICAL_LABEL:
        w = new VerticalLabel(m_label, parent);
        break;
    case ACTIONWIDGETTYPE_CHECKBOX:
        w = new QCheckBox(m_label, parent);
        this->setCheckable(true);
        connect(w, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
        QObject::connect(w, SIGNAL(pressed()), this, SLOT(slotPressed()));
        break;
    case ACTIONWIDGETTYPE_COMBOBOX:
        {
            QComboBox *cb = new QComboBox(parent);
            cb->addItems(m_items);
            cb->setCurrentText(m_selection);
            connect(cb, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotCurrentIndexChanged(QString)));
            w = cb;
        }
        break;

    case ACTIONWIDGETTYPE_BUTTON:
        {
            QPushButton *pb = new QPushButton(m_label, parent);
            QObject::connect(pb, SIGNAL(pressed()), this, SLOT(slotPressed()));

            if (m_image.size() != 0)
            {
                pb->setIcon(QIcon(m_image));
            }
            w = pb;
        }
        break;
    case ACTIONWIDGETTYPE_BLACKROCKRADIOBUTTON:
    case ACTIONWIDGETTYPE_BLACKROCKBUTTON:
        BlackrockButton *pb = new BlackrockButton(parent, m_label, m_color);
        QObject::connect(pb, SIGNAL(pressed()), this, SLOT(slotPressed()));

        if (m_type == ACTIONWIDGETTYPE_BLACKROCKRADIOBUTTON)
            pb->setRadio(true);
        if (m_type == ACTIONWIDGETTYPE_BLACKROCKBUTTON)
        {
            pb->setAlwaysSelected();
        }

        if (m_image.size() != 0)
        {
            pb->setImage(m_image);
            pb->setDimensionsToIcon();
        }
        w = pb;
        break;
    }
    if (m_styleSheet.size() != 0)
    {
        w->setStyleSheet(m_styleSheet);
    }
    if (m_tooltip.size() != 0)
    {
        w->setToolTip(m_tooltip);
    }
    m_widget = w;
    w->show();
    return w;
}

void ToolbarWidgetAction::deleteWidget(QWidget * widget)
{
    widget->hide();
    widget->deleteLater();
    m_widget = 0;
}

void ToolbarWidgetAction::slotPressed()
{
    emit pressed();
}

void ToolbarWidgetAction::slotCurrentIndexChanged(QString sText)
{
    emit currentIndexChanged(sText);
}



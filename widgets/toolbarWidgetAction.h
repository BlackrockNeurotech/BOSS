#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QWidgetAction>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"
#include "../widgets/verticallabel.h"
#include "../widgets/BlackrockButton.h"


typedef enum ActionWidgetType_
{
    ACTIONWIDGETTYPE_BUTTON,
    ACTIONWIDGETTYPE_BLACKROCKBUTTON,
    ACTIONWIDGETTYPE_BLACKROCKRADIOBUTTON,
    ACTIONWIDGETTYPE_LABEL,
    ACTIONWIDGETTYPE_CHECKBOX,
    ACTIONWIDGETTYPE_VERTICAL_LABEL,
    ACTIONWIDGETTYPE_COMBOBOX

} ActionWidgetType;

class ToolbarWidgetAction : public QWidgetAction
{
    Q_OBJECT

public:
    ToolbarWidgetAction(QObject *parent, QString label, ActionWidgetType type = ACTIONWIDGETTYPE_BUTTON);
    ToolbarWidgetAction(QObject *parent, QStringList items, QString selection, ActionWidgetType type = ACTIONWIDGETTYPE_COMBOBOX);

    void setColor(QColor color) { m_color = color; }
    void setImage(QString image);
    void setStyleSheet(QString styleSheet);
    void setToolTip(QString tooltip);
    QWidget * getWidget() { return m_widget;}

public slots:
    void slotPressed();
    void slotCurrentIndexChanged(QString);

signals:
    void pressed();
    void currentIndexChanged(QString);

protected:
    virtual QWidget* createWidget (QWidget * parent);
    virtual void deleteWidget (QWidget * widget);

private:
    QString m_label;
    QString m_image;
    QStringList m_items;
    QString m_selection;

    QColor m_color;
    QColor m_colorDark;
    QColor m_colorHover;
    QColor m_colorHoverDark;

    QString m_styleSheet;
    QString m_tooltip;
    ActionWidgetType m_type;
    QWidget *m_widget;
};


#endif // TOOLBARWIDGETACTION_H

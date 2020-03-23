#ifndef CHECKRADIOGROUP_H
#define CHECKRADIOGROUP_H

#include <QWidget>
#include <QMap>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"

class QRadioButton;
class QCheckBox;

class CheckRadioGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER false)
public:
    CheckRadioGroup(QWidget *parent, ChannelColumn channelColumn);
    ~CheckRadioGroup();

    // interfact to QDataWidgetMapper
    QString currentSelection() const;
    void setCurrentSelection(QString selection);
    void submit();

    // initialization
    void addRadioButton(QRadioButton *button, QString selection);
    void setCheckBox(QCheckBox *button, QString checked, QString unchecked);

signals:
    void selectionChanged(QString selection);

public slots:
    void slotChannelChanged(int channel);
    void slotClicked(bool checked);
    void slotGroupClicked(bool checked);

private:
    QDataWidgetMapper *m_mapper;

    QString m_currentSelection;
    QString m_defaultSelection;
    QString m_noSelection;
    QMap<QString, QRadioButton*> m_buttonMap;
    QCheckBox *m_checkbox;

    void setChannelsDiffer();
    void clearRadioButtons();
    void enableRadioButtons(bool enable);
    QString enableGroup(bool enable);
};

#endif // CHECKRADIOGROUP_H

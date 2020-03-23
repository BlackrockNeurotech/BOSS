#ifndef RADIOGROUP_H
#define RADIOGROUP_H

#include <QWidget>
#include <QMap>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"

class QRadioButton;

class RadioGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER true)
public:
    RadioGroup(QWidget *parent, ChannelColumn channelColumn);
    ~RadioGroup();

    QString currentSelection() const;
    void setCurrentSelection(QString selection);
    void addRadioButton(QRadioButton *button, QString selection);
    void enableOne(QRadioButton *button, bool bEnable);
    void submit();

    QDataWidgetMapper *m_mapper;

signals:
    void selectionChanged(QString selection);

public slots:
    // configuration slots
    void slotChannelChanged(int channel);
    // radio button slots
    void slotClicked(bool checked);

private:
    QString m_currentSelection;
    QString m_noSelection;
    QMap<QString, QRadioButton*> m_buttonMap;

    void clearRadioButtons();
    void setChannelsDiffer();
};

#endif // RADIOGROUP_H

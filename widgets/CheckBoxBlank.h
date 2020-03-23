#ifndef CHECKBOXBLANK_H
#define CHECKBOXBLANK_H

#include <QCheckBox>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"


class CheckBoxBlank : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(QString currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER true)

public:
    CheckBoxBlank(QWidget* parent = 0);
    ~CheckBoxBlank();

    void setChannelColumn(int column);

    QString currentSelection() const;
    void setCurrentSelection(QString selection);
    void submit();

    QDataWidgetMapper *m_mapper;

signals:
    void selectionChanged(QString selection);

public slots:
    // configuration slots
    void slotChannelChanged(int channel);
    // CheckBox slot
    void slotValueChanged(int value);

private:
    QString m_currentSelection;
};


#endif // CHECKBOXBLANK_H

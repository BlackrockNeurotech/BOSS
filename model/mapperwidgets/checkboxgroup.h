#ifndef CHECKBOXGROUP_H
#define CHECKBOXGROUP_H

#include <QWidget>
#include <QMap>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"

class QCheckBox;

class CheckBoxGroup : public QWidget
{
    Q_OBJECT
        Q_PROPERTY(QList<QVariant> currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER true)
public:
    CheckBoxGroup(QWidget *parent, ChannelColumn channelColumn);
    ~CheckBoxGroup();

    QList<QVariant> currentSelection() const;
    void setCurrentSelection(QList<QVariant> selection);
    void submit();

     // initialization
     void setSelectionId(QCheckBox *button, uint selection);

signals:
     void selectionChanged(QList<QVariant> selection);

public slots:
    void slotChannelChanged(int channel);
    void slotClicked(bool checked);

private:
    QDataWidgetMapper *m_mapper;
    
    QList<QVariant> m_currentSelection;
    QMap<uint, QCheckBox*> m_buttonMap;
    QMap<QCheckBox*, uint> m_revButtonMap;

    void addSelection(uint selection);
    void removeSelection(uint selection);
    void setChannelsDiffer();
};

#endif // CHECKBOXGROUP_H

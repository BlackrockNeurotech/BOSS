#ifndef SPINBOXBLANK_H
#define SPINBOXBLANK_H

#include <QSpinBox>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"


class SpinBoxBlank : public QSpinBox
{
    Q_OBJECT
    Q_PROPERTY(QString currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER true)

public:
    SpinBoxBlank(QWidget* parent = 0);
    ~SpinBoxBlank();

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
    // spinbox slot
    void slotValueChanged(int value);
    void slotEditingFinished();

private:
    QString m_currentSelection;
};


#endif // SPINBOXBLANK_H

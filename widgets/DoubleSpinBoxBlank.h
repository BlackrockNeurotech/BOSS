#ifndef DOUBLESPINBOXBLANK_H
#define DOUBLESPINBOXBLANK_H

#include <QSpinBox>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"


class DoubleSpinBoxBlank : public QDoubleSpinBox
{
    Q_OBJECT
    Q_PROPERTY(QString currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER true)

public:
    DoubleSpinBoxBlank(QWidget* parent = 0);
    ~DoubleSpinBoxBlank();

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
    void slotValueChanged(const QString &text);
    void slotEditingFinished();

private:
    QString m_currentSelection;
};


#endif // DOUBLESPINBOXBLANK_H

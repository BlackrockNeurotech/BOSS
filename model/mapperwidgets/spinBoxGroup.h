#ifndef SPINBOXGROUP_H
#define SPINBOXGROUP_H

#include <QWidget>
#include <QMap>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"

class QAbstractSpinBox;


// Note: this data widget mapper class works best if all the spin boxes in the group are the same type.
// ie. All the spin boxes are either QDoubleSpinBox or QSpinBox

class SpinBoxFunction
{
public:
    enum functionType { fxnBase, fxnMultiply, fxnColumnMultiply, fxnPercent };

    SpinBoxFunction(functionType fxn, qreal op = 0, ChannelColumn channelColumn = CC_END, int precision = 0);

    QString getValue();
    bool setValue(QString sValue);
    QString getBaseValue();
    void setBaseValue(QString sValue);

private:
    functionType  m_fxn;
    qreal         m_multiplier;
    qreal         m_range;
    QString       m_sValue;
    ChannelColumn m_columnMultiplier;
    int           m_precision;
};


class SpinBoxGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER true)
public:
    SpinBoxGroup(QWidget *parent, ChannelColumn channelColumn);
    ~SpinBoxGroup();

     QString currentSelection() const;
     void setCurrentSelection(QString selection);
     void addSpinBox(QAbstractSpinBox *spinBox, SpinBoxFunction::functionType fxn, qreal op = 0, ChannelColumn channelColumn = CC_END);
     void enable(bool bEnable);
     void submit();
     void updateOnEditingFinished(bool bUpdateOnEditingFinished);

     QDataWidgetMapper *m_mapper;
 
protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void selectionChanged(QString selection);
    void editingFinished(QString selection);

public slots:
    // configuration slots
    void slotChannelChanged(int channel);
    // spin box slots
    void slotEditingFinished();
    void slotValueChanged(int value);
    void slotValueChanged(double value);

private:
     QString  m_currentSelection;
     QMap<QAbstractSpinBox*, SpinBoxFunction*> m_spinMap;
     bool m_bUpdateOnEditingFinished;

     void setValue(QString value, QAbstractSpinBox* sender = NULL);
     bool updateSpinBoxes(QAbstractSpinBox* spinBox);
};

#endif // SPINBOXGROUP_H

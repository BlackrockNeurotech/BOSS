#ifndef UNITSELECTION_H
#define UNITSELECTION_H

#include <QLabel>
#include <QStringList>
#include <QPainter>
#include <QPushButton>
#include <QHBoxLayout>
#include <armadillo>

#include "../model/channelconfigmodel.h"
#include "../model/configmodel.h"
#include "../widgets/uniteditmenu.h"


class UnitCheckBox : public QPushButton
{
    Q_OBJECT

public:
    UnitCheckBox(QWidget *parent, int unit, QColor color);
    ~UnitCheckBox();

    void setColor(QColor color);
    void setUnit(int unit) { m_unit = unit; }
    int unit() { return m_unit; }

protected:
    virtual void paintEvent (QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    int    m_unit;
    QColor m_color;
    QColor m_colorDark;
    QColor m_colorLight;
};


class unitSelection : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QStringList currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER true)

public:
    unitSelection(QWidget *parent = 0, bool useVisibleUnits = false, bool mainWidget = true);

    QStringList currentSelection() const;
    void setCurrentSelection(QStringList selection);
    bool usesVisibleUnits() { return m_useVisibleUnits; }

signals:
     void selectionChanged(QStringList selection);

public slots:
    void slotTriggered(bool checkState);
    void slotVisibleUnitsChanged(int channel, QStringList visibleUnits);
    void slotUnitsChanged(int channel);
    void slotChannelStateChanged(int channel, ChannelState state);
    void slotChannelChanged(int channel);

private:
    bool m_mainWidget;

    QVector<int>           m_visibleUnits;
    bool                   m_useVisibleUnits;

    QHBoxLayout           *m_layout;
    QVector<UnitCheckBox*> m_checkboxes;

    QList<QColor>          m_unitColors;
    QVector<int>           m_checkedUnits;
    QStringList            m_selection;

    void clearUnits();
    void setUnits(bool useVisibleUnits);
    QVector<int> getCheckedUnits();

    QColor getColor(int unit);
    void updateModel();
    void setSelection(QStringList selection);
};


#endif // UNITSELECTION_H

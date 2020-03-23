#ifndef POINTSIZESLIDER_H
#define POINTSIZESLIDER_H

#include <QSlider>
#include <QLabel>
#include <QPainter>

#include "../model/channelconfigmodel.h"
#include "multipleChoiceSlider.h"

class PointSizeSlider : public MultipleChoiceSlider
{
    Q_OBJECT
    Q_PROPERTY(int currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER true)

public:
    PointSizeSlider(QWidget *parent = 0);
    ~PointSizeSlider();
    void setOptions();

    int currentSelection() const;
    void setCurrentSelection(int selection);

signals:
    void selectionChanged(int selection);

public slots:
    void slotValueChanged(int value);

protected:
    void paintEvent(QPaintEvent *event);

private:
    // model variables
    int  currentSelection_;
    QList<QImage> m_images;

};


#endif // POINTSIZESLIDER_H

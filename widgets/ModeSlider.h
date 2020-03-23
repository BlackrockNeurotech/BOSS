#ifndef MODESLIDER_H
#define MODESLIDER_H

#include <QSlider>
#include <QLabel>
#include <QPainter>

#include "../model/channelconfigmodel.h"
#include "multipleChoiceSlider.h"

class ModeSlider : public MultipleChoiceSlider
{
    Q_OBJECT
    Q_PROPERTY(QString currentSelection READ currentSelection WRITE setCurrentSelection NOTIFY selectionChanged USER true)

public:
    ModeSlider(QWidget *parent = 0);
    ~ModeSlider();
    void setOptions(QStringList options, QStringList tooltips, QStringList bitmaps);

    QString currentSelection() const;
    void setCurrentSelection(QString selection);

signals:
    void selectionChanged(QString selection);

public slots:
    void slotValueChanged(int value);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QStringList m_options;
    QStringList m_bitmaps;
    QList<int>  m_optionSize;
    QList<QImage> m_images;

	void drawText(QPainter *painter, QRect r, QString s, int size, bool enabled = true);

    // model variables
    QString     currentSelection_;

};


#endif // MODESLIDER_H

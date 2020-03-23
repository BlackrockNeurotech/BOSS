#ifndef BLACKROCKBUTTON_H
#define BLACKROCKBUTTON_H

#include <QPushButton>

class BlackrockButton : public QPushButton
{
    Q_OBJECT

public:
    BlackrockButton(QWidget *parent, QString text = "", QColor color = QColor(Qt::lightGray).lighter(), QColor checked = QColor("#009FE3").lighter(), QColor disabled = QColor(Qt::gray));
    ~BlackrockButton();

    void setRadio(bool bRadio) { m_bRadio = bRadio; }  // radio means it can't be clicked on when checked
    void setInvert(bool bInvert) { m_bInvert = bInvert; }  // invert, means invert the gradient when checked
    void setAlwaysSelected();
    void setImage(QString path);

    void setDimensionsToIcon();
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QColor m_color;
    QColor m_colorDark;
    QColor m_colorHover;
    QColor m_colorHoverDark;

    QColor m_checkedColor;
    QColor m_checkedColorDark;
    QColor m_checkedColorHover;
    QColor m_checkedColorHoverDark;

    QColor m_disabledColor;

    QImage *m_image;
    bool    m_bRadio;
    bool    m_bInvert;

    int m_width;
    int m_height;

    QLinearGradient radioGradient(bool mouseOver, int height);
    QLinearGradient checkableGradient(bool mouseOver, int height);
    QLinearGradient invertGradient(bool mouseOver, int height);
    QLinearGradient regularGradient(bool mouseOver, int height);
};

#endif // BLACKROCKBUTTON_H

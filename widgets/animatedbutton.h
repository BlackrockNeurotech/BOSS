#ifndef ANIMATEDBUTTON_H
#define ANIMATEDBUTTON_H
#include <QPushButton>
#include <QColor>
#include <QPainter>
#include <QTimer>

class AnimatedButton : public QPushButton
{
    Q_OBJECT
public:
    AnimatedButton(QWidget *parent, QString text = QString());
    ~AnimatedButton();
    void setColor(QColor color);
    void startAnimation( ) { m_timer.start();}
    void stopAnimation( ) { m_timer.stop();}
    void reset();

private:
    QColor m_color;
    QColor m_colorDark;
    QColor m_colorHover;
    QColor m_colorHoverDark;

    QColor m_blinkColor;
    QColor m_blinkColorDark;
    QColor m_blinkColorHover;
    QColor m_blinkColorHoverDark;

    QColor m_tempColor;
    QColor m_tempColorDark;
    QColor m_tempColorHover;
    QColor m_tempColorHoverDark;


    QTimer m_timer;
    bool   m_bStatus;


protected:
    virtual void paintEvent (QPaintEvent *event);

private slots:
    void toggleColor();
};

#endif // ANIMATEDBUTTON_H

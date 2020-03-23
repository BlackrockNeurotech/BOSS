#ifndef GROUPBUTTON_H
#define GROUPBUTTON_H

#include <QPushButton>
#include <QImage>

class GroupButton : public QPushButton
{
    Q_OBJECT

public:
    GroupButton(QWidget *parent, int nGroup, QList<int> nChannels, QString text = QString(""));
    ~GroupButton();

    void setSelected(bool bSelected);
    void setDisplayed(bool bDisplayed);
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void	mouseMoveEvent(QMouseEvent *event);
private:
    QColor m_color;
    bool m_bSelected;
    bool m_bDisplayed;

    QImage *m_drawImage;
    QString m_text;

    QList<int> m_nChannels;
    int m_nGroup;

public slots:
    void slotClicked(bool);

signals:
    void selected(int nGroup);
};

#endif // GROUPBUTTON_H

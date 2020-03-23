#ifndef PROGRESSBUTTON_H
#define PROGRESSBUTTON_H

#include <QPushButton>
#include <QImage>

class ProgressButton : public QPushButton
{
    Q_OBJECT

public:
    ProgressButton(QWidget *parent, int nChannel, QString text = QString(""), bool cancel = true);
    ~ProgressButton();

    void setProgress(int percent);
    void showText(bool bShow);
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void	mouseMoveEvent(QMouseEvent *event);
private:
    QColor m_color;
    int m_percent;
    int m_nChannel;
    bool m_cancel;
    bool m_bShowText;

    QImage *m_imageCancel;
    QImage *m_imageCancelHover;
    QImage *m_drawImage;
    QString m_text;

public slots:
    void slotCancel(bool);

signals:
    void cancelProgress(int nChannel);
};

#endif // PROGRESSBUTTON_H

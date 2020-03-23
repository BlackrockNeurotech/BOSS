#ifndef MENUITEM_H
#define MENUITEM_H

#include <QWidgetAction>
#include <QPushButton>

class ActionItem : public QWidgetAction
{
    Q_OBJECT

public:
    ActionItem(QObject *parent, int menuIdx, QString name, QString text, QColor color);
    ~ActionItem();
    void setIdx(int idx) { m_idx = idx; }

protected:
    virtual QWidget* createWidget(QWidget *parent);

private:
    int     m_idx;
    QColor  m_color;
    QString m_text;
    QString m_name;

    public slots:
    void slotTriggered(bool checked);

signals:
    void triggered(bool checked, int idx, QString name);


};

class MenuPushButton : public QPushButton
{
    Q_OBJECT

public:
    MenuPushButton(QString text, QColor color, QWidget *parent = 0);
    ~MenuPushButton();

    void setColor(QColor color);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QColor m_color;
    QColor m_colorDark;
    QColor m_colorHover;
    QColor m_colorHoverDark;
};


#endif // MENUITEM_H
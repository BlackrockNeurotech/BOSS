#ifndef DOCKWIDGET_TITLEBAR_H
#define DOCKWIDGET_TITLEBAR_H

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QDockWidget>
#include <QStringList>


class DockWidgetTitleBar : public QWidget
{
    Q_OBJECT

public:
    DockWidgetTitleBar(QDockWidget *parent, bool bAutohide = false);
    virtual ~DockWidgetTitleBar();

    void setWindowTitle(const QString &title);
    void setDocked(bool dock);
    void setAutohide(bool autohide);

public slots:
    void slotDockClicked(bool state);
    void slotAutohideClicked(bool state);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event); 

signals:
    void dock();
    void autohide();

private:
    QLabel      *m_title;
    QToolButton *m_dockfloatToolButton;
    QToolButton *m_autohideToolButton;
    QToolButton *m_closeToolButton;

    bool m_bAutohide;
};


#endif // DOCKWIDGET_TITLEBAR_H

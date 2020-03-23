#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QDockWidget>

#include "../widgets/DockWidgetTitleBar.h"


class DockWidget : public QDockWidget
{
    Q_OBJECT

public:
	DockWidget(QWidget *parent = 0, QString name = "", bool bAutohide = false);
    ~DockWidget();

    void  resetWindowTitle();
    QSize sizeHint() const { return this->size(); }

public slots :
    void slotToggleDock();
    void slotToggleAutoHide();
    void dockStatusChanged();

signals:
    void autohide();
    void dockVisibilityChanged(bool visible);

protected:
	virtual void hideEvent(QHideEvent * event);
	virtual void showEvent(QShowEvent * event);

private:
    DockWidgetTitleBar *m_titleBar;
    bool m_mouseDown;
};


#endif // DOCKWIDGET_H

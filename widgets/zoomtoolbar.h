#ifndef ZOOMTOOLBAR_H
#define ZOOMTOOLBAR_H

#include <QToolBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidgetAction>
#include <QLabel>
#include <QVector>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"
#include "../widgets/arrowbutton.h"

enum zoomType { Vinc, Vdec, Hinc, Hdec, Left, Right, FitToView };

class ZoomToolBar : public QToolBar
{
    Q_OBJECT

public:
    ZoomToolBar(QWidget *parent = 0);
    ~ZoomToolBar();

signals:
    void zoomChanged(zoomType type);

private slots:
    void slotVInc(bool value);
    void slotVDec(bool value);
    void slotHinc(bool value);
    void slotHDec(bool value);
    void slotLeft(bool value);
    void slotRight(bool value);
    void slotFitToView(bool value);

private:
    QWidget     *m_parent;

    QVBoxLayout *m_layout;
    ArrowButton *m_zoomInH;
    ArrowButton *m_zoomOutH;
    ArrowButton *m_zoomInV;
    ArrowButton *m_zoomOutV;
    ArrowButton *m_left;
    ArrowButton *m_right;
    ArrowButton *m_fitToView;

    int  m_nGeom;
    int  m_nHeight;
};


#endif // ZOOMTOOLBAR_H

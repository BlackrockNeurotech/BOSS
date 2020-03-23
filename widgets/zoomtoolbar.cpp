#include "zoomtoolbar.h"

ZoomToolBar::ZoomToolBar(QWidget *parent) :
 QToolBar(parent)
{
    this->setStyleSheet("QToolBar {border-bottom: none; border-top: none;}");
    this->setOrientation(Qt::Vertical);

    QWidget *w = new QWidget();
    m_layout = new QVBoxLayout();
    m_layout->setSpacing(1);
    m_layout->setMargin(4);
    m_layout->setSizeConstraint(QLayout::SetNoConstraint);

    // make the scroll bar the height of the font
    QFont myFont;
    QFontMetrics fm(myFont);
    QRectF f = fm.boundingRect("123");
    m_nHeight = f.height() * 4;
    m_nGeom = f.height() + 2;

    // create the +/- buttons and set the dimensions
    m_zoomInV = new ArrowButton(this, ArrowButton::vzoomin, m_nGeom);
    m_zoomOutV = new ArrowButton(this, ArrowButton::vzoomout, m_nGeom);
    m_zoomInH = new ArrowButton(this, ArrowButton::hzoomin, m_nGeom);
    m_zoomOutH = new ArrowButton(this, ArrowButton::hzoomout, m_nGeom);
    m_fitToView = new ArrowButton(this, ArrowButton::fittoview, m_nGeom);

    m_zoomInV->setMaximumSize(m_nGeom, m_nGeom);
    m_zoomOutV->setMaximumSize(m_nGeom, m_nGeom);
    m_zoomInH->setMaximumSize(m_nGeom, m_nGeom);
    m_zoomOutH->setMaximumSize(m_nGeom, m_nGeom);
    m_fitToView->setMaximumSize(m_nGeom, m_nGeom);

    m_zoomInV->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_zoomOutV->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_zoomInH->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_zoomOutH->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_fitToView->setAttribute(Qt::WA_LayoutUsesWidgetRect);

    m_zoomInV->setToolTip(QString("Zoom in Vertically"));
    m_zoomOutV->setToolTip(QString("Zoom out Vertically"));
    m_zoomInH->setToolTip(QString("Zoom in Horizontally"));
    m_zoomOutH->setToolTip(QString("Zoom out Horizontally"));
    m_fitToView->setToolTip(QString("Fit in View"));

    // add the widgets to the layout
    m_layout->addWidget(m_zoomInV, 0, Qt::AlignVCenter | Qt::AlignLeft);
    m_layout->addSpacing(4);
    m_layout->addWidget(m_zoomOutV, 0, Qt::AlignVCenter | Qt::AlignRight);
    m_layout->addSpacing(4);
    m_layout->addWidget(m_zoomInH, 0, Qt::AlignVCenter | Qt::AlignLeft);
    m_layout->addSpacing(4);
    m_layout->addWidget(m_zoomOutH, 0, Qt::AlignVCenter | Qt::AlignRight);
    m_layout->addSpacing(4);
/*    m_layout->addWidget(m_left, 0, Qt::AlignVCenter | Qt::AlignRight);
    m_layout->addSpacing(4);
    m_layout->addWidget(m_right, 0, Qt::AlignVCenter | Qt::AlignRight);
    m_layout->addSpacing(4);
 */   m_layout->addWidget(m_fitToView, 0, Qt::AlignVCenter | Qt::AlignRight);

    w->setLayout(m_layout);
    this->addWidget(w);

#ifndef OSMAC
    this->setStyleSheet("background-color: black;");
#endif
    this->setContentsMargins(0, 0, 0, 0);
    this->setMinimumHeight(30);

    QObject::connect(m_zoomInH, SIGNAL(clicked(bool)), this, SLOT(slotHinc(bool)));
    QObject::connect(m_zoomOutH, SIGNAL(clicked(bool)), this, SLOT(slotHDec(bool)));
    QObject::connect(m_zoomInV, SIGNAL(clicked(bool)), this, SLOT(slotVInc(bool)));
    QObject::connect(m_zoomOutV, SIGNAL(clicked(bool)), this, SLOT(slotVDec(bool)));
    //QObject::connect(m_left, SIGNAL(clicked(bool)), this, SLOT(slotLeft(bool)));
    //QObject::connect(m_right, SIGNAL(clicked(bool)), this, SLOT(slotRight(bool)));
    QObject::connect(m_fitToView, SIGNAL(clicked(bool)), this, SLOT(slotFitToView(bool)));
}

ZoomToolBar::~ZoomToolBar()
{
}

void ZoomToolBar::slotVInc(bool value)
{
    emit zoomChanged(Vinc);
}

void ZoomToolBar::slotVDec(bool value)
{
    emit zoomChanged(Vdec);
}

void ZoomToolBar::slotHinc(bool value)
{
    emit zoomChanged(Hinc);
}

void ZoomToolBar::slotHDec(bool value)
{
    emit zoomChanged(Hdec);
}

void ZoomToolBar::slotLeft(bool value)
{
    emit zoomChanged(Left);
}

void ZoomToolBar::slotRight(bool value)
{
    emit zoomChanged(Right);
}

void ZoomToolBar::slotFitToView(bool value)
{
    emit zoomChanged(FitToView);
}


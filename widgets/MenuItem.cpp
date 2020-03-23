#include "MenuItem.h"
#include <QPainter>

ActionItem::ActionItem(QObject * parent, int menuIdx, QString text, QString name, QColor color) :
QWidgetAction(parent),
m_idx(menuIdx)
{
    m_text = text;
    m_color = color;
    m_name = name;
}

ActionItem::~ActionItem()
{

}

QWidget* ActionItem::createWidget(QWidget * parent)
{
    MenuPushButton *w = new MenuPushButton(m_text, m_color, parent);
    w->setColor(m_color);
    QObject::connect(w, SIGNAL(clicked(bool)), this, SLOT(slotTriggered(bool)));

    w->show();
    return w;
}

void ActionItem::slotTriggered(bool checked)
{
    emit triggered(checked, m_idx, m_name);
}


// button
MenuPushButton::MenuPushButton(QString text, QColor color, QWidget *parent) :
QPushButton(text, parent)
{
    setColor(color);
    // without Mouse Tracking the delete submenu won't pop up on mouse over
    setMouseTracking(true);
}

void MenuPushButton::setColor(QColor color)
{
    m_color = color;
    m_colorDark = m_color.darker();

    m_colorHover = m_color.lighter();
    m_colorHoverDark = color;
}

void MenuPushButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QRect r(this->geometry());
    r.moveTo(0, 0);
    QLinearGradient gradient(0, 0, 0, r.height());
    gradient.setColorAt(0, m_color);
    gradient.setColorAt(1, m_colorDark);

    bool mouseOver = false;
    QPoint globalPos = QCursor::pos();
    QPoint pos = this->mapFromGlobal(globalPos);

    if (r.contains(pos))
        mouseOver = true;

    if (this->isDown())
    {
        gradient.setColorAt(0, m_colorDark);
        gradient.setColorAt(1, m_color);
    }
    else if (mouseOver)
    {
        if (this->isChecked())
        {
            gradient.setColorAt(0, m_colorHoverDark);
            gradient.setColorAt(1, m_colorHover);
        }
        else
        {
            gradient.setColorAt(0, m_colorHover);
            gradient.setColorAt(1, m_colorHoverDark);
        }
    }
    painter.fillRect(r, gradient);
    painter.setPen(QPen(Qt::black));
    painter.drawRect(r);

    // draw the button text
    r.setLeft(5);
    painter.setPen(QPen(Qt::black));
    painter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, this->text());
}

MenuPushButton::~MenuPushButton()
{
}
#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QMouseEvent>
#include <QPixmap>

namespace Ui {
class WelcomePage;
}

class RecentLabel : public QLabel
{
    Q_OBJECT
public:
    explicit RecentLabel(QWidget *parent = 0) :
        QLabel(parent)
    {
        setTextFormat(Qt::RichText);
        setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    }
    ~RecentLabel(){}

    void setText(const QString & text);
    QString encodeWhiteSpaces(QString fn);

};

class LabelWithImage : public QLabel
{
    Q_OBJECT
public:
    LabelWithImage();
    ~LabelWithImage();
protected: 
    void resizeEvent(QResizeEvent *event);
private:
    QPixmap m_image;
};

class WelcomePage : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomePage(QWidget *parent = 0);
    ~WelcomePage();

    void addRecentFile(const QString &fn);
    void clearRecentFiles();

private:
    Ui::WelcomePage *ui;
    QVector<QLabel *> m_recentFiles;

    LabelWithImage m_image;
    
    QString decodeWhiteSpaces(QString fn);

public slots: 
void slotOpenLinkActivated(QString link);
void slotOpenMergeLinkActivated(QString link);
void slotRecentLinkActivated(QString link);

signals:

    void signalOpenLinkActivated();
    void signalOpenMergeLinkActivated();
    void signalRecentLinkActivated(QString link);
};

#endif // WELCOMEPAGE_H

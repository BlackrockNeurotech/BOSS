#include "welcomepage.h"
#include "ui_welcomepage.h"
#include <QVBoxLayout>
#include <qfiledialog.h>


void RecentLabel::setText(const QString & text)
{
    QString encoded = encodeWhiteSpaces(text);
    QString new_text = text;
    if (text.size() > 100)
    {
        new_text = "..." + text.right(97);
    }
    QString link("<a href = " + encoded + ">" + new_text + "< / a>");
    QLabel::setText(link);
    
}

QString RecentLabel::encodeWhiteSpaces(QString fn)
{
    QString modified = fn.replace(" ", "%20");
    return modified;

}

WelcomePage::WelcomePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WelcomePage)
{
    ui->setupUi(this);
    //this->layout()->addWidget(&m_image);
   
}

WelcomePage::~WelcomePage()
{
    clearRecentFiles();
    delete ui;
}


void WelcomePage::addRecentFile(const QString &fn)
{
    RecentLabel *lbl = new RecentLabel(this);
    lbl->setText(fn);
    connect(lbl, SIGNAL(linkActivated(QString)), this, SLOT(slotRecentLinkActivated(QString)));
    
    QVBoxLayout * layout = (QVBoxLayout *)ui->recentWidget->layout();
    layout->insertWidget(m_recentFiles.size()+1, lbl);
    m_recentFiles.append(lbl);
}

void WelcomePage::clearRecentFiles()
{
    for (int i(0); i < m_recentFiles.size(); ++i)
        if (m_recentFiles[i])
            delete m_recentFiles[i];

    m_recentFiles.clear();
}

QString WelcomePage::decodeWhiteSpaces(QString fn)
{
    QString modified = fn.replace("%20", " ");
    return modified;  

}



void WelcomePage::slotOpenLinkActivated(QString link)
{
    Q_UNUSED(link);
    emit signalOpenLinkActivated();
}

void WelcomePage::slotOpenMergeLinkActivated(QString link)
{
    Q_UNUSED(link);
    emit signalOpenMergeLinkActivated();
}

void WelcomePage::slotRecentLinkActivated(QString link)
{
#ifdef WIN64
    // This is a hack to ensure that some dlls are started before we load a file
    // Without it, if the user clicks on a recent file link, it takes more than 30 sec to read the xml
    // It only applies to windows, not OSx

    QFileDialog::Options opt = QFileDialog::DontUseNativeDialog;
    QString filters = "Blackrock files (*.nev *.ns*);;Spike files (*.nev);;Continuous data files (*.ns*)";
    QFileDialog dialog;
    dialog.setNameFilter(filters);
    dialog.setOptions(opt);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.showMinimized();

    dialog.close();
    // Hack finished
#endif

    QString new_link = decodeWhiteSpaces(link);
    emit signalRecentLinkActivated(new_link);
}


LabelWithImage::LabelWithImage()
{
    m_image.load(":/new/Bitmaps/Bitmaps/splashscreen.png");
    setPixmap(m_image);
    setMinimumSize(1, 1);
}

LabelWithImage::~LabelWithImage()
{

}

void LabelWithImage::resizeEvent(QResizeEvent *event)
{
    QSize s = event->size();
    QPixmap im = m_image.scaled(s, Qt::KeepAspectRatio);
    setPixmap(im);
    QLabel::resizeEvent(event);
}
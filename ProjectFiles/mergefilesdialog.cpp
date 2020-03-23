#include "mergefilesdialog.h"
#include "ui_mergefilesdialog.h"

MergeFilesDialog::MergeFilesDialog(QString lastPath, QWidget *parent) :
QDialog(parent),
m_lastPath(lastPath),
ui(new Ui::MergeFilesDialog)
{
    ui->setupUi(this);
}

MergeFilesDialog::~MergeFilesDialog()
{
    delete ui;
}


void MergeFilesDialog::addFiles()
{
        QStringList list;
        QString path(m_lastPath);
        QFileDialog::Options opt = 0;
        QString filters = "Blackrock files (*.nev *.ns*);;Spike files (*.nev);;Continuous data files (*.ns*)";
    #ifdef WIN64
        filters += ";;Plexon files (*.pl2)";
        opt = QFileDialog::DontUseNativeDialog;
    #endif
        list = QFileDialog::getOpenFileNames(this,
            "Please select one or more files to add", path, filters, 0, opt);
    
        if (list.isEmpty() == false)
        {
            QFileInfo finfo(list[0]);
            QDir dir = finfo.dir();
            m_lastPath = dir.absolutePath();
            ui->fileList->addItems(list);
        }

}

void MergeFilesDialog::removeFiles()
{
    QList<QListWidgetItem *> selected = ui->fileList->selectedItems();

    for (int i(0); i < selected.size(); ++i)
    {
        int r = ui->fileList->row(selected[i]);
        ui->fileList->takeItem(r);
    }

}

void MergeFilesDialog::checkInputs()
{
    QString path = ui->groupName->text();
    if (isValidPath(path) == false)
    {
        QMessageBox::warning(this, "Invalid Session Name Path", "Please assign a valid name path to the group of selected files.");
        return;
    }
    

    m_name = ui->groupName->text();
    m_files.clear();
    for (int i(0); i < ui->fileList->count(); ++i)
        m_files.append(ui->fileList->item(i)->text());

    if ((m_files.size() > 1) && !sameExtensions())
    {
        m_files.clear();
        QMessageBox::warning(this, "Extension mismatch", "The selected files have different file extensions and cannot be concatenated. Please review the list of selected files and try again.");
        return;
    }
    
    if (m_files.size() > 0)
        QDialog::accept();
    else
        reject();
}


void MergeFilesDialog::reject()
{
    m_name.clear();
    m_files.clear();
    ui->fileList->clear();
    QDialog::reject();
}

void MergeFilesDialog::browseSessionName()
{
    QFileDialog::Options opt = 0;
#ifdef WIN64
    opt = QFileDialog::DontUseNativeDialog;
#endif
    QString session = QFileDialog::getSaveFileName(this, tr("Select Concatenated File Session Location"),
        m_lastPath,
        tr("Concatenated File Sessions (*.cfs)"),
        (QString *)0,
        opt);     // .cfs for "Concatenated File Sessions"

    if (!session.isEmpty())
    {
        QString base = session.section(".", 0, 0);
        session = base + ".cfs";
    }

    ui->groupName->setText(session);

    if (!session.isEmpty())
    {
        QFileInfo finfo(session);
        QDir dir = finfo.dir();
        m_lastPath = dir.absolutePath();
    }

}

void MergeFilesDialog::getExtension(QString &ext)
{
    ext.clear();
    if (m_files.size() > 0)
    {
        QFileInfo finfo(m_files[0]);
        ext = finfo.suffix();
    }
}

bool MergeFilesDialog::isValidPath(QString & path)
{
    bool res = false; 
    if (path.isEmpty())
        return res;
    
    QFileInfo finfo(path);
    QDir dir = finfo.dir();
    QString fn = finfo.fileName();
    QString fpath = finfo.filePath();
    if ((fn != fpath) && 
        (dir.dirName() != ".") &&
        !dir.dirName().isEmpty() &&
        dir.exists() &&
        !fn.isEmpty())
        res = true;

    return res;
}

bool MergeFilesDialog::sameExtensions()
{
    bool res = true;
    if (m_files.isEmpty())
        return res;
    
    QFileInfo finfo(m_files[0]);
    QString targetExtension(finfo.suffix());

    for (int i(1); i < m_files.size(); ++i)
    {
        finfo.setFile(m_files[i]);
        QString   extension(finfo.suffix());
        if (targetExtension != extension)
        {
            res = false;
            break;
        }
    }

    return res;
}
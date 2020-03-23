#include "savemergedfilesdialog.h"
#include "ui_savemergedfilesdialog.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QAbstractButton>
#include <QFileDialog>
#include <QMessageBox>

SaveMergedFilesDialog::SaveMergedFilesDialog(QStringList & sources, QString & lastPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveMergedFilesDialog)
{
    ui->setupUi(this);

    m_lastPath = lastPath;

    QHeaderView* header = ui->filesTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    // Connect signals and slots
    connect(ui->buttonGroupExtension, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(slotUpdateExtensions(QAbstractButton *)));
    connect(ui->checkBoxOneOutputFile, SIGNAL(stateChanged(int)), this, SLOT(slotOneOutputFile(int)));
    connect(ui->checkBoxUseBaseFileName, SIGNAL(stateChanged(int)), this, SLOT(slotBaseFileNameChecked(int)));
    connect(ui->browsePB, SIGNAL(clicked()), this, SLOT(slotBrowseButtonClicked()));
    connect(ui->lineEditBaseFileName, SIGNAL(textChanged(const QString &)), this, SLOT(slotBaseFileNameChanged(const QString &)));
    connect(ui->filesTable, SIGNAL(cellChanged(int, int)), this, SLOT(slotUpdateExtensionsForCell(int, int)));


    // Fill source column and make it read only
    fillSources(sources);

    initDestinations();

}

SaveMergedFilesDialog::~SaveMergedFilesDialog()
{
    delete ui;
}

void SaveMergedFilesDialog::getDestinationNames(QStringList & destination)
{
    destination = m_destination;
}

void SaveMergedFilesDialog::fillSources(QStringList & sources)
{
    ui->filesTable->setRowCount(sources.size());
    
    ui->filesTable->blockSignals(true);
    for (int i(0); i < sources.size(); ++i)
    {
        QTableWidgetItem * item = new QTableWidgetItem(sources[i]);
        item->setFlags(item->flags() &  ~Qt::ItemIsEditable);
        ui->filesTable->setItem(i, 0, item);
    }
    ui->filesTable->blockSignals(false);
}

void SaveMergedFilesDialog::initDestinations()
{
    ui->filesTable->blockSignals(true);
    for (int i(0); i < ui->filesTable->rowCount(); ++i)
    {
        QTableWidgetItem * item = new QTableWidgetItem(QString());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->filesTable->setItem(i, 1, item);
    }
    ui->filesTable->blockSignals(false);
}


void SaveMergedFilesDialog::slotCheckInputs()
{
    // Check that the path is valid
    QString path = ui->pathLineEdit->text();
    if (path.isEmpty())
    {
        QMessageBox::warning(this, "Undefined saved location", "Please enter a valid path for the output files.");
        return;
    }

    // Check that the extension is set
    if (m_extension.isEmpty())
    {
        QMessageBox::warning(this, "Undefined extension", "Please select an extension for the output files.");
        return;
    }

    // Check that the path exists and offer to create it if not
    QDir dir(path);
    if (!dir.exists())
    {
        QMessageBox::StandardButton btn = QMessageBox::question(this, "Path does not exist", "The selected path does not exist. Would you like to create it?");
        if (btn == QMessageBox::Yes)        
            dir.mkpath(dir.path());        
        else
            return;
    }

    // Make sure the path ends with "/"
    if (!path.endsWith("/"))
        path += "/";

    // Check that all filenames are defined
    if (ui->checkBoxOneOutputFile->isChecked())
    {
        QString text = ui->filesTable->item(0, 1)->text();
        text = text.section(".", 0, 0);

        if (text.isEmpty())
        {
            QMessageBox::warning(this, "Invalid filename", "Please enter a valid filename for the output file.");
            return;
        }
    }
    else
    {
        QStringList list;
        for (int i(0); i < ui->filesTable->rowCount(); ++i)
        {
            QTableWidgetItem * item = ui->filesTable->item(i, 1);
            QString text = item->text().section(".", 0, 0);
            list << text;
            if (text.isEmpty())
            {
                QMessageBox::warning(this, "Invalid filename", "Please enter a valid filename for each of the output files.");
                return;
            }
        }

        // Check that there are no duplicates
        int removedEntries = list.removeDuplicates();
        if (removedEntries != 0)
        {
            QMessageBox::warning(this, "Invalid filename", "There are duplicated filenames in the list. Please choose different names for the outputs.");
            return;
        }

    }

    // Create destination string list (path + destinations)
    m_destination.clear();
    for (int i(0); i < ui->filesTable->rowCount(); ++i)
    {
        QTableWidgetItem * item = ui->filesTable->item(i, 1);
        QString text = item->text();
        m_destination << (path + text);
        if (ui->checkBoxOneOutputFile->isChecked())
            break;
    }

    QDialog::accept();
}

void SaveMergedFilesDialog::slotOneOutputFile(int state)
{
    // Clear all cells
    ui->filesTable->blockSignals(true);
    for (int i(0); i < ui->filesTable->rowCount(); ++i)
    {
        QTableWidgetItem * item = ui->filesTable->item(i, 1);        
        item->setText(QString());        
    } 

    if (state == Qt::Checked)
    { 
        ui->filesTable->setSpan(0, 1, ui->filesTable->rowCount(), 1);
        ui->checkBoxUseBaseFileName->blockSignals(true);
        ui->checkBoxUseBaseFileName->setCheckState(Qt::Unchecked);
        ui->checkBoxUseBaseFileName->blockSignals(false);
        ui->lineEditBaseFileName->blockSignals(true);
        ui->lineEditBaseFileName->clear();
        ui->lineEditBaseFileName->blockSignals(false);

    }
    else
    {
        ui->filesTable->setSpan(0, 1, 1, 1);
    }

    ui->filesTable->blockSignals(false);
}

void SaveMergedFilesDialog::slotBaseFileNameChecked(int state)
{
    ui->lineEditBaseFileName->setEnabled(state == Qt::Checked);

    if (state == Qt::Checked)
    {
        ui->checkBoxOneOutputFile->setCheckState(Qt::Unchecked);
    }
    else
    {
        ui->lineEditBaseFileName->clear();
        
        ui->filesTable->blockSignals(true);
        // Make destination filenames editable and clear their contents
        for (int i(0); i < ui->filesTable->rowCount(); ++i)
        {
            QTableWidgetItem * item = ui->filesTable->item(i, 1);
            item->setFlags(item->flags() |  Qt::ItemIsEditable);
            item->setText(QString());
        }
        ui->filesTable->blockSignals(false);
    }
}

void SaveMergedFilesDialog::slotBaseFileNameChanged(const QString & base)
{
    if (base.isEmpty())
        return;

    ui->filesTable->blockSignals(true);
    for (int i(0); i < ui->filesTable->rowCount(); ++i)
    {
        QString text = base + QString::number(i + 1) + "." + m_extension;
        QTableWidgetItem * item = ui->filesTable->item(i, 1);
        item->setText(text);
        item->setFlags(item->flags() &  ~Qt::ItemIsEditable);
    }
    ui->filesTable->blockSignals(false);
}

void SaveMergedFilesDialog::slotBrowseButtonClicked()
{
    ui->pathLineEdit->clear();

    QFileDialog::Options opt = QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;
#ifdef WIN64
    opt |= QFileDialog::DontUseNativeDialog;
#endif
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select a directory"),
        m_lastPath, opt);

    if (!dir.isEmpty())
    {
        m_lastPath = dir;
        ui->pathLineEdit->setText(dir);
    }
}

void SaveMergedFilesDialog::slotUpdateExtensions(QAbstractButton * button)
{
    m_extension = button->text().toLower();

    ui->filesTable->blockSignals(true);
    for (int i(0); i < ui->filesTable->rowCount(); ++i)
    {
        QTableWidgetItem * item = ui->filesTable->item(i, 1);
        QString text = item->text();
        if (!text.isEmpty())
        {
            text = text.section(".", 0, 0);
            text += "." + m_extension;
            Qt::ItemFlags flags = item->flags();            
            item->setFlags(flags | Qt::ItemIsEditable);
            item->setText(text);
            item->setFlags(flags);
            
        }
    }
    ui->filesTable->blockSignals(false);
}

void SaveMergedFilesDialog::slotUpdateExtensionsForCell(int row, int column)
{
    if (column != 1)
        return;

    QTableWidgetItem * item = ui->filesTable->item(row, 1);
    QString text = item->text();

    if (text.isEmpty() || m_extension.isEmpty())
        return;

    if (text.contains(".")) 
        text = text.section(".", 0, 0);

    text += "." + m_extension;
    Qt::ItemFlags flags = item->flags();
    ui->filesTable->blockSignals(true);
    item->setText(text);
    item->setFlags(flags | Qt::ItemIsEditable);
    ui->filesTable->blockSignals(false);
}
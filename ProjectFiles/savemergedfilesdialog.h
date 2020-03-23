#ifndef SAVEMERGEDFILESDIALOG_H
#define SAVEMERGEDFILESDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QAbstractButton>

namespace Ui {
class SaveMergedFilesDialog;
}

class SaveMergedFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveMergedFilesDialog(QStringList & sources, QString & lastPath, QWidget *parent = 0);
    ~SaveMergedFilesDialog();

    void getDestinationNames(QStringList & destination);
    QString getLastPath(){ return m_lastPath; }
private:
    Ui::SaveMergedFilesDialog *ui;

    void fillSources(QStringList & sources);
    void initDestinations();

    QStringList m_destination;
    QString     m_extension;
    QString     m_lastPath;

public slots:
    void slotCheckInputs();
    void slotOneOutputFile(int state);
    void slotBaseFileNameChecked(int state);
    void slotBaseFileNameChanged(const QString & base);
    void slotBrowseButtonClicked();
    void slotUpdateExtensions(QAbstractButton * button);
    void slotUpdateExtensionsForCell(int row, int column);
};

#endif // SAVEMERGEDFILESDIALOG_H

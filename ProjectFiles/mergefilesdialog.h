#ifndef MERGEFILESDIALOG_H
#define MERGEFILESDIALOG_H

#include <QDialog>
#include <qfiledialog.h>
#include <qmessagebox.h>

namespace Ui {
class MergeFilesDialog;
}

class MergeFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MergeFilesDialog(QString lastPath, QWidget *parent = 0);
    ~MergeFilesDialog();

    void getLastPath(QString &path) { path = m_lastPath; }
    void getInputs(QStringList & files, QString & name) { files = m_files; name = m_name; }
    void getExtension(QString &ext);

private:
    Ui::MergeFilesDialog *ui;
    QString m_lastPath;
    QStringList m_files;
    QString m_name;
    
    bool isValidPath(QString & path);
    bool sameExtensions();
    

public slots:
    void checkInputs();
    void addFiles();
    void removeFiles();
    void reject();
    void browseSessionName();
    
};

#endif // MERGEFILESDIALOG_H

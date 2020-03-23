/////////////////////////////////////////////////////////////////
///
/// @file FileUI.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  Aug 17, 2016
///
/// @brief FileUI class interfaces with the user to open and save files
///
/// @note
///

#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QDir>
#include <qstandardpaths.h>

#include "../ProjectFiles/FileUI.h"
#include "../model/channelconfigmodel.h"
#include "../ProjectFiles/mergefilesdialog.h"
#include "../backend/file/fileinterface.h"
#include "../model/configmodel.h"
#include "../ProjectFiles/savemergedfilesdialog.h"
#include "../ProjectFiles/licenseCheck.h"

QStringList FileUI::m_notMatchingExtensions = QStringList() << ".ns1" << ".ns2" << ".nev" << ".pl";

/// @author Holly Ausbeck
/// @date   Aug 17, 2016
/// @brief  Default constructor for FileUI class
/// @param [in]
FileUI::FileUI()
{
    m_bMergedSession = false;
}

/// @author Holly Ausbeck
/// @date   Aug 17, 2016
/// @brief  Destructor for FileUI class - deletes contents of Queue
FileUI::~FileUI()
{   
}

// get the recent file lists for file menu and welcome view from the config model
void FileUI::InitializeRecentFiles()
{
    ConfigModel *cm = ConfigModel::instance();

    m_lastPath = cm->item(0, OFSMODEL_COLUMN_LAST_PATH)->data(Qt::DisplayRole).toString();
    m_recentFiles = cm->item(0, OFSMODEL_COLUMN_RECENT_FILES)->data(Qt::DisplayRole).toStringList();
}


// save the recent files to the config model
void  FileUI::saveRecentFiles()
{
    ConfigModel *cm = ConfigModel::instance();

    cm->setData(cm->index(0, OFSMODEL_COLUMN_LAST_PATH), m_lastPath);
    cm->setData(cm->index(0, OFSMODEL_COLUMN_RECENT_FILES), m_recentFiles);

}

// move the current file to the front and limit the size to 10
void  FileUI::updateRecentFiles()
{
    if (!m_currentFileName.isEmpty())
    {
        m_recentFiles.push_front(m_currentFileName);
        m_recentFiles.removeDuplicates();
    }
    if (m_recentFiles.size() > 10)
        m_recentFiles.pop_back();
}


// file open functions
// the file returned has a full path and extension of .nev, .nsx, .cfs or .pl2
QString FileUI::getInputFileFromUser(QMainWindow *parent)
{
    m_currentFileName = QChar::Null;

    // if there are no batch processes running
    if (!batchProcessesRunning())
    {
        QString path(m_lastPath);
        QString filters = "Blackrock files (*.nev *.ns* *.cfs);;Spike files (*.nev);;Continuous data files (*.ns*);;Concatenated File Sessions (*.cfs)";
        QFileDialog::Options opt = 0;

#ifdef WIN64    // POS only runs on Windows
        filters += ";;Plexon files (*.pl2)";
        opt = QFileDialog::DontUseNativeDialog;
#endif
        m_currentFileName = QFileDialog::getOpenFileName(parent,
            "Please select a file to open", path, filters, 0, opt);

        if (!m_currentFileName.isNull())
        {
            QFileInfo finfo(m_currentFileName);
            QDir dir = finfo.dir();
            m_lastPath = dir.absolutePath();

            updateRecentFiles();
            saveRecentFiles();
            m_bMergedSession = false;
        }
    }
    return m_currentFileName;
}

// file open functions
// the file returned has a full path and extension of .nev, .nsx, .cfs or .pl2
QStringList FileUI::getInputFilesFromUser(QMainWindow *parent, QString &sessionName)
{
    QStringList sFiles;

    // if there are no batch processes running
    if (!batchProcessesRunning())
    {
        MergeFilesDialog dlg(m_lastPath, parent);
        QDialog::DialogCode result = (QDialog::DialogCode)dlg.exec();

        QString name;
        dlg.getInputs(sFiles, name);
        dlg.getLastPath(m_lastPath);

        if (checkStorage(m_lastPath, sFiles) == false)
        {
            sFiles.clear();
            return sFiles;
        }
        sessionName = name;
        m_currentFileName = name;
        m_bMergedSession = true;
    }
    return sFiles;
}


// verify the file exists
// sFile has a full path and extension of .nev, .nsx, .cfs or .pl2
bool FileUI::fileExists(QString sFile)
{
    QFileInfo finfo(sFile);
    if (!finfo.exists())
    {
        QMessageBox msg(QMessageBox::Critical,
            "File not found",
            "The selected file does not exist. "
            "\nPlease select another file to open.",
            QMessageBox::Ok);
        msg.exec();

        if (m_recentFiles.removeOne(sFile))
            updateRecentFiles();

        return false;
    }
    m_currentFileName = sFile;
    return true;
}

// sFile has a full path and extension of .nev, .nsx, .cfs or .pl2
bool FileUI::analysisSessionExists(QString sFile)
{
    QString path = getPathForAnalysisSession(sFile);
    QDir dir(path);
    return dir.exists();
}


// clear all data from analysis session on file close
//bool bDeleteFiles = cm->item(0, OFSMODEL_COLUMN_DELETION_CHANNEL_FILES)->data(Qt::DisplayRole).toBool();

// If this is a merged file session, get the file list from the session file  
// else return a list of one file.
QStringList FileUI::getInputFileList(QString sFile)
{
    QStringList sFileList;
    QString extension;

    QFileInfo finfo(sFile);
    if (finfo.suffix() == "cfs")
    {
        m_bMergedSession = true;
        QFile mfs(sFile);
        mfs.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&mfs);

        while (!in.atEnd()) 
        {
            sFileList << in.readLine();
        }
        mfs.close();

        if (sFileList.size() != 0)
        {
            QFileInfo fileinfo(sFileList[0]);
            extension = fileinfo.suffix();
        }
        else
        {
            QMessageBox msg(QMessageBox::Critical,
                "Empty file",
                "The selected file does not contain a valid concatenation session. "
                "\nPlease select another file to open.",
                QMessageBox::Ok);
            msg.exec();
        }
    }
    else
    {
        m_bMergedSession = false;
        sFileList << sFile;
        extension = finfo.suffix();
    }
    return sFileList;
}

QStringList FileUI::getOutputFilesFromUser(QMainWindow *parent, QString &sSessionPath)
{
    QStringList sFileList;
    FileInterface * fileinterface = FileInterface::instance();
    QStringList sources = fileinterface->getCurrentFileNames();

    if (m_bMergedSession)
    {
        SaveMergedFilesDialog dlg(sources, m_lastPath);
        int result = dlg.exec();
        if ((QDialog::DialogCode) result == QDialog::Accepted)
        {
            dlg.getDestinationNames(sFileList);
            m_lastPath = dlg.getLastPath();
        }
    }
    else
    {
        QString dir(m_lastPath);
        QString selectedFilter;

        QFileDialog::Options opt = 0;
#ifdef WIN64
        opt = QFileDialog::DontUseNativeDialog;
#endif

        QString fn = QFileDialog::getSaveFileName(parent, QString("Save As"),
            dir + "/untitled.nev",
            QString("Neural event file (*.nev);;Matlab file (*.mat);;CSV file (*.csv);;NeuroExplorer (*.nex)"), &selectedFilter, opt);

        // Make sure the filename has the appropriate suffix set
        // If there is no extension, QFileDialog sets the proper extension to match the selected filter, but 
        // if the extension is wrong (i.e. .cs instead of .csv) it won't fix it. We need to do it...
        QString suffix = selectedFilter.mid(selectedFilter.size() - 4, 3);
        QString name = fn.section('.', 0, 0);
        if (!fn.endsWith(suffix))
            fn = name + "." + suffix;

        sFileList << fn;
        m_lastPath = name;
    }
    if (!compareToCurrentFileNames(sFileList, sources))
    {
        QMessageBox msg(QMessageBox::Critical, "Error",
            "The current file(s) cannot be overwritten. Please choose a different file name(s).",
            QMessageBox::Ok);
        msg.exec();

        sFileList.clear();
    }
    sSessionPath = m_lastPath;
    return sFileList;
}

bool FileUI::createSession(QStringList filenames, QString &sSessionfile)
{
    if (createSessionPath(getPathForAnalysisSession(sSessionfile)))
    {
        return createSessionFile(filenames, sSessionfile);
    }
    return false;
}

QStringList FileUI::addNevs(QStringList sFiles, QString &sFile)
{
    if (!nevFilesInInputFileList(sFiles))
    {
        QStringList sNevFileList = checkForCorrespondingNevFileNames(sFiles);

        if (sNevFileList.count())
        {
            sFiles += sNevFileList;
        }
    }
    if (!createSession(sFiles, sFile))
    {
        sFiles.clear();
    }
    return sFiles;
}

// sSessionPath is the path returned by getPathForAnalysisSession()
bool FileUI::createSessionPath(QString sSessionPath)
{
    QDir dir;

    if (!dir.mkpath(sSessionPath))
    {
        QMessageBox msg(QMessageBox::Critical,
            "Session Creation Error",
            "Error creating the session folder.",
            QMessageBox::Ok);
        msg.exec();

        return false;
    }
    return true;
}

bool FileUI::createSessionFile(QStringList filenames, QString &sSessionPath)
{
    if (sSessionPath.isEmpty() == false)
    {
        QFileInfo finfo(sSessionPath);
        QString ext = finfo.suffix();

        if (ext == "cfs")
        {
            QFile sessionFile(sSessionPath);

            if (sessionFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&sessionFile);

                for (int f(0); f < filenames.size(); ++f)
                    out << filenames[f] << "\n";

                sessionFile.close();
                return true;
            }
        }
        else
            return true;
    }
    return false;
}


// sFile has a full path and extension of .nev, .nsx, .cfs or .pl2
// this function should only be called if a previous editing session exists
bool FileUI::continuePreviousEditingSession(QString sFile, QMainWindow *parent)
{
    ConfigModel *cm = ConfigModel::instance();
    bool bAsk = cm->item(0, OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION)->data(Qt::DisplayRole).toBool();

    if (bAsk)
    {
        QMessageBox msgBox(QMessageBox::Question,
            "Editing Session Started",
            "Do you want to continue the previous analysis session for this file?",
            QMessageBox::Yes | QMessageBox::No,
            parent);
        QCheckBox cb("Don't ask me again", &msgBox);
        msgBox.setCheckBox(&cb);

        int ret = msgBox.exec();

        switch (ret)
        {
        case QMessageBox::Yes:
            cm->setData(cm->index(0, OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION), "true");
            break;
        case QMessageBox::No:
            cm->setData(cm->index(0, OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION), "false");
            break;
        }
        if (cb.isChecked())
            cm->setData(cm->index(0, OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION), "false");
        else
            cm->setData(cm->index(0, OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION), "true");
    }
    return cm->item(0, OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION)->data(Qt::DisplayRole).toBool();
}

// Delete the session folder
// sSessionPath = session folder path
void FileUI::deleteSession(QString sSessionPath, bool bDelete)
{
    ConfigModel *cm = ConfigModel::instance();
    // clear all data from analysis session on file close
    bool bDeleteFiles = cm->item(0, OFSMODEL_COLUMN_DELETION_CHANNEL_FILES)->data(Qt::DisplayRole).toBool();

    if (!sSessionPath.isEmpty() && (bDeleteFiles || bDelete))
    {
        QDir dir(sSessionPath);
        dir.removeRecursively();
    }
}

// sFile has a full path and extension of .nev, .nsx, .cfs or .pl2
// create a path name from the file name
QString FileUI::getPathForAnalysisSession(QString sFile)
{
    int index = sFile.lastIndexOf('.');
    QString path = sFile.left(index);
    QString extension = sFile.right(sFile.size() - index - 1);
    path = path + "-" + extension;
    return path;
}

// return false if there are no batch processes running.
bool FileUI::batchProcessesRunning() 
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();

    if (ccm->batchCount() > 0)
    {
        QString message("Do you want to cancel the current Sorting/Detecting batch processes?");

        QMessageBox msg(QMessageBox::Question, "Cancel Batch Processing", message, QMessageBox::Yes | QMessageBox::No);
        int answer = msg.exec();

        if (answer == QMessageBox::Yes)
        {
            ccm->exitBatch();
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}

// If this is a merged file session, get the file list from the session file  
// else return a list of one file.
bool FileUI::nevFilesInInputFileList(QStringList filenames)
{
    QStringList nevs = filenames.filter(QString(".nev"), Qt::CaseInsensitive);
    return (nevs.count() > 0);
}

// If this is a merged file session, get the file list from the session file  
// else return a list of one file.
bool FileUI::nevFilesInInputFileList(QString sFile)
{
    QStringList sFileList = getInputFileList(sFile);
    QStringList nevs = sFileList.filter(QString(".nev"), Qt::CaseInsensitive);
    return (nevs.count() > 0);
}

QStringList FileUI::checkForCorrespondingNevFileNames(QStringList filenames)
{
    QStringList nevNames = getNevNames(filenames);

    if (nevNames.count() == filenames.count())
    {
        QString message("Do you want to also open the following file(s) that appear to be part of the same analysis session?");

        foreach(QString name, nevNames)
        {
            message += "\n" + name;
        }
        QMessageBox msg(QMessageBox::Question, "Open NEV File(s)", message, QMessageBox::Yes | QMessageBox::No);
        int answer = msg.exec();

        if (answer != QMessageBox::Yes)
        {
            nevNames.clear();
        }
    }
    else
    {
        nevNames.clear();
    }
    return nevNames;
}


/// @author Holly Ausbeck
/// @date   Aug 17, 2016
/// @brief  Check the file list for corresponding .nev files
/// @param [in]  list of file names
/// @param [out] list of corresponding .nev files
QStringList FileUI::getNevNames(QStringList & sources)
{
    QStringList NevNames;

    QStringList nevSources = sources.filter(QString(".nev"), Qt::CaseInsensitive);

    if (nevSources.size() > 0)
        return NevNames;

    foreach(QString ext, m_notMatchingExtensions)
    {
        QStringList invalidSources = sources.filter(ext, Qt::CaseInsensitive);

        if (invalidSources.size() > 0)
            return NevNames;
    }
    foreach(QString sName, sources)
    {
        int index = sName.lastIndexOf(".");

        QString sNevName = sName.left(index);
        sNevName += ".nev";
        QFileInfo finfo(sNevName);

        if (finfo.exists())
        {
            NevNames << sNevName;
        }
    }
    return NevNames;
}

/// @author Holly Ausbeck
/// @date   Aug 17, 2016
/// @brief  Verify that none of the destination file names are the same as the current file names
/// @param [in]  list of file names
/// @param [out] true/false = OK to use/conflict
bool FileUI::compareToCurrentFileNames(const QStringList & destination_filenames, const QStringList & source_filenames)
{
    foreach(QString sName, source_filenames)
    {
        if (destination_filenames.contains(sName))
        {
            return false;
        }
    }
    return true;
}

// returns true if there is enough disk space available
bool FileUI::checkStorage(QString path, QStringList filenames)
{
    QStorageInfo storage; /* = QStorageInfo::root();*/
    storage.setPath(path);
    quint64 requiredSize = 0;
    QFileInfo finfo;

    for (int f(0); f < filenames.size(); ++f)
    {
        finfo.setFile(filenames[f]);
        requiredSize += finfo.size();
    }
    requiredSize *= 1.2;
    quint64 bytesAvailable = storage.bytesAvailable();

    if (bytesAvailable < requiredSize)
    {
        // There's not enough space to process file. Put up a message and exit
        QMessageBox msg(QMessageBox::Critical,
            "Not enough available hard disk space",
            "There is not enough free space on your drive to process the selected file(s). "
            "\nBOSS needs as many bytes as the file size that is going to process to be free.",
            QMessageBox::Ok);
        msg.exec();

        return false;
    }
    return true;
}

// returns true if there is enough disk space available
bool FileUI::checkLicense(QMainWindow *parent)
{
	return false;
//    ConfigModel *cm = ConfigModel::instance();
//    bool bNotify = cm->item(0, OFSMODEL_COLUMN_SHOW_NO_LICENSE_MESSAGE)->data(Qt::DisplayRole).toBool();
//    bool bDemoMode = false;
//
//    if (bNotify)
//    {
//        LicenseCheck lc;
//        if (lc.isThereALicense() == false)
//        {
//            QString text = "No license for version " + QString::number(APP_VERSION_MAJOR) + "." + QString::number(APP_VERSION_MINOR) + " was found. The free version of BOSS can only open up to 16 channels. Please contact Blackrock Microsystems Sales to purchase a license.\n\n";
//
//#ifdef OSMAC            
//            text += "If you have purchased a license, please enable your license by downloading <a href = 'https://www.wibu.com/us/downloads-user-software.html'>CodeMeter User Runtime. You may proceed without a license, however the free version of BOSS can only open 16 channels.< / a>";
//            //<a href='https://www.wibu.com/us/downloads-user-software.html</a>";
//#endif
//            QMessageBox msgBox(QMessageBox::Warning,
//                "No License Found",
//                text,
//                QMessageBox::Ok,
//                parent);
//
//            QCheckBox cb("Don't ask me again", &msgBox);
//            msgBox.setCheckBox(&cb);
//
//            msgBox.setTextFormat(Qt::RichText);
//            int ret = msgBox.exec();
//
//            if (cb.isChecked())
//                cm->setData(cm->index(0, OFSMODEL_COLUMN_SHOW_NO_LICENSE_MESSAGE), "false");
//            else
//                cm->setData(cm->index(0, OFSMODEL_COLUMN_SHOW_NO_LICENSE_MESSAGE), "true");
//            bDemoMode = true;
//        }
//    }
//    return bDemoMode;
}





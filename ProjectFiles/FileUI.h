/////////////////////////////////////////////////////////////////
///
/// @file FileUI.h
/// @attention  (c) Copyright 2016 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  Aug 17, 2016
///
/// @brief FileUI class interfaces with the user to open and save files
///
/// @note
///

#ifndef FILEUI_H_INCLUDED
#define FILEUI_H_INCLUDED

#include <QMainWindow>
#include <QStringList>

class FileUI 
{
public:
    FileUI();
    ~FileUI();

    // recent file lists for file menu and welcome view
    void InitializeRecentFiles();
    void saveRecentFiles();
    QStringList getRecentFiles() { return m_recentFiles; }
    void updateRecentFiles();

    // file open/save operations
    QString getInputFileFromUser(QMainWindow *parent);
    QStringList getInputFilesFromUser(QMainWindow *parent, QString &sSessionPath);
    QStringList getOutputFilesFromUser(QMainWindow *parent, QString &sSessionPath);

    // session functions
    QStringList getInputFileList(QString sFile);
    bool createSession(QStringList filenames, QString &sSessionfile);
    QStringList addNevs(QStringList filenames, QString &sSessionfile);

    // utility funcions
    bool fileExists(QString sFile);
    bool analysisSessionExists(QString sFile);
    QString getPathForAnalysisSession(QString sFile);
    bool continuePreviousEditingSession(QString sFile, QMainWindow *parent);
    void deleteSession(QString sSessionPath, bool bDelete);
    bool batchProcessesRunning();
    bool checkStorage(QString path, QStringList filenames);
    bool checkLicense(QMainWindow *parent);
    QString getLastPath() { return m_lastPath; }

private:
    QString         m_currentFileName;
    QString         m_lastPath;             // last path used for file open dialog
    QStringList     m_recentFiles;
    bool            m_bMergedSession;


    static QStringList m_notMatchingExtensions;

    // session functions
    bool createSessionFile(QStringList filenames, QString &sessionName);
    bool createSessionPath(QString sSessionPath);

    // utility funcions
    QStringList getNevNames(QStringList & filenames);
    bool compareToCurrentFileNames(const QStringList & destination_filenames, const QStringList & source_filenames);
    QStringList checkForCorrespondingNevFileNames(QStringList filenames);
    bool nevFilesInInputFileList(QString sFile);
    bool nevFilesInInputFileList(QStringList sFiles);
};

#endif // FILEUI_H_INCLUDED  

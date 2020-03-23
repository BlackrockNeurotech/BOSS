#include <QWidget>
#include <QTableView>
#include <QSettings>
#include <QDir>
#include <QPropertyAnimation>
#include <QFileDialog>
#include <QCheckBox>
#include <QStorageInfo>
#include <qstandardpaths.h>
#include <QSizePolicy>
#include <QProgressDialog>

#include "../ProjectFiles/mainwindow.h"
#include "ui_mainwindow.h"
#include "../backend/dataprocessing/spike/spikeproxymodel.h"
#include "../backend/dataprocessing/continuous/timeproxymodel.h"
#include "../model/channelconfigmodel.h"
#include "../backend/file/fileinterface.h"
#include "../widgets/feature2dtoolbar.h"
#include "../widgets/alignmenttoolbar.h"
#include "../ProjectFiles/mergefilesdialog.h"
#include "../ProjectFiles/savemergedfilesdialog.h"




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_bUiInitialized(false)
{
    m_toolBarWidgets = 0;
    m_toolBarDocks = 0;
    m_channelWindow = 0;
    m_channelTableView = 0;
    m_TimeEnergySplitter = 0;
    m_dockTime = 0;
    m_PeakEnergySplitter = 0;
    m_signalEnergyGraphicsView = 0;
    m_TimeGraphicsView = 0;
    m_TimeWaveformGraphicsView = 0;
    m_FFTGraphicsView = 0;
    m_waveformWindow = 0;
    m_Correlograms = 0;
    m_waveformGraphicsView = 0;
    m_2DWindow = 0;
    m_twoDGraphicsView = 0;
    m_energyGraphicsView = 0;
    m_peakHistogram = 0;
    m_GLWindow = 0;
    m_GLWidget = 0;
    m_UnitsSplitter = 0;
    m_FeatureVSFeature = 0;
    m_slider = 0;
    m_welcomePage = NULL;
    m_central = NULL;
    m_bDemoMode = false;
    m_bStartUp = true;
    m_bFileOpen = false;

    ui->setupUi(this);
#ifdef OSMAC
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    
    
    initDocksToolbar();

    m_bUiInitialized = true;
    m_readURL = new ReadURL(QString("http://www.blackrockmicro.com/BOSS/version.txt"), this);

    connect(m_readURL, SIGNAL(finished()), this, SLOT(slotCheckForUpdatesFinished()));

    m_CurrentWizardPage = WIZARDPAGE_END;    
    initOptionsToolbar();
    ui->leftToolBar->setStyleSheet("QToolBar { border: 0px; background-color: #FFFFFF}");

    QObject::connect(FileInterface::instance(), SIGNAL(sucessImportingFile(int)), this, SLOT(slotInitData(int)));
    QObject::connect(FileInterface::instance(), SIGNAL(error(int, QString)), this, SLOT(slotDisplayError(int, QString)));
    QObject::connect(FileInterface::instance(), SIGNAL(fileImported()), this, SLOT(slotFileImported()));

    initStateActions();

    QCoreApplication::setOrganizationName("Blackrock");
    QCoreApplication::setOrganizationDomain("blackrockmicro.com");
    QCoreApplication::setApplicationName("Blackrock Offline Spike Sorter");

    initToolbar();

    // instantiate the channel config model
    ChannelConfigModel * ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(ccm, SIGNAL(channelStateChanged(int, ChannelState)), this, SLOT(slotChannelStateChanged(int, ChannelState)));
    connect(ccm, SIGNAL(spikesInitialized(int)), this, SLOT(slotSpikesInitialized(int)));
    connect(ccm, SIGNAL(errorFound(int,QString)), this, SLOT(slotDisplayError(int,QString)));
    connect(ccm, SIGNAL(sortFeaturesChanged(int)), this, SLOT(slotSortFeaturesChanged(int)));
    connect(ccm, SIGNAL(setUndoEnabled(bool)), this, SLOT(slotSetUndoEnabled(bool)));
    connect(ccm, SIGNAL(setRedoEnabled(bool)), this, SLOT(slotSetRedoEnabled(bool)));
    connect(ccm, SIGNAL(signalEnergyTypeChanged(int)), this, SLOT(slotSignalEnergyTypeChanged(int)));
    connect(ccm, SIGNAL(glEditStateChanged(int, GLWidgetState)), this, SLOT(slotGlEditStateChanged(int, GLWidgetState)));
    

    // continuous proxy models (timescan and timeslice) must be initialized before spike scan
    TimeProxyModel::instance();
    SpikeProxyModel::instance();

    // Try loading ConfigModel XML
    ConfigModel *cm = ConfigModel::instance();
    QString path = ConfigModel::configPath();
    cm->getFromXML(path);

    m_fileUI.InitializeRecentFiles();

    setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::AllowNestedDocks);// | QMainWindow::AnimatedDocks);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);   

    restoreMain(path);
    hideDisplayWindows();
    updateRecentFiles();
    QThread* thread = QThread::currentThread();
    thread->setPriority(QThread::TimeCriticalPriority);

}

MainWindow::~MainWindow() 
{
    delete ui;
    if (m_welcomePage)
        delete m_welcomePage;
    if (m_central)
        delete m_central;

}

void MainWindow::initDocksToolbar()
{
    m_toolBarDocks = new DockToolbar(this);
    m_toolBarDocks->setWindowTitle("Docks Toolbar");
    m_toolBarDocks->setObjectName("Docks Toolbar");

    addToolBar(Qt::TopToolBarArea, m_toolBarDocks);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QString dir = ConfigModel::configPath();

    if (!m_fileUI.batchProcessesRunning())
    {
        closeFile();
        ConfigModel::instance()->saveToXML(dir);
        event->accept();
    }
    else
        event->ignore();
}

// why is this here???
void MainWindow::changeEvent( QEvent* event )
{
    if ((event->type() == QEvent::WindowStateChange) && (windowState() == Qt::WindowFullScreen))
    {
        if (m_toolBarWidgets && m_toolBarWidgets->isVisible())
        {
            m_toolBarWidgets->hide();
            m_toolBarWidgets->show();
        }

        if (m_toolBarDocks && m_toolBarDocks->isVisible())
        {
            m_toolBarDocks->hide();
            m_toolBarDocks->show();
        }

        if(ui->mainToolBar->isVisible())
        {
            ui->mainToolBar->hide();
            ui->mainToolBar->show();
        }

        if(ui->leftToolBar->isVisible())
        {
            ui->leftToolBar->hide();
            ui->leftToolBar->show();
        }
        else
        {
            ui->leftToolBar->show();
            ui->leftToolBar->hide();
        }
    }
}


void MainWindow::initOptionsToolbar()
{
    /* Options Slider */
    IconResourceFiles(":/new/Bitmaps/Bitmaps/OptionsWithBar.png", ":/new/Bitmaps/Bitmaps/OptionsWithBar.png", ":/new/Bitmaps/Bitmaps/OptionsWithBarMouseOver.png", ":/new/Bitmaps/Bitmaps/OptionsWithBarPushed.png"),
        
    ui->dockOptions->setAllowedAreas(Qt::LeftDockWidgetArea);

    QIcon icon(":/new/Bitmaps/Bitmaps/options_PB.png");
    icon.addFile(":/new/Bitmaps/Bitmaps/options_PB.png", QSize(), QIcon::Normal, QIcon::On);
    icon.addFile(":/new/Bitmaps/Bitmaps/options_PB_hovered.png", QSize(), QIcon::Active, QIcon::On);
    icon.addFile(":/new/Bitmaps/Bitmaps/options_PB_pushed.png", QSize(), QIcon::Active, QIcon::Off);

    ui->dockOptions->resetWindowTitle();
    ui->dockOptions->setObjectName("Options");
    m_dockOptions = ui->dockOptions;
    m_actionOptions = ui->dockOptions->toggleViewAction();
    m_actionOptions->setIcon(icon);

    ui->leftToolBar->setIconSize(QSize(24, 100));
    ui->leftToolBar->addAction(m_actionOptions);
    ui->leftToolBar->setContentsMargins(0, 0, 0, 0);
    QToolButton* toolButton = (QToolButton*)ui->leftToolBar->widgetForAction(m_actionOptions);
    toolButton->setStyleSheet("QToolButton { border: 0px;}");

    connect(m_actionOptions, SIGNAL(toggled(bool)), this, SLOT(slotOptionsChange(bool)));
    m_actionOptions->setChecked(false);
}

void MainWindow::showOptions(bool show)
{
    bool bChecked = m_actionOptions->isChecked();
    m_actionOptions->setVisible(show);

    if (!bChecked && show)
    {
        m_actionOptions->setChecked(false);
        m_actionOptions->activate(QAction::Trigger);
    }
    else if (bChecked && !show)
    {
        m_actionOptions->setChecked(true);
        m_actionOptions->activate(QAction::Trigger);
    }
}

void MainWindow::showWindow()
{
    if (isVisible() == false)
    {
//#ifdef TRIAL
//
//        QString filedir = ConfigModel::configPath()
//
//        filedir += "/validityCheck";
//        Expiration exp(filedir);
//        int daysLeft;
//        bool bExpired;
//
//        bExpired = exp.CheckExpiry(&daysLeft);
//
//        if (bExpired)
//        {
//            QMessageBox msg(QMessageBox::Critical,
//                "Trial Period Expired",
//                "The trial period for this software has expired. Please contact Blackrock Sales.",
//                QMessageBox::Ok, this);
//            msg.exec();
//            return;
//        }
//        else
//        {
//            QString text = "The remaining time of the trial period for this software is " + QString::number(daysLeft) + " day(s). ";
//                                                                                                                                                                                                                                            Please contact Blackrock Sales if you wish to renew the trial period.";
//
//            QMessageBox msg(QMessageBox::Warning,
//                "Trial Period Remaining Time",
//                text,
//                QMessageBox::Ok, this);
//            msg.exec();
//        }
//
//#endif

        // Check license and notify the user if this is running on demo mode and enable/disable saving
        show();
        m_bDemoMode = m_fileUI.checkLicense(this);
        checkForUpdatesOnStartUp();
        m_bStartUp = false;
    }
}

void MainWindow::slotWizardPageChanged(int channel, WizardPage page)
{
    Q_UNUSED(channel);

    if (m_CurrentWizardPage != page)
    {
        bool firstTime = m_CurrentWizardPage == WIZARDPAGE_END;

        if (!firstTime)
            saveDocksSize();

        if (m_CurrentWizardPage != WIZARDPAGE_END)
        {
            m_toolBarDocks->updateDocks(page, false);

            if ((page == WIZARDPAGE_SORT) && (m_CurrentWizardPage == WIZARDPAGE_DETECT))
            {
                m_toolBarDocks->raiseDock(DOCK_FEATURES);
            }
        }
        // set the options page
        m_slider->changeState(page);
        m_CurrentWizardPage = page;

        // the widget toolbar only applies to features
        if (m_CurrentWizardPage == WIZARDPAGE_DETECT)
        {
            m_toolBarWidgets->hide();
            slotSignalEnergyTypeChanged(channel);
//            m_TimeGraphicsView->show();
            m_TimeWaveformGraphicsView->hide();
        }
        else if ((m_signalEnergyGraphicsView != 0) && (m_TimeGraphicsView != 0))
        {
            m_toolBarWidgets->show();
            m_signalEnergyGraphicsView->hide();
            m_TimeGraphicsView->hide();
//            m_TimeWaveformGraphicsView->show();
        }
        if (!firstTime)
            restoreDocksSize();
    }
}

void MainWindow::hideDisplayWindows()
{
    enableActions(false);
    ui->actionSave->setEnabled(false);

    m_actionOptions->setVisible(false);
    m_actionOptions->setChecked(true);
    m_actionOptions->activate(QAction::Trigger);

    if (m_toolBarWidgets != 0)
        m_toolBarWidgets->hide();

    if (m_toolBarDocks != 0)
    {
        m_toolBarDocks->hideDocks();
        m_toolBarDocks->hide();
    }
    ui->mainToolBar->hide();
    switchCentralWidget(true);
 }

void MainWindow::enableActions(bool bEnable)
{
    ui->actionClose_File->setEnabled(bEnable);
    ui->actionLoad_Parameters->setEnabled(bEnable);
    ui->actionReset_Layout->setEnabled(bEnable);
    ui->actionSave_Editing_Session->setEnabled(bEnable);
    ui->actionSave_Parameters->setEnabled(bEnable);

    if (bEnable == false)
    {
        ui->actionRedo->setEnabled(bEnable);
        ui->actionUndo->setEnabled(bEnable);
    }
}

void  MainWindow::slotStateDetect()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();

    WizardPage page = (WizardPage)ccm->data(ccm->index(channel, CC_PAGE), Qt::UserRole).toInt();

    if (page != WIZARDPAGE_DETECT)
        ccm->setData(ccm->index(channel, CC_PAGE), QVariant(WIZARDPAGE_DETECT), Qt::DisplayRole);
}

void  MainWindow::slotStateSort()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();

    WizardPage page = (WizardPage)ccm->data(ccm->index(channel, CC_PAGE), Qt::UserRole).toInt();

    if (page != WIZARDPAGE_SORT)
        ccm->setData(ccm->index(channel, CC_PAGE), QVariant(WIZARDPAGE_SORT), Qt::DisplayRole);
}

void  MainWindow::slotStatePublish()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();

    WizardPage page = (WizardPage)ccm->data(ccm->index(channel, CC_PAGE), Qt::UserRole).toInt();

    if (page != WIZARDPAGE_EVALUATE)
        ccm->setData(ccm->index(channel, CC_PAGE), QVariant(WIZARDPAGE_EVALUATE), Qt::DisplayRole);
}

void MainWindow::SetButtonWizardPage()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    WizardPage page = (WizardPage)ccm->data(ccm->index(ccm->CurrentChannel(), CC_PAGE), Qt::UserRole).toInt();

    QPushButton *button = (QPushButton *)ActionStateList[page]->getWidget();
    button->setChecked(true);
}

// called when the user selects file/open
void MainWindow::slotOpenFile()
{
    QString sFile = m_fileUI.getInputFileFromUser(this);

    if (!sFile.isNull())
    {
        openFile(sFile);
    }
}

// called when the user selects a file from the recent file list
void MainWindow::slotOpenFile(QString sFile)
{
    if (m_fileUI.batchProcessesRunning())
        return;

    openFile(sFile);
}

// note: !!!!!  this should be the same slot as open file.
// then multi-select file menu.
// then save a cfs file for > 1 even if it is ns5 & nev
// Then if a nev is already included when continuing a session, don't check for nev.
void MainWindow::slotOpenAndMerge()
{
    QString sFile;
    QStringList sFiles = m_fileUI.getInputFilesFromUser(this, sFile);

    closeFile();
    openFile(sFile, sFiles);
}

// sFile has a full path and extension of .nev, .nsx, .cfs or .pl2
void MainWindow::openFile(QString sFile)
{
    // Verify that file exists
    if (!m_fileUI.fileExists(sFile))
    {
        // if it doesn't exist, remove it from recent files menu and welcome screen
        updateRecentFiles();
        return;
    }
    // get the list of files belonging to this session   
    QStringList sFiles = m_fileUI.getInputFileList(sFile);
    openFile(sFile, sFiles);
}

// sFile has a full path and extension of .nev, .nsx, .cfs or .pl2
// sFiles is the list of files belonging to a concatenation session, or the single file
bool MainWindow::openFile(QString sFile, QStringList sFiles)
{
    if (sFiles.isEmpty())
        return false;

    if (m_fileUI.checkStorage(sFile, sFiles) == false)
        return false;

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    closeFile();
    ccm->nukeModel();

    m_sSessionPath = m_fileUI.getPathForAnalysisSession(sFile);
    QFileInfo finfo(sFile);

    if (m_fileUI.analysisSessionExists(sFile))
    {
        if (m_fileUI.continuePreviousEditingSession(sFile, this))
        {
            QString xmlPath = m_sSessionPath + "/channelModel.xml";

            if (QFile(xmlPath).exists())
            {
                // there is a previous editing session, read the xml
                if (!ccm->getFromXML(xmlPath))
                {
                    // There is a problem with the previous editing session, can't read session file, start over
                    m_fileUI.deleteSession(m_sSessionPath, true);
                }
            }
            else
            {
                // There is a problem with the previous editing session, no session file, start over
                m_fileUI.deleteSession(m_sSessionPath, true);
            }
        }
        else
        {
            // The user doesn't want to continue the previous editing session, start over
            m_fileUI.deleteSession(m_sSessionPath, true);
        }
    }
    if (!m_fileUI.analysisSessionExists(sFile))
    {
        sFiles = m_fileUI.addNevs(sFiles, sFile);
        m_fileUI.createSession(sFiles, sFile);
    }

    if (sFiles.count() == 0)
        return false;

    m_fileUI.updateRecentFiles();
    FileInterface * fileinterface = FileInterface::instance();

    if (fileinterface->startEditingSession(sFiles, m_sSessionPath, m_bDemoMode))
    {
        QString sIssue = fileinterface->getLastError();

        if (!sIssue.isEmpty())
        {
            QString message(" Do you want to continue the editing session?");
            message = sIssue + message;

            // There was a possible issue opening the files ask the user if they want to proceed anyway
            QMessageBox msg(QMessageBox::Question, "Open file(s)", message, QMessageBox::Yes | QMessageBox::No);
            int answer = msg.exec();

            if (answer != QMessageBox::Yes)
            {
                return false;
            }
        }
        QString title("Blackrock Offline Spike Sorter - " + finfo.fileName());
        this->setWindowTitle(title);
        QtConcurrent::run(fileinterface, &FileInterface::readFile);

        initialize(!fileinterface->hasContinuousData());
    }
    else
    {
        // There was an error opening the file
        QMessageBox msg(QMessageBox::Critical, "Error opening file",
            fileinterface->getLastError(),
            QMessageBox::Ok);
        msg.exec();
        return false;
    }
    return true;
}


// update the recent file lists for file menu and welcome view
void MainWindow::updateRecentFiles()
{
    m_fileUI.updateRecentFiles();
    m_fileUI.saveRecentFiles();
    QStringList recentFiles = m_fileUI.getRecentFiles();

    if (!recentFiles.isEmpty())
    {
        ui->menuRecent_Files->clear();
        ui->menuRecent_Files->setEnabled(true);
        
        if (m_welcomePage != NULL)
            m_welcomePage->clearRecentFiles();

        foreach(QString str, recentFiles)
        {
            // File menu
            RecentFileItem * action = new RecentFileItem(str, ui->menuRecent_Files);
            connect(action, SIGNAL(triggered(QString)), this, SLOT(slotOpenFile(QString)));
            ui->menuRecent_Files->addAction((QAction *)action);

            // Welcome screen
            if (m_welcomePage != NULL)
                m_welcomePage->addRecentFile(str);
        }
    }
    else if (m_bUiInitialized)
        ui->menuRecent_Files->setDisabled(true);
}

bool MainWindow::systemUpToDate(QString urlText, bool *bMajorUpdate, bool *bMinorUpdate, bool *bBuildUpdate)
{
    // urlText begins with Ver and then there's a list of the latest versions separated by spaces
    // it is important to maintain this formatting for proper parsing here

    *bMajorUpdate = false;
    *bMinorUpdate = false;
    *bBuildUpdate = false;
    bool ret = true;

    // Discard the "Ver" portion
    if (urlText.startsWith("VER "))
    {
        urlText.remove("VER ");
    }
    
    QStringList versions = urlText.split(" ", QString::SkipEmptyParts);
    
    for (int i(0); i < versions.size(); ++i)
    {
        QStringList MajorMinorBuild = versions[i].split(".");
        if (MajorMinorBuild.size() != 3)
            continue;
        if (MajorMinorBuild[0].toInt() == APP_VERSION_MAJOR)
        {
            // Major is the same, compare minor
            if (MajorMinorBuild[1].toInt() == APP_VERSION_MINOR)
            {
                // Minor is the same, compare build
                if (MajorMinorBuild[2].toInt() > APP_VERSION_BUILD)
                {
                    *bBuildUpdate = true;
                    ret = false;
                }
            }
            else if (MajorMinorBuild[1].toInt() > APP_VERSION_MINOR)
            {
                *bMinorUpdate = true;
                ret = false;
            }
        }
        else if (MajorMinorBuild[0].toInt() > APP_VERSION_MAJOR)
        {
            *bMajorUpdate = true;
            ret = false;
        }

    }

    return ret;
}

QMenu * MainWindow::createPopupMenu()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    WizardPage page = (WizardPage)ccm->data(ccm->index(ccm->CurrentChannel(), CC_PAGE), Qt::UserRole).toInt();
    
    QMenu * menu = QMainWindow::createPopupMenu();
    this->m_toolBarDocks->createPopupMenu(menu, page);
   
    QList<QAction *> list = menu->actions();

    QString sTitle = "Widgets Toolbar";
    QAction *action = 0;

    // find the corresponding action in the menu
    for (int i(0); i < list.size(); ++i)
    {
        QAction *act = list[i];
        QString text = act->text();

        if (text == sTitle)
        {
            action = act;
            break;
        }
    }
    if (page == WIZARDPAGE_DETECT)
    {
        if (action != 0)
        {
            action->setDisabled(true);
            action->setChecked(false);
        }
    }
    else if (page == WIZARDPAGE_SORT)
    {
        if (action != 0)
        {
            action->setEnabled(true);
        }
    }
    return menu;
}

void MainWindow::switchCentralWidget(bool bWelcome)
{
    if (bWelcome)
    {
        if (m_welcomePage == NULL)
        {
            m_welcomePage = new WelcomePage(this);
            connect(m_welcomePage, SIGNAL(signalOpenLinkActivated()), this, SLOT(slotOpenFile()), Qt::QueuedConnection);
            connect(m_welcomePage, SIGNAL(signalOpenMergeLinkActivated()), this, SLOT(slotOpenAndMerge()), Qt::QueuedConnection);
            connect(m_welcomePage, SIGNAL(signalRecentLinkActivated(QString)), this, SLOT(slotOpenFile(QString)), Qt::QueuedConnection);
        }

        bool centralSet = this->centralWidget() == m_central;
        this->setCentralWidget(m_welcomePage);

        if (centralSet)
        {
            m_central = NULL;
        }
    }
    else
    {
        if (m_central == NULL)
        {
            m_central = new QWidget();
            m_central->setMaximumHeight(1);
        }

        bool welcomeSet = this->centralWidget() == m_welcomePage;
        this->setCentralWidget(m_central);

        if (welcomeSet)
        {
            m_welcomePage = NULL;
        }

    }
}

void MainWindow::slotFileImported()
{
    ui->actionSave->setEnabled(true);
}

void MainWindow::slotInitData(int channel)
{
    if (channel != -1)
    {
        m_channelTableView->setFirstChannelWithData(channel);
        m_slider->initGeometry();
        m_toolBarWidgets->initialize();
        restoreSettings(ConfigModel::configPath());
        showOptions(true);
        enableActions(true);

        m_toolBarDocks->updateDocks(m_CurrentWizardPage, true);

        if (m_CurrentWizardPage == WIZARDPAGE_SORT)
            m_toolBarWidgets->show();
        else
            m_toolBarWidgets->hide();

        m_toolBarDocks->show();
        ui->mainToolBar->show();
        ui->leftToolBar->show();
        init3DWidget();
    }
    else
    {
        FileInterface * fileinterface = FileInterface::instance();
        QMessageBox msg(QMessageBox::Critical, "Error opening file",
                    fileinterface->getLastError(),
                    QMessageBox::Ok);
        msg.exec();
        closeFile(true);
        hideDisplayWindows();
    }
}

void MainWindow::initialize(bool spikeOnly)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(wizardPageChanged(int, WizardPage)), this, SLOT(slotWizardPageChanged(int, WizardPage)));
    WizardPage initialWizardPage = WIZARDPAGE_DETECT;
    
    switchCentralWidget(false);

    for (int i = 0; i < WIZARDPAGE_END; ++i)
    {
        ActionStateList[i]->setVisible(false);
        ActionStateList[i]->setChecked(false);
    }
    if (spikeOnly)
    {
        initialWizardPage = WIZARDPAGE_SORT;
    }
    ActionStateList[WIZARDPAGE_DETECT]->setIconVisibleInMenu(!spikeOnly);

    initChannelTable();
    initSlider();
    initWidgetsToolbar();
    initHistogramFiltering();

    if (spikeOnly)
    {
        initTimeLineSpike();
    }
    else
    {
        initFFT();
        initHistogramFiltering();
        initTimeLineContinuous();
    }
    initWaveform();
    init2DFeature();
    init3DWindow();
    initUnits();
    initFeatureVSFeature();
    initCorrelograms();

    for (int i = initialWizardPage; i < WIZARDPAGE_END; ++i)
    {
        ActionStateList[i]->setVisible(true);
    }
    m_bFileOpen = true;
}

void MainWindow::initWidgetsToolbar()
{
    if (m_toolBarWidgets == 0)
    {
        m_toolBarWidgets = new MainWidgetToolBar(this);
        m_toolBarWidgets->setWindowTitle("Widgets Toolbar");
        m_toolBarWidgets->setObjectName("Widgets Toolbar");
        addToolBar(Qt::TopToolBarArea, m_toolBarWidgets);
        m_toolBarWidgets->hide();

        m_toolBarWidgets->setStyleSheet("QToolBar { border: 0px; background-color: #FFFFFF}");
    }
}

void MainWindow::initChannelTable()
{
    if (m_channelWindow == 0)
    {
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_CHANNELS);
        m_channelWindow = new QMainWindow(dock);
        m_channelWindow->setWindowFlags(Qt::Widget);
        m_channelWindow->setContextMenuPolicy(Qt::NoContextMenu);

        // tetrodes
        //ChannelToolBar *toolbar = new ChannelToolBar(m_channelWindow);
        //m_channelWindow->addToolBar(toolbar);

        m_channelTableView = new ChannelTableView(m_channelWindow);
        m_channelWindow->setCentralWidget(m_channelTableView);
        dock->setWidget(m_channelWindow);
    }
}

void MainWindow::initHistogramFiltering()
{
    if (m_PeakEnergySplitter == 0)
    {
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_PEAKHEIGHTS);
        m_PeakEnergySplitter = new QSplitter(Qt::Horizontal, dock);

        // create energy
        m_energyGraphicsView = new EnergyGraphicsView(m_PeakEnergySplitter);

        // create peak heights
        m_peakHistogram = new PeakHeightsGraphicsView(m_PeakEnergySplitter);

        m_PeakEnergySplitter->addWidget(m_peakHistogram);
        m_PeakEnergySplitter->addWidget(m_energyGraphicsView);
        m_PeakEnergySplitter->setVisible(false);
        dock->setWidget(m_PeakEnergySplitter);
    }
}

void MainWindow::initTimeLineContinuous()
{
    if (m_dockTime == 0)
    {
        QVBoxLayout *vLayout = new QVBoxLayout;
        QHBoxLayout *hLayout = new QHBoxLayout;
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_TIMELINE);

        // create the tool bar for scrolling through large files
        m_ScrollRangeToolBar = new ScrollRangeToolBar(dock);
        // create the tool bar for zooming
        m_ZoomToolBar = new ZoomToolBar(dock);

        // create time/energy splitter
        m_TimeEnergySplitter = new QSplitter(Qt::Vertical, dock);

        // create timeline
        m_TimeGraphicsView = new TimeGraphicsView(m_TimeEnergySplitter);

        // create time waveform view
        m_TimeWaveformGraphicsView = new TimeWaveformGraphicsView(m_TimeEnergySplitter);

        // create signal energy
        m_signalEnergyGraphicsView = new SignalEnergyGraphicsView(m_TimeEnergySplitter);

        m_TimeEnergySplitter->addWidget(m_signalEnergyGraphicsView);
        m_TimeEnergySplitter->addWidget(m_TimeGraphicsView);
        m_TimeEnergySplitter->addWidget(m_TimeWaveformGraphicsView);

        QObject::connect(m_ZoomToolBar, SIGNAL(zoomChanged(zoomType)), m_signalEnergyGraphicsView, SLOT(slotZoomChanged(zoomType)));
        QObject::connect(m_ZoomToolBar, SIGNAL(zoomChanged(zoomType)), m_TimeGraphicsView, SLOT(slotZoomChanged(zoomType)));
        QObject::connect(m_ZoomToolBar, SIGNAL(zoomChanged(zoomType)), m_TimeWaveformGraphicsView, SLOT(slotZoomChanged(zoomType)));

        hLayout->addWidget(m_TimeEnergySplitter);
        hLayout->addWidget(m_ZoomToolBar);
        hLayout->setSpacing(0);
        hLayout->setContentsMargins(0, 0, 0, 0);

        vLayout->addWidget(m_ScrollRangeToolBar);
        vLayout->addLayout(hLayout);
        vLayout->setSpacing(0);
        vLayout->setContentsMargins(0, 0, 0, 0);

        m_dockTime = new QWidget(dock);
        dock->setWidget(m_dockTime);
        m_dockTime->setLayout(vLayout);

        connect(m_TimeGraphicsView->horizontalScrollBar(), SIGNAL(valueChanged(int)), m_signalEnergyGraphicsView->horizontalScrollBar(), SLOT(setValue(int)));
    }
}

void MainWindow::initTimeLineSpike()
{
    if (m_dockTime == 0)
    {
        QVBoxLayout *vLayout = new QVBoxLayout;
        QHBoxLayout *hLayout = new QHBoxLayout;
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_TIMELINE);

        // create the tool bar for scrolling through large files
        m_ScrollRangeToolBar = new ScrollRangeToolBar(dock);
        // create the tool bar for zooming
        m_ZoomToolBar = new ZoomToolBar(dock);

        // create time waveform view
        m_TimeWaveformGraphicsView = new TimeWaveformGraphicsView(m_TimeEnergySplitter);

        QObject::connect(m_ZoomToolBar, SIGNAL(zoomChanged(zoomType)), m_TimeWaveformGraphicsView, SLOT(slotZoomChanged(zoomType)));

        hLayout->addWidget(m_TimeWaveformGraphicsView);
        hLayout->addWidget(m_ZoomToolBar);
        hLayout->setSpacing(0);
        hLayout->setContentsMargins(0, 0, 0, 0);

        vLayout->addWidget(m_ScrollRangeToolBar);
        vLayout->addLayout(hLayout);
        vLayout->setSpacing(0);
        vLayout->setContentsMargins(0, 0, 0, 0);

        m_dockTime = new QWidget(dock);
        dock->setWidget(m_dockTime);
        m_dockTime->setLayout(vLayout);
    }
}

void MainWindow::initFFT()
{
    if (m_FFTGraphicsView == 0)
    {
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_FREQSPEC);
        m_FFTGraphicsView = new FFTGraphicsView(dock);
        dock->setWidget(m_FFTGraphicsView);
    }
}

void MainWindow::initWaveform()
{
    if (m_waveformWindow == 0)
    {
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_WAVEFORMS);
        m_waveformWindow = new QMainWindow(dock);
        m_waveformWindow->setWindowFlags(Qt::Widget);
        m_waveformWindow->setContextMenuPolicy(Qt::NoContextMenu);

        AlignmentToolBar *toolbar = new AlignmentToolBar(m_waveformWindow);
        m_waveformWindow->addToolBar(toolbar);

        m_waveformGraphicsView = new WaveformGraphicsView(m_waveformWindow);
        m_waveformWindow->setCentralWidget(m_waveformGraphicsView);
        dock->setWidget(m_waveformWindow);
    }
}

void MainWindow::init2DFeature()
{
    if (m_2DWindow == 0)
    {
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_2DCLUSTER);
        m_2DWindow = new QMainWindow(dock);
        m_2DWindow->setWindowFlags(Qt::Widget);
        m_2DWindow->setContextMenuPolicy(Qt::NoContextMenu);

        Feature2DToolBar *toolbar = new Feature2DToolBar(m_2DWindow);
        m_2DWindow->addToolBar(toolbar);

        m_twoDGraphicsView = new FeatureGraphicsView(FEATUREID_PCA1, FEATUREID_PCA2, m_2DWindow);
        m_2DWindow->setCentralWidget(m_twoDGraphicsView);
        dock->setWidget(m_2DWindow);
    }
}

void MainWindow::init3DWindow()
{
    if (m_GLWindow == 0)
    {
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_3DCLUSTER);
        m_GLWindow = new QMainWindow(dock);
        m_GLWindow->setWindowFlags(Qt::Widget);
        m_GLWindow->setContextMenuPolicy(Qt::NoContextMenu);

        m_toolbar3D = new Feature3DToolBar(m_GLWindow);
        m_GLWindow->addToolBar(m_toolbar3D);
        dock->setWidget(m_GLWindow);
    }
}

void MainWindow::init3DWidget()
{
    if (m_GLWidget == 0)
    {
        m_GLWidget = new GLWidget(m_GLWindow);
        m_GLWindow->setCentralWidget(m_GLWidget);

        connect(m_toolbar3D->m_actionReset, SIGNAL(pressed()), m_GLWidget, SLOT(slotResetRotation()));
    }
}

void MainWindow::initUnits()
{
    if (m_UnitsSplitter == 0)
    {
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_UNITS);
        m_UnitsSplitter = new UnitsSplitter(dock);
        dock->setWidget(m_UnitsSplitter);
    }
}

void MainWindow::initFeatureVSFeature()
{
    if (m_FeatureVSFeature == 0)
    {
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_FEATURES);
        m_FeatureVSFeature = new FeatureVSFeature(dock);
        dock->setWidget(m_FeatureVSFeature);
    }
}

void MainWindow::initCorrelograms()
{
    if (m_Correlograms == 0)
    {
        DockWidget* dock = m_toolBarDocks->getDockWidget(DOCK_CORRELOGRAMS);
        m_Correlograms = new Correlograms(dock);
        dock->setWidget(m_Correlograms);
    }
}

void MainWindow::slotCloseFile()
{
    if (m_fileUI.batchProcessesRunning())
        return;

    closeFile();
}

void MainWindow::closeFile(bool bError)
{
    FileInterface * fileinterface = FileInterface::instance();
    fileinterface->closeFile();

    if (m_bFileOpen)
    {
        // save the channel file information
        QString path = m_sSessionPath;
        path += "/channelModel.xml";
        ChannelConfigModel::instance()->saveToXML(path);

        // save the general editing settings
        QString dir = ConfigModel::configPath();
        saveSettings(dir);
    }
    if (m_bFileOpen || bError)
    {   
        ui->mainToolBar->hide();
        showOptions(false);
        hideDisplayWindows(); 
        updateRecentFiles();

        if (m_toolBarWidgets != 0)
        {
            delete m_toolBarWidgets;
            m_toolBarWidgets = 0;
        }
        if (m_slider != 0)
        {
            delete m_slider;
            m_slider = 0;
        }
        if (m_waveformWindow != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_WAVEFORMS);
            delete m_waveformWindow;
            m_waveformWindow = 0;
            m_waveformGraphicsView = 0;
        }
        if (m_2DWindow != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_2DCLUSTER);
            delete m_2DWindow;
            m_2DWindow = 0;
            m_twoDGraphicsView = 0;
        }
        if (m_GLWindow != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_3DCLUSTER);
            delete m_GLWindow;
            m_GLWindow = 0;
            m_GLWidget = 0;
        }
        if (m_dockTime != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_TIMELINE);
            delete m_dockTime;
            m_dockTime = 0;
            m_ScrollRangeToolBar = 0;
            m_ZoomToolBar = 0;
            m_TimeEnergySplitter = 0;
            m_TimeGraphicsView = 0;
            m_TimeWaveformGraphicsView = 0;
            m_signalEnergyGraphicsView = 0;
        }
        if (m_PeakEnergySplitter != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_PEAKHEIGHTS);
            delete m_PeakEnergySplitter;
            m_PeakEnergySplitter = 0;
            m_peakHistogram = 0;
            m_energyGraphicsView = 0;
        }
        if (m_FFTGraphicsView != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_FREQSPEC);
            delete m_FFTGraphicsView;
            m_FFTGraphicsView = 0;
        }
        if (m_channelWindow != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_CHANNELS);
            delete m_channelWindow;
            m_channelWindow = 0;
            m_channelTableView = 0;
        }
        if (m_UnitsSplitter != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_UNITS);
            delete m_UnitsSplitter;
            m_UnitsSplitter = 0;
        }
        if (m_FeatureVSFeature != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_FEATURES);
            delete m_FeatureVSFeature;
            m_FeatureVSFeature = 0;
        }
        if (m_Correlograms != 0)
        {
            m_toolBarDocks->releaseWidget(DOCK_CORRELOGRAMS);
            delete m_Correlograms;
            m_Correlograms = 0;
        }
        QString title("Blackrock Offline Spike Sorter");
        this->setWindowTitle(title);
    }
    m_CurrentWizardPage = WIZARDPAGE_END;
    m_bFileOpen = false;
}


void MainWindow::getDestinationFileNames(QStringList & destination_filenames)
{
    destination_filenames.clear();

}

void MainWindow::checkForUpdatesOnStartUp()
{
    ConfigModel * cm = ConfigModel::instance();
    bool bCheck = cm->item(0, OFSMODEL_COLUMN_UPDATES_ON_STARTUP)->data(Qt::UserRole).toBool();
    if (!bCheck)
        return;

    m_bStartUp = true;
    slotCheckForUpdates();

}

void MainWindow::slotSave()
{
    //LicenseCheck lc;
    //if (lc.isThereALicense() == false)
    //{
    //    QMessageBox msg(QMessageBox::Critical, "No license found",
    //        "Please plugin your dongle in order to proceed",
    //        QMessageBox::Ok);
    //    msg.exec();
    //    m_bDemoMode = true;
    //    return;
    //}
    //m_bDemoMode = false;

    FileInterface * FI = FileInterface::instance();
    QString sSession;
    QStringList destination_filenames = m_fileUI.getOutputFilesFromUser(this, sSession);

    if (destination_filenames.isEmpty())
        return;
    
    DataSaver * saver = FI->saveResults(destination_filenames);
    if (saver)
    {            
        QProgressDialog *dlg = new QProgressDialog("Saving in progress...", "Cancel", 0, 100, this);
        dlg->setMinimumDuration(1000);      
        dlg->show();
        connect(saver, SIGNAL(signalProgress(int)), dlg, SLOT(setValue(int)));
        connect(saver, SIGNAL(finished()), dlg, SLOT(reset()));
        connect(dlg, SIGNAL(canceled()), FileInterface::instance(), SLOT(slotCancelSave()));
        QThreadPool::globalInstance()->start(saver, QThread::HighestPriority);
    }
}


void MainWindow::slotSaveEditingSession()
{
    QString path = m_sSessionPath;

    if (!path.isEmpty())
    {
        path += "/channelModel.xml";
        ChannelConfigModel::instance()->saveToXML(path);
    }
}

void MainWindow::initToolbar()
{
    ui->mainToolBar->setStyleSheet("QToolBar { border: 0px; background-color: #FFFFFF}");
    ui->mainToolBar->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < WIZARDPAGE_END; ++i)
    {
        ui->mainToolBar->addAction(ActionStateList[i]);
        ActionStateList[i]->setVisible(false);
        ActionStateList[i]->setChecked(false);
    }

    QString styleSheet =
            "QPushButton {color: white;}"
            "QPushButton {background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 #333333, stop: 0.9 #ffffff);}"
            "QPushButton:hover {background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 #646464, stop: 0.9 #ffffff);}"
            "QPushButton:checked {background-color: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 #333333, stop: 0.9 #00FF00);}"
            "QPushButton:disabled {background-color: #c0c0c0;}"
            "QPushButton {min-height: 2.0em; min-width: 5em; border-style: outset; border-color: grey; border-width: 1px; border-radius: 3px; }";

    for (int i = 0; i < WIZARDPAGE_END; ++i)
    {
        QPushButton *pb = (QPushButton *)ActionStateList[i]->getWidget();

        if(pb)
        {
            pb->setCheckable(true);
            pb->setAttribute(Qt::WA_LayoutUsesWidgetRect);
            ActionStateGroup->addButton(pb);
        }

//        ActionStateList[i]->setStyleSheet(styleSheet);

    }
    connect((QPushButton *)ActionStateList[WIZARDPAGE_DETECT]->getWidget(),
            SIGNAL(clicked()), this, SLOT(slotStateDetect()) );
    connect((QPushButton *)ActionStateList[WIZARDPAGE_SORT]->getWidget(),
            SIGNAL(clicked()), this, SLOT(slotStateSort()) );
    connect((QPushButton *)ActionStateList[WIZARDPAGE_EVALUATE]->getWidget(),
            SIGNAL(clicked()), this, SLOT(slotStatePublish()) );
}

void MainWindow::initStateActions()
{
    ActionStateList[WIZARDPAGE_DETECT] = new ToolbarWidgetAction(ui->mainToolBar, QString("Detect"), ACTIONWIDGETTYPE_BLACKROCKRADIOBUTTON);
    ActionStateList[WIZARDPAGE_SORT] = new ToolbarWidgetAction(ui->mainToolBar, QString("Sort"), ACTIONWIDGETTYPE_BLACKROCKRADIOBUTTON);
    ActionStateList[WIZARDPAGE_EVALUATE] = new ToolbarWidgetAction(ui->mainToolBar, QString("Publish"), ACTIONWIDGETTYPE_BLACKROCKRADIOBUTTON);

    ActionStateGroup = new QButtonGroup();
    ActionStateGroup->setExclusive(true);
}

void MainWindow::initSlider()
{
    // Initialize slider
    if (m_slider == 0)
    {
        m_slider = new OptionsSlider(m_dockOptions);
        m_slider->setToolbar(ui->leftToolBar);
        m_dockOptions->setWidget(m_slider);
        ui->leftToolBar->setContentsMargins(0, 0, 0, 0);
    }
}

void MainWindow::slotPreferences()
{
    PreferencesDialog preferencesDialog(this);
    preferencesDialog.ShowMaxSamples(FileInterface::instance()->hasContinuousData());
    preferencesDialog.exec();
}

void MainWindow::slotAboutBOSS()
{    
    QString text = "<p align='left'>" + QString("<b>BOSS Version: </ b>") + APP_VERSION_STR + "<br>"
        + " </p>";
    text += QString(APP_COPYRIGHT_STR) + "<br>";

    text += "<table style = 'width:100%'>"
        "<tr>"
        "<td><b>Qt: </b>< / td>"
        "<td>Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies)< / td>"
        "< / tr>"
        "<tr>"
        "<td><b>DspFilters: </b>< / td>"
        "<td>Copyright (C) 2009 by Vincent Falco< / td>"
        "< / tr>"
        "< / table>";

 
    QMessageBox msg(QMessageBox::Information,
        "About BOSS...",
        text,
        QMessageBox::Ok);

    msg.setIconPixmap(QPixmap(":/new/Bitmaps/Bitmaps/BOSS.png"));
    
    msg.exec();

}

void MainWindow::slotResetLayout()
{
    QString dir = QCoreApplication::applicationDirPath();
    restoreSettings(dir);

    m_toolBarDocks->updateDocks(m_CurrentWizardPage);

    if (m_CurrentWizardPage == WIZARDPAGE_SORT)
        m_toolBarWidgets->show();
    else
        m_toolBarWidgets->hide();
}

void MainWindow::slotUndo()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ccm->undo(ccm->CurrentChannel());
}

void MainWindow::slotRedo()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ccm->redo(ccm->CurrentChannel());
}

void MainWindow::slotSaveParameters()
{
    QString dir(m_sSessionPath);
    QFileDialog::Options opt = 0;
#ifdef WIN64
    opt = QFileDialog::DontUseNativeDialog;
#endif
    QString fn = QFileDialog::getSaveFileName(this, tr("Save File"),
        dir + "/parameters.bos",
        tr("BOS Files (*.bos)"),
        (QString *)0,
        opt);

    if (!fn.isEmpty())
    {
        ChannelConfigModel::instance()->saveToXML(fn, true);
    }
}


void MainWindow::slotLoadParameters()
{
    QString path(m_sSessionPath);
    QFileDialog::Options opt = 0;
    
#ifdef WIN64    
    opt = QFileDialog::DontUseNativeDialog;
#endif
    
    QString fn = QFileDialog::getOpenFileName(this,
        "Please select a parameter file", path, "BOS files (*.bos)", 0, opt);

    if (!fn.isEmpty())
    {
        // Make user select channels to copy parameters
        LoadParametersDialog dlg;
        int ret = dlg.exec();

        if ((ret == QDialog::Accepted) && (dlg.getSelection() != LOADPARAMETERS_NONE))
        {
            QList<int> channels = dlg.getRange();
            ChannelConfigModel::instance()->getParameters(fn, channels);
        }
    }
}


void MainWindow::slotCheckForUpdates()
{
    if (!m_readURL->read())
    {
        // If checking for updates on startup, we want to be silent, and only display message if there are updates available
        if (!m_bStartUp)
        {
            QString text = "There were connection errors and the system could not check if there are updates available. Please verify your network connectivity and try again.";

            QMessageBox msg(QMessageBox::Information,
                "Check For Updates",
                text,
                QMessageBox::Ok);

            msg.exec();
        }
    }
}


void MainWindow::slotCheckForUpdatesFinished()
{
    // Open dialog
    QMessageBox msg;
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle("Check For Updates");
    msg.setStandardButtons(QMessageBox::Ok);
    bool bUpdatesAvailable = false;

    QString text;

    if (m_readURL->error())
    {
        text = "There were connection errors and the system could not check if there are updates available. Please verify your network connectivity and try again.";
    }
    else
    {
        QString str = m_readURL->getContents();
        bool bMajorUpdateAvailable, bMinorUpdateAvailable, bBuildUpdateAvailable;
        if (systemUpToDate(str, &bMajorUpdateAvailable, &bMinorUpdateAvailable, &bBuildUpdateAvailable))
        {
            text = "BOSS is up-to-date";
        }
        else
        {
            bUpdatesAvailable = true;

            if (bMinorUpdateAvailable || bBuildUpdateAvailable)
            {
                text += "There is a newer version available for your license. Use the link below to download it. \n";
            }
            if (bMajorUpdateAvailable)
            {
                text += "There is a newer version available for purchase. You can download a trial version using the link below.\n\nContact Sales at Blackrock Microsystems to upgrade.\n";
            }       
            QString webAddress = "www.blackrockmicro.com/BOSS"; 
            text += "\n" + webAddress;
        }
    }

    msg.setText(text);  
    msg.setIconPixmap(QPixmap(":/new/Bitmaps/Bitmaps/BOSS.png"));

    if (bUpdatesAvailable || !m_bStartUp)
        msg.exec();

    m_bStartUp = false;
}


void MainWindow::slotSetUndoEnabled(bool enabled)
{
    ui->actionUndo->setEnabled(enabled);
}

void MainWindow::slotSetRedoEnabled(bool enabled)
{
    ui->actionRedo->setEnabled(enabled);
}

void MainWindow::slotChannelChanged(int channel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ChannelState state = (ChannelState)ccm->data(ccm->index(channel, CC_STATE), Qt::UserRole).toInt();
    bool detecting = (state == CHANNELSTATE_DETECTING);
    bool sorting = (state == CHANNELSTATE_SORTING);
    bool disablePages = detecting || sorting;
    SetAllWizardPagesEnabled(!disablePages);

    if(detecting)
        ActionStateList[WIZARDPAGE_DETECT]->setEnabled(true);
    else if(sorting)
        ActionStateList[WIZARDPAGE_SORT]->setEnabled(true);

    bool haveSpikes = ccm->data(ccm->index(channel, CC_SDFILE_INIT), Qt::UserRole).toBool();
    quint64 nWaveforms = ccm->data(ccm->index(channel, CC_NWAVEFORMS), Qt::UserRole).toULongLong();

    if (detecting)
    {
        ActionStateList[WIZARDPAGE_SORT]->setEnabled(false);
        ActionStateList[WIZARDPAGE_EVALUATE]->setEnabled(false);
    }
    else if(!sorting)
    {
        haveSpikes = haveSpikes && (nWaveforms > 0);
        ActionStateList[WIZARDPAGE_SORT]->setEnabled(haveSpikes);
        ActionStateList[WIZARDPAGE_EVALUATE]->setEnabled(false);

        if (haveSpikes)
        {
            ActionStateList[WIZARDPAGE_EVALUATE]->setEnabled(false);
        }
    }
    SetButtonWizardPage();

    if (m_CurrentWizardPage == WIZARDPAGE_DETECT)
        slotSignalEnergyTypeChanged(channel);
}

void MainWindow::slotSortFeaturesChanged(int channel)
{
    slotChannelChanged(channel);
}

void MainWindow::slotOptionsChange(bool visible)
{
}

void MainWindow::slotSignalEnergyTypeChanged(int channel)
{
    if ((m_signalEnergyGraphicsView != 0) && (m_energyGraphicsView != 0) && (m_TimeGraphicsView != 0))
    {
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        SignalEnergy se = (SignalEnergy)ccm->data(ccm->index(channel, CC_SIGNAL_ENERGY), Qt::UserRole).toInt();

        if ((se == ADDITIONAL_SIGNAL_ENERGY) || (se == ADDITIONAL_NONLINEAR_SIGNAL_ENERGY) || (se == ADDITIONAL_SIGNED_SIGNAL_ENERGY))
        {
            if (!m_signalEnergyGraphicsView->isVisible())
            {
                QRect r = m_dockTime->geometry();
                r.setHeight(m_dockTime->height() / 2);
                m_signalEnergyGraphicsView->resize(m_TimeGraphicsView->width(), m_dockTime->height() / 2);
                r.translate(0, m_dockTime->height() / 2);
                m_TimeGraphicsView->resize(m_TimeGraphicsView->width(), m_dockTime->height() / 2);
                m_TimeEnergySplitter->setStretchFactor(0, 1);
                m_TimeEnergySplitter->setStretchFactor(1, 2);
                m_TimeEnergySplitter->setStretchFactor(2, 1);
            }
            if (m_CurrentWizardPage == WIZARDPAGE_DETECT)
            {
                m_energyGraphicsView->show();
                m_signalEnergyGraphicsView->show();
                m_signalEnergyGraphicsView->slotZoomChanged(FitToView);
                m_TimeGraphicsView->slotZoomChanged(FitToView);
            }
        }
        else
        {
            m_energyGraphicsView->hide();
            m_signalEnergyGraphicsView->hide();
        }
    }
}

// when the state changes to define clusters, make the 3D window on top
void MainWindow::slotGlEditStateChanged(int channel, GLWidgetState editState)
{
    if (editState == GLWIDGETSTATE_CENTROIDS)
        m_toolBarDocks->raiseDock(DOCK_3DCLUSTER);
}

void MainWindow::slotSpikesInitialized(int channel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    if(channel == ccm->CurrentChannel())
        slotChannelChanged(channel);
}

void MainWindow::slotChannelStateChanged(int channel, ChannelState state)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();

    if (channel == ccm->CurrentChannel())
        slotChannelChanged(channel);

    bool enable = true;

    //if ((ccm->channelSelectionCount() > 1) || (ccm->channelSelectionCount() == 0))
    //    enable = false;

    ui->actionLoad_Parameters->setEnabled(enable);
    ui->actionSave_Parameters->setEnabled(enable);

//    {
//        SetAllWizardPagesEnabled(!detecting);

//        if(detecting)
//        {
//            ActionStateList[WIZARDPAGE_DETECT]->setEnabled(true);
//        }
//    }
}

void MainWindow::SetAllWizardPagesEnabled(bool enable)
{
    for (int i = 0; i < WIZARDPAGE_END; ++i)
        ActionStateList[i]->setEnabled(enable);
}


void MainWindow::slotDisplayError(int channel, QString error)
{
    QString title;
    if (channel >= 0)
        title = "Error in Channel " + QString::number(channel);
    else
        title = "Error found";

    QMessageBox msg(QMessageBox::Critical,
                title,
                error,
                QMessageBox::Ok);
    msg.exec();
}

// Note the user's settings are saved in Documents\Blackrock Microsystems\Configuration
void MainWindow::saveSettings(QString dir)
{
    dir = dir + "/settings.ini";
    QSettings settings(dir, QSettings::IniFormat);

    m_toolBarDocks->showDocks();
    settings.setValue("MainWindow/Geometry", saveGeometry());
    settings.setValue("MainWindow/State", saveState());
}

// Note the default settings are saved in Documents\Blackrock Microsystems\Configuration\Blackrock Microsystems\Data
void MainWindow::restoreSettings(QString dir)
{
    dir = dir + "/settings.ini";

    QSettings settings(dir, QSettings::IniFormat);

    QSize minSize = minimumSize();
    this->setFixedSize(this->size());

    restoreState(settings.value("MainWindow/State").toByteArray());

    this->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    this->setMinimumSize(minSize);

    restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
}

void MainWindow::restoreMain(QString dir)
{
    dir = dir + "/settings.ini";

    QSettings settings(dir, QSettings::IniFormat);
    restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
}

void MainWindow::saveDocksSize()
{

    m_docsWidth.clear();
    m_docsHeight.clear();

    DockWidget* dockchannels = m_toolBarDocks->getDockWidget(DOCK_CHANNELS);
    m_docsWidth.push_back(dockchannels->width());
    m_docsHeight.push_back(dockchannels->height());
    DockWidget* dockTimeline = m_toolBarDocks->getDockWidget(DOCK_TIMELINE);
    m_docsWidth.push_back(dockTimeline->width());
    m_docsHeight.push_back(dockTimeline->height());


}

void MainWindow::restoreDocksSize()
{
    DockWidget* dockchannels = m_toolBarDocks->getDockWidget(DOCK_CHANNELS);
    DockWidget* dockTimeline = m_toolBarDocks->getDockWidget(DOCK_TIMELINE);
    QList<QDockWidget*> docks;
    docks.push_back(dockchannels);
    docks.push_back(dockTimeline);
    resizeDocks(docks, m_docsWidth, Qt::Horizontal);
    resizeDocks(docks, m_docsHeight, Qt::Vertical);
}

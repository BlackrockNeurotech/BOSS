#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QtConcurrent>
#include <QLabel>
#include <QProgressBar>
#include <QString>
#include <QDate>
#include <QButtonGroup>

#include "../model/channelconfigmodel.h"
#include "../model/configmodel.h"
#include "../widgets/optionsslider.h"
#include "../widgets/channeltableview.h"
#include "../widgets/glwidgetpc.h"
#include "../graphics/peakheightsgraphicsview.h"
#include "../graphics/timegraphicsview.h"
#include "../graphics/timewaveformgraphicsview.h"
#include "../graphics/FFTGraphicsView.h"
#include "../graphics/waveformgraphicsview.h"
#include "../graphics/featuregraphicsview.h"
#include "../graphics/energygraphicsview.h"
#include "../graphics/signalenergygraphicsview.h"
#include "../widgets/UnitsSplitter.h"
#include "../widgets/featurevsfeature.h"
#include "../widgets/correlograms.h"
#include "../widgets/ScrollRangeToolBar.h"
#include "../widgets/zoomtoolbar.h"
#include "../widgets/preferencesdialog.h"
#include "../widgets/loadparametersdialog.h"
#include "../widgets/toolbarWidgetAction.h"
#include "../widgets/DockWidget.h"
#include "../widgets/MainWidgetToolBar.h"
#include "../widgets/RecentFileItem.h"
#include "../ProjectFiles/dockMap.h"
#include "../backend/network/readURL.h"
#include "../widgets/DockToolbar.h"
#include "../ProjectFiles/welcomepage.h"
#include "../ProjectFiles/fileui.h"
#include "../widgets/feature3dtoolbar.h"
#include "../widgets/ChannelToolBar.h"
#include "../ProjectFiles/AppVersion.h"


enum CentralWindows
{
    CENTRALWINDOW_PEAKHIST,
    CENTRALWINDOW_FEATURES,
    CENTRALWINDOW_WAVEFORMS,
    CENTRALWINDOW_PUBLISH,
    CENTRALWINDOW_NA
};

struct IconResourceFiles
{
    IconResourceFiles(QString n, QString d, QString a, QString s)
    {
        normal = n;
        disabled = d;
        active = a;
        selected = s;
    }
    QString normal;
    QString disabled;
    QString active;
    QString selected;
};



namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Slider
    OptionsSlider *m_slider;
    QSize sizeHint() const { return this->size(); }

private:

    Ui::MainWindow *ui;
    QAction* m_actionOptions;
    DockWidget* m_dockOptions;
    ToolbarWidgetAction* ActionStateList[WIZARDPAGE_END];

    QButtonGroup *ActionStateGroup;
    WizardPage m_CurrentWizardPage;
    FileUI m_fileUI;

    void initialize(bool spikeOnly);

    void initOptionsToolbar();
    void initWidgetsToolbar();
    void initDocksToolbar();
    void initStateWindowMap();
    void initChannelTable();
    void initFFT();
    void initHistogramFiltering();
    void initTimeLineContinuous();
    void initTimeLineSpike();
    void initWaveform();
    void init3DWindow();
    void init3DWidget();
    void init2DFeature();
    void initUnits();
    void initFeatureVSFeature();
    void initCorrelograms();
    void initToolbar();
    void initStateActions();
    void initSlider();
    void WizardPageChanged(WizardPage state);
    void SetButtonWizardPage();
    void SetAllWizardPagesEnabled(bool enable);

    void showOptions(bool show);
    void hideDisplayWindows();
    void enableActions(bool bEnable);
    void updateRecentFiles();
    bool systemUpToDate(QString urlText, bool *bMajorUpdate, bool *bMinorUpdate, bool *bBuildUpdate);
    QMenu * createPopupMenu();
    void switchCentralWidget(bool bWelcome);

    MainWidgetToolBar        *m_toolBarWidgets;
    DockToolbar              *m_toolBarDocks;
    Feature3DToolBar         *m_toolbar3D;

    // waveforms
    QMainWindow              *m_waveformWindow;
    WaveformGraphicsView     *m_waveformGraphicsView;
    // 2D features
    QMainWindow              *m_2DWindow;
    FeatureGraphicsView      *m_twoDGraphicsView;
    // 3D features
    QMainWindow              *m_GLWindow;
    GLWidget                 *m_GLWidget;
    // timeline
    QWidget                  *m_dockTime;
    QSplitter                *m_TimeEnergySplitter;
    ScrollRangeToolBar       *m_ScrollRangeToolBar;
    ZoomToolBar              *m_ZoomToolBar;
    SignalEnergyGraphicsView *m_signalEnergyGraphicsView;
    TimeGraphicsView         *m_TimeGraphicsView;
    TimeWaveformGraphicsView *m_TimeWaveformGraphicsView;
    // peak heights and energy histograms
    QSplitter                *m_PeakEnergySplitter;
    PeakHeightsGraphicsView  *m_peakHistogram;
    EnergyGraphicsView       *m_energyGraphicsView;
    // fft
    FFTGraphicsView          *m_FFTGraphicsView;
    // channel list
    QMainWindow              *m_channelWindow;
    ChannelTableView         *m_channelTableView;
    // units 
    UnitsSplitter            *m_UnitsSplitter;
    // feature vs feature grid
    FeatureVSFeature         *m_FeatureVSFeature;
    // Correlograms grid
    Correlograms             *m_Correlograms;

    bool                      m_bUiInitialized;
    bool                      m_bDemoMode;
    bool                      m_bFileOpen;
    QString                   m_sSessionPath;
    WelcomePage              *m_welcomePage;
    QWidget                  *m_central;

    ReadURL                  *m_readURL;
    bool                      m_bStartUp;     // Checking for updates on startup

    QList<int> m_docsWidth;
    QList<int> m_docsHeight;


    void saveSettings(QString dir);
    void restoreSettings(QString dir);
    void restoreMain(QString dir);
    void saveDocksSize();
    void restoreDocksSize();

public slots:

    void showWindow();
    // Channel Model slots
    void slotWizardPageChanged(int channel, WizardPage page);
    void slotInitData(int channel);
    void slotChannelChanged(int channel);
    void slotSpikesInitialized(int channel);
    void slotChannelStateChanged(int channel, ChannelState state);
    void slotDisplayError(int channel, QString error);
    void slotSortFeaturesChanged(int channel);
    void slotOptionsChange(bool);
    void slotSignalEnergyTypeChanged(int channel);
    void slotGlEditStateChanged(int channel, GLWidgetState editState);
    void slotFileImported();

protected:
    void closeEvent(QCloseEvent *event);
    void changeEvent( QEvent* e );

private: 
    void getDestinationFileNames(QStringList & destination_filenames);
    void checkForUpdatesOnStartUp();
    void closeFile(bool bError = false);
    void openFile(QString filename);
    bool openFile(QString sFile, QStringList sFiles);

private slots:
    void slotStateDetect();
    void slotStateSort();
    void slotStatePublish();

    void slotOpenFile();
    void slotOpenFile(QString filename);
    void slotOpenAndMerge();

    void slotCloseFile();
    void slotSave();
    void slotSaveEditingSession();
    void slotPreferences();
    void slotAboutBOSS();
    void slotResetLayout();
    void slotUndo();
    void slotRedo();
    void slotSaveParameters();
    void slotLoadParameters();
    void slotCheckForUpdates();
    void slotCheckForUpdatesFinished();

    void slotSetUndoEnabled(bool enabled);
    void slotSetRedoEnabled(bool enabled);
};


#endif // MAINWINDOW_H

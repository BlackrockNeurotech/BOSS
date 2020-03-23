#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

// ConfigModel.h
#include <QStandardItemModel>
#include <QString>
#include <QDir>

static const quint64 DEFAULT_TIMELINE_SAMPLES = 1800000 * 2;
static const quint64 DEFAULT_TIMELINE_SPIKES = 10000;


// The OFS model is a table format. The rows are channels and the collumns are channel attributes
enum ConfigModelColumn
{
    // File Information
    OFSMODEL_COLUMN_LAST_PATH,
    OFSMODEL_COLUMN_RECENT_FILES,  

    // Display settings
    OFSMODEL_COLUMN_UNIT_COLORS,
    OFSMODEL_COLUMN_POINT_SIZE,
    OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES,             // number of continuous samples to display
    OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS,           // number of spikes to display
    OFSMODEL_COLUMN_APPLY_TO_ALL_SELECTED_CHANNELS,

    // User Preferences
    OFSMODEL_COLUMN_DELETION_CHANNEL_FILES,                   // clear all data from analysis session on file close
    OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION,            // Automatically continuous previous analysis session when file is opened if one exists
    OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION,	  // Ask whether to continuous previous analysis session when file is opened
    OFSMODEL_COLUMN_SHOW_NO_LICENSE_MESSAGE,	              // Show the no license message on BOSS startup.
    OFSMODEL_COLUMN_UPDATES_ON_STARTUP,                       // check for updates on startup

    OFSMODEL_COLUMN_END
};

const int MAX_COLUMNS = OFSMODEL_COLUMN_END;
const int MAX_ROWS = 1;
const int NBR_ROWS = 1;

class ConfigModel : public QStandardItemModel
{
    Q_OBJECT

public:
    static ConfigModel* instance();
    static QString configPath();
    static QString docPath();

    static QColor ColorGradientLight;
    static QColor ColorGradientDark;
    static QColor ColorGradientLightHover;
    static QColor ColorGradientDarkHover;
    static QColor ColorGradientSelectedLight;
    static QColor ColorGradientSelectedDark;
    static QColor ColorGradientSelectedLightHover;
    static QColor ColorGradientSelectedDarkHover;
    static QColor ColorGradientBorderLight;
    static QColor ColorGradientBorderDark;
    static QColor ColorDisabled;
    static QColor ColorUnitText;
    static QColor ColorThresholds;
    static QColor ColorSeparators;
    static QColor ColorHighlight;
    static QColor ColorContinuous;
    static QColor ColorExtracted;
    static QColor ColorTooltip;
    static QColor ColorDockTitlebar;
    static QColor ColorDockTitlebarForeground;
    static QColor ColorX_Axis;
    static QColor ColorY_Axis;
    static QColor ColorZ_Axis;


    // QAbstractTableModel virtual functions
    ConfigModel(QObject *parent = 0);

    QColor getColor(int unit);
    QList<QColor> getColors() { return m_unitColors; }
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    bool saveToXML(QString dir);
    bool getFromXML(QString dir);

    static QStringList ColumnNames;

private:
    static ConfigModel *inst;
    void generateColors();
    void setColors();
    QList<QColor> m_unitColors;

    QString getColumnName(ConfigModelColumn column);

signals:
    void pointSizeChanged(int size);
    void numDisplayedSamplesChanged();
    void numDisplayedWaveformsChanged();
};

#endif // CONFIGMODEL_H

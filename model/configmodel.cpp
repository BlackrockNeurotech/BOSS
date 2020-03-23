#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QStandardPaths>

#include "../model/configmodel.h"
#include "../model/channelconfigmodel.h"

/*static*/
QColor ConfigModel::ColorGradientLight = QColor("#e9e9e9");
QColor ConfigModel::ColorGradientDark = QColor("#b7b7b7");
QColor ConfigModel::ColorGradientLightHover = QColor("#f5f5f5");
QColor ConfigModel::ColorGradientDarkHover = QColor("#bbbbbb");
QColor ConfigModel::ColorGradientSelectedLight = QColor("#00b3f9");
QColor ConfigModel::ColorGradientSelectedDark = QColor("#008ec6");
QColor ConfigModel::ColorGradientSelectedLightHover = QColor("#00b7ff");
QColor ConfigModel::ColorGradientSelectedDarkHover  = QColor("#0099d0");
QColor ConfigModel::ColorGradientBorderLight = QColor("#1da7c2");
QColor ConfigModel::ColorGradientBorderDark = QColor("#0072a3");
QColor ConfigModel::ColorDisabled = QColor(Qt::gray);
QColor ConfigModel::ColorUnitText = QColor("#009FE3");
QColor ConfigModel::ColorThresholds = QColor("#009FE3");
QColor ConfigModel::ColorSeparators = QColor("#009FE3");
QColor ConfigModel::ColorHighlight = QColor("#009FE3");
QColor ConfigModel::ColorContinuous = QColor(Qt::darkGray).darker();
QColor ConfigModel::ColorExtracted = QColor(Qt::lightGray);
QColor ConfigModel::ColorTooltip = QColor(Qt::lightGray);
QColor ConfigModel::ColorDockTitlebar = QColor("#DBDBDB");
QColor ConfigModel::ColorDockTitlebarForeground = QColor("#B9B9B9");
QColor ConfigModel::ColorX_Axis = QColor(Qt::red);
QColor ConfigModel::ColorY_Axis = QColor(Qt::green);
QColor ConfigModel::ColorZ_Axis = QColor("dodgerblue");

ConfigModel *ConfigModel::inst = 0;


/// @note
/// header string data for each column in the model
/* static */
QStringList ConfigModel::ColumnNames = QStringList()
/* File */
<< "Last Path" << "Recent Files"

/* Display */
<< "Unit Colors" << "Point Size" << "Number Samples to Display" << "Number Spikes to Display" << "Apply Settings to All Selected Channels"

/* Preferences */
<< "Deletion of Channel Files" << "Continue Prev Editing Session" << "Ask Continue Prev Editing Session" << "Show no license found message"
<< "Updates on Startup";

ConfigModel *ConfigModel::instance()
{
  if (!inst)
    inst = new ConfigModel();
  return inst;
}

QString ConfigModel::docPath()
{
    QString path;
#ifdef OSMAC
    path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
    path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif
    return path;
}

QString ConfigModel::configPath()
{
    QString path;
#ifdef OSMAC
    path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
    path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif
    path += "/Blackrock Microsystems/Configuration";
    return path;
}


ConfigModel::ConfigModel(QObject *parent)
    :QStandardItemModel(1, MAX_COLUMNS, parent)
{
    // Initialize the data    
    QString path = docPath() + "/Blackrock Microsystems/Data";
    setData(index(0, OFSMODEL_COLUMN_LAST_PATH), path);
    setData(index(0, OFSMODEL_COLUMN_RECENT_FILES), QStringList());

    // Unit colors
    generateColors();
    setColors();

    setData(index(0, OFSMODEL_COLUMN_POINT_SIZE), "2");
    setData(index(0, OFSMODEL_COLUMN_DELETION_CHANNEL_FILES), "false" );
    setData(index(0, OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION), "true");
	setData(index(0, OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION), "true");
	setData(index(0, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES), QString::number(DEFAULT_TIMELINE_SAMPLES));
    setData(index(0, OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS), QString::number(DEFAULT_TIMELINE_SPIKES));
    setData(index(0, OFSMODEL_COLUMN_APPLY_TO_ALL_SELECTED_CHANNELS), "false");
    setData(index(0, OFSMODEL_COLUMN_SHOW_NO_LICENSE_MESSAGE), "true");
    setData(index(0, OFSMODEL_COLUMN_UPDATES_ON_STARTUP), "false");

    connect(this, SIGNAL(numDisplayedSamplesChanged()), ChannelConfigModel::instance(), SLOT(slotMaxDisplayedSamplesChanged()));
    connect(this, SIGNAL(numDisplayedWaveformsChanged()), ChannelConfigModel::instance(), SLOT(slotMaxDisplayedWaveformsChanged()));
}

bool ConfigModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    QStandardItemModel::setData(index, value, role);

    if (index.column() == OFSMODEL_COLUMN_POINT_SIZE)
    {
        emit pointSizeChanged(value.toInt());
    }
    else if (index.column() == OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES)
    {
        emit numDisplayedSamplesChanged();
    }
    else if (index.column() == OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS)
    {
        emit numDisplayedWaveformsChanged();
    }
	return true;
}

void ConfigModel::generateColors()
{
    QStringList strColors, strBaseColors;
    strBaseColors << "darkgray" << "lime" << "dodgerblue" << "magenta" << "yellow" << "aqua" << "red" << "darkorchid" << "green" << "salmon" << "teal" << "deeppink" <<
        "sandybrown" << "deepskyblue" << "moccasin" << "mediumslateblue" << "palevioletred" << "firebrick" << "darkseagreen" << "lightblue" << "sienna" <<
        "plum" << "springgreen" << "mediumvioletred" << "turquoise" << "tomato" << "tan" << "gold" << "steelblue" << "navajowhite" << "coral" << "cornflowerblue" << 
        "crimson" << "greenyellow" << "rosybrown" << "royalblue" << "hotpink" << "khaki" << "darkorange" << "blueviolet" << "chocolate" << "cadetblue" << "lightsteelblue" <<
        "aquamarine" << "darkgoldenrod" << "navy" << "indianred" << "mediumvioletred" << "lightyellow" << "lightpink" << "skyblue" << "tan" << "powderblue" << "indigo" << 
        "olive" << "peru" << "seashell" << "thistle" << "violet" << "yellowgreen" << "darksalmon" << "orangered" << "darkturquoise" << "mediumpurple" ;
    
    strColors << strBaseColors << strBaseColors << strBaseColors << strBaseColors;
    
    setData(index(0, OFSMODEL_COLUMN_UNIT_COLORS), strColors);
}

void ConfigModel::setColors()
{
    // get the color configuration
    QStringList colors = item(0, OFSMODEL_COLUMN_UNIT_COLORS)->data(Qt::DisplayRole).toStringList();

    for (int i = 0; i < colors.size(); ++i)
    {
        m_unitColors.push_back(QColor(colors[i]));
    }
}

QColor ConfigModel::getColor(int unit)
{
    QColor color(Qt::red);

    if ((unit != INVALID_WF) && (unit < m_unitColors.size()))
       color = m_unitColors.at(unit);

    return color;
}


/// @author Sylvana Alpert
/// @date   July 9, 2015
/// @brief  Write the contents of the model to XML
/// @param [in] dir of the file that will hold the XML
/// @return TRUE if success; FALSE otherwise
bool ConfigModel::saveToXML(QString dir)
{
    dir = dir + "/configModel.xml";
    QFile file(dir);

    if (!file.open(QFile::WriteOnly | QIODevice::Text))
        return false;

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("BOSS");


    writer.writeStartElement("File");
    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_LAST_PATH), item(0, OFSMODEL_COLUMN_LAST_PATH)->data(Qt::DisplayRole).toString());
    
    writer.writeStartElement(getColumnName(OFSMODEL_COLUMN_RECENT_FILES));
    QStringList files = item(0, OFSMODEL_COLUMN_RECENT_FILES)->data(Qt::DisplayRole).toStringList();
    foreach(QString str, files)
        writer.writeTextElement("file", str);
    writer.writeEndElement();

    writer.writeEndElement();

    writer.writeStartElement("Display");
    writer.writeStartElement(getColumnName(OFSMODEL_COLUMN_UNIT_COLORS));
    QStringList colors = item(0, OFSMODEL_COLUMN_UNIT_COLORS)->data(Qt::DisplayRole).toStringList();
    foreach(QString str, colors)    
        writer.writeTextElement("color", str);    
    writer.writeEndElement();

    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_POINT_SIZE), item(0, OFSMODEL_COLUMN_POINT_SIZE)->data(Qt::DisplayRole).toString());
    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES), item(0, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES)->data(Qt::DisplayRole).toString());
    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS), item(0, OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS)->data(Qt::DisplayRole).toString());
    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_APPLY_TO_ALL_SELECTED_CHANNELS), item(0, OFSMODEL_COLUMN_APPLY_TO_ALL_SELECTED_CHANNELS)->data(Qt::DisplayRole).toString());
    writer.writeEndElement();
    
    writer.writeStartElement("Preferences");
    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_DELETION_CHANNEL_FILES), item(0, OFSMODEL_COLUMN_DELETION_CHANNEL_FILES)->data(Qt::DisplayRole).toString());
    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION), item(0, OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION)->data(Qt::DisplayRole).toString());
    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION), item(0, OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION)->data(Qt::DisplayRole).toString());
    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_SHOW_NO_LICENSE_MESSAGE), item(0, OFSMODEL_COLUMN_SHOW_NO_LICENSE_MESSAGE)->data(Qt::DisplayRole).toString());
    writer.writeTextElement(getColumnName(OFSMODEL_COLUMN_UPDATES_ON_STARTUP), item(0, OFSMODEL_COLUMN_UPDATES_ON_STARTUP)->data(Qt::DisplayRole).toString());
    writer.writeEndElement();

	
    writer.writeEndElement();
    writer.writeEndDocument();
    return true;
}


/// @author Sylvana Alpert
/// @date   July 9, 2015
/// @brief  Get the contents of the model from XML
/// @param [in] dir of the file that will hold the XML
/// @return TRUE if success; FALSE otherwise
bool ConfigModel::getFromXML(QString dir)
{
    beginResetModel();
    blockSignals(true);

    dir = dir + "/configModel.xml";

    QFile file(dir);

    if (!file.exists() || !file.open(QFile::ReadOnly | QIODevice::Text))
        return false;

    QXmlStreamReader reader(&file);
    QStringRef name;
    QStringList colors;
    QStringList files;

    while (!reader.atEnd())
    {
        reader.readNextStartElement();
        name = reader.name();
        if (name == "BOSS")
            continue;
        else if (name == "File")
        {
            while (reader.readNextStartElement())
            {
                name = reader.name();
                if (name == getColumnName(OFSMODEL_COLUMN_LAST_PATH))
                    setData(index(0, OFSMODEL_COLUMN_LAST_PATH), reader.readElementText());

                else if (name == getColumnName(OFSMODEL_COLUMN_RECENT_FILES))
                {
                    while (reader.readNextStartElement())
                    {
                        name = reader.name();
                        if (name == "file")
                            files << reader.readElementText();
                    }
                }

            }
        }
        else if (name == "Display")
        {
            while (reader.readNextStartElement())
            {
                name = reader.name();
                if (name == getColumnName(OFSMODEL_COLUMN_UNIT_COLORS))
                {
                    while (reader.readNextStartElement())
                    {
                        name = reader.name();
                        if (name == "color")
                            colors << reader.readElementText();
                    }
                }                
                else if (name == getColumnName(OFSMODEL_COLUMN_POINT_SIZE))
                    setData(index(0, OFSMODEL_COLUMN_POINT_SIZE), reader.readElementText());
                else if (name == getColumnName(OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES))
                    setData(index(0, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES), reader.readElementText());
                else if (name == getColumnName(OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS))
                    setData(index(0, OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS), reader.readElementText());
                else if (name == getColumnName(OFSMODEL_COLUMN_APPLY_TO_ALL_SELECTED_CHANNELS))
                    setData(index(0, OFSMODEL_COLUMN_APPLY_TO_ALL_SELECTED_CHANNELS), reader.readElementText());
            }
        }
        else if (name == "Preferences")
        {
            while (reader.readNextStartElement())
            {
                name = reader.name();
                if (name == getColumnName(OFSMODEL_COLUMN_DELETION_CHANNEL_FILES))
                    setData(index(0, OFSMODEL_COLUMN_DELETION_CHANNEL_FILES), reader.readElementText());
                else if (name == getColumnName(OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION))
                    setData(index(0, OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION), reader.readElementText());
                else if (name == getColumnName(OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION))
                    setData(index(0, OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION), reader.readElementText());
                else if (name == getColumnName(OFSMODEL_COLUMN_SHOW_NO_LICENSE_MESSAGE))
                    setData(index(0, OFSMODEL_COLUMN_SHOW_NO_LICENSE_MESSAGE), reader.readElementText());

                else if (name == getColumnName(OFSMODEL_COLUMN_UPDATES_ON_STARTUP))
                    setData(index(0, OFSMODEL_COLUMN_UPDATES_ON_STARTUP), reader.readElementText());
            }
        }        
    }

    setData(index(0, OFSMODEL_COLUMN_RECENT_FILES), files);
    setData(index(0, OFSMODEL_COLUMN_UNIT_COLORS), colors);
    setColors();

    blockSignals(false);
    endResetModel();

    return true;

}

/// @author Sylvana Alpert
/// @date   July 9, 2015
/// @brief  get the name of the column with no spaces or -'s which are illegal XML characters
/// @param [in] column
/// @return name
QString ConfigModel::getColumnName(ConfigModelColumn column)
{
    QString name = ColumnNames.at(column);
    name = name.replace(QChar(' '), QString("_"));    
    return name;
}
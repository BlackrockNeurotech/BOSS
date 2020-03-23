#include "loadparametersdialog.h"
#include "ui_loadParametersDialog.h"
#include <qstringlist.h>
#include <qabstractbutton.h>


LoadParametersDialog::LoadParametersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadParametersDialog),
    m_selection(LOADPARAMETERS_NONE)
{
    ui->setupUi(this);
    ui->buttonGroup->setId(ui->radioButtonRange, LOADPARAMETERS_RANGE);
    ui->buttonGroup->setId(ui->radioButtonAll, LOADPARAMETERS_ALL);
    connect(ui->radioButtonRange, SIGNAL(toggled(bool)), this, SLOT(slotEnableRangeInput(bool)));
    connect(this, SIGNAL(accepted()), this, SLOT(slotGetSelections()));
}

LoadParametersDialog::~LoadParametersDialog()
{
    delete ui;
}


QList<int>  LoadParametersDialog::getRange()
{
    if (m_range.isEmpty())
        return QList<int>();
    
    // Remove white spaces
    m_range.remove(" "); 

    // Split
    QStringList strList = m_range.split(",", QString::SkipEmptyParts);

    // Iterate and get ranges
    QList<int> list;
    foreach (QString str, strList)
    {
        if (str.contains("-"))
        {
            // This is a range
            QStringList range = str.split("-", QString::SkipEmptyParts);
            if (range.size() == 2)
            {
                int rangeStart, rangeEnd;
                bool ok1, ok2;
                rangeStart = range[0].toInt(&ok1);
                rangeEnd   = range[1].toInt(&ok2);
                if (ok1 && ok2)
                {
                    for (int i = rangeStart; i <= rangeEnd; ++i)
                        list.push_back(i);
                }
            }
        }
        else
        {
            // Single channel 
            bool ok;
            int value = str.toInt(&ok);
            if (ok)
                list.push_back(value);
        }
    }

    return list;
}

void LoadParametersDialog::slotGetSelections()
{
    m_range = ui->rangeInput->text();

    m_selection = (LoadParametersSelection) ui->buttonGroup->checkedId();

    if (m_selection == -1)
        m_selection = LOADPARAMETERS_NONE;
    else if (m_selection == LOADPARAMETERS_ALL)
        m_range.clear();
    else if (m_selection == LOADPARAMETERS_RANGE && m_range.isEmpty())
        m_selection = LOADPARAMETERS_NONE;

}

void LoadParametersDialog::slotEnableRangeInput(bool checked)
{
    ui->rangeInput->setEnabled(checked);

    if (!checked)
        ui->rangeInput->clear();
}
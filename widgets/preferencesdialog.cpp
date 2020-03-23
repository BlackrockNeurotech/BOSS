#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    m_bShowMaxSamples = true;

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));

    // Initialize with data from model
    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ConfigModel::instance());
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(ui->spinBoxNSamples, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES);
    m_mapper->addMapping(ui->spinBoxNWaveforms, OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS);

    m_mapper->addMapping(ui->checkBoxDeleteFiles, OFSMODEL_COLUMN_DELETION_CHANNEL_FILES);
    m_mapper->addMapping(ui->checkBoxContinuePrevSession, OFSMODEL_COLUMN_CONTINUE_PREV_EDITING_SESSION);
    m_mapper->addMapping(ui->checkBoxAskPrevSession, OFSMODEL_COLUMN_ASK_TO_CONTINUE_PREV_EDITING_SESSION);

    m_mapper->addMapping(ui->checkBoxUpdatesOnStartup, OFSMODEL_COLUMN_UPDATES_ON_STARTUP);
    m_mapper->toFirst();

    connect(ui->checkBoxDeleteFiles, SIGNAL(stateChanged(int)), this, SLOT(slotToggleCheckBoxDeleteFiles(int)));
    connect(ui->checkBoxContinuePrevSession, SIGNAL(stateChanged(int)), this, SLOT(slotToggleCheckBoxContinuePrevSession(int)));
    connect(ui->checkBoxAskPrevSession, SIGNAL(stateChanged(int)), this, SLOT(slotToggleCheckBoxAskPrevSession(int)));
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
    delete m_mapper;
}

void PreferencesDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    ui->spinBoxNSamples->setVisible(m_bShowMaxSamples);
    ui->labelContinuousSamples->setVisible(m_bShowMaxSamples);
}

void PreferencesDialog::slotAccept()
{
    m_mapper->submit();

}

void PreferencesDialog::slotToggleCheckBoxDeleteFiles(int state)
{
    if (state == Qt::Unchecked)
    {
        ui->checkBoxContinuePrevSession->setEnabled(true);
        ui->checkBoxAskPrevSession->setEnabled(true);
        ui->checkBoxContinuePrevSession->setChecked(false);
        ui->checkBoxAskPrevSession->setChecked(true);
    }
    else if (state == Qt::Checked)
    {
        ui->checkBoxContinuePrevSession->setChecked(false);
        ui->checkBoxAskPrevSession->setChecked(false);
        ui->checkBoxContinuePrevSession->setEnabled(false);
        ui->checkBoxAskPrevSession->setEnabled(false);
    }
}

void PreferencesDialog::slotToggleCheckBoxContinuePrevSession(int state)
{
    if (state == Qt::Checked)
    {
        ui->checkBoxAskPrevSession->setChecked(false);
    }
}

void PreferencesDialog::slotToggleCheckBoxAskPrevSession(int state)
{
    if (state == Qt::Checked)
    {
        ui->checkBoxContinuePrevSession->setChecked(false);
    }
}
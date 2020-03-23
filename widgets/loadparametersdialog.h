#ifndef LOADPARAMETERSDIALOG_H
#define LOADPARAMETERSDIALOG_H

#include <QDialog>
#include <QList>

namespace Ui {
class LoadParametersDialog;
}

enum LoadParametersSelection
{
    LOADPARAMETERS_ALL = 0,
    LOADPARAMETERS_RANGE,
    LOADPARAMETERS_NONE
};

class LoadParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadParametersDialog(QWidget *parent = 0);
    ~LoadParametersDialog();

    LoadParametersSelection getSelection() { return m_selection; }
    QList<int>              getRange();


private:
    Ui::LoadParametersDialog *ui;
    QString m_range;
    LoadParametersSelection m_selection;

public slots: 
    void slotGetSelections();
    void slotEnableRangeInput(bool);
    
};

#endif // LOADPARAMETERSDIALOG_H

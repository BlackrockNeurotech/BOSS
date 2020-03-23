#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QDataWidgetMapper>
#include "../model/configmodel.h"
#include "../model/mapperwidgets/radiogroup.h"

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    void ShowMaxSamples(bool show = true){ m_bShowMaxSamples = show; } // if true (continuous data file) the user can set the max samples to display

protected:
    virtual void	showEvent(QShowEvent *event);

private:
    Ui::PreferencesDialog *ui;
    QDataWidgetMapper * m_mapper;
    bool m_bShowMaxSamples;

public slots:
    void slotAccept();
    void slotToggleCheckBoxDeleteFiles(int state);
    void slotToggleCheckBoxContinuePrevSession(int state);
    void slotToggleCheckBoxAskPrevSession(int state);
};

#endif // PREFERENCESDIALOG_H

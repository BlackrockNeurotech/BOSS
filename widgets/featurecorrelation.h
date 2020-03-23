#ifndef FEATURECORRELATION_H
#define FEATURECORRELATION_H

#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDataWidgetMapper>
#include <armadillo>

#include "../model/channelconfigmodel.h"
#include "../model/mapperwidgets/checkboxgroup.h"


class FeaturePushButton : public QPushButton
{
    Q_OBJECT

public:
    FeaturePushButton(QWidget *parent, int row, int col, QColor color, bool bSquare = true);
    ~FeaturePushButton();

    void setColor(QColor color);
    int row() { return m_row; }
    int col() { return m_col; }
    void setDimensions(int width, int height);
    //virtual bool hasHeightForWidth() const { return true; }
    //virtual int heightForWidth(int width) const;


protected:
    virtual void paintEvent (QPaintEvent *event);

private:
    int m_row;
    int m_col;
    QColor m_color;
    QColor m_colorDark;
    QColor m_colorHover;
    QColor m_colorHoverDark;
    QColor m_colorHoverSelected;
    QColor m_colorHoverSelectedDark;
    QColor m_colorGreen;
    QColor m_colorGreenDark;
    bool   m_bSquare;
public slots:
    void slotTriggered(bool checked);

signals:
    void triggered(bool checked, int row, int col);
};



class FeatureCorrelation : public QWidget
{
    Q_OBJECT

public:
    FeatureCorrelation(QWidget *parent = 0);

    virtual bool hasHeightForWidth() const { return true; }
    virtual int heightForWidth(int width) const;

    CheckBoxGroup *m_checkboxGroup;

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void resizeEvent(QResizeEvent * event);

public slots:
    void slotChannelChanged(int channel);

    void slotTriggered(bool checkState, int row, int col);
    void slotCorrMatrixReady(int channel);

private:
    QGridLayout *m_mainLayout;

    QList<QCheckBox*>          m_features;               // the checkboxes for all possible features
    QList<FeaturePushButton*> m_selections;         // the buttons for all possible feature combinations

    void initializeFeatures();
    void initializeSelections();
	void initializeSizes();
	void clearCurrentFeatures();
    void clearCurrentSelections();
    void populateFeatures();
    void populateSelections();
    void selectCurrentSelections();
    void saveCurrentSelections();
};


#endif // FEATURECORRELATION_H

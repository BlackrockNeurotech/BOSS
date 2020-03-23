
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtWidgets>
#include <QtOpenGL/QGLWidget>
#include <QImage>
#include <QPainterPath>
#include <QStandardItem>


#include "../model/channelconfigmodel.h"
#include "../backend/dataprocessing/continuous/timescan.h"


class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    int xRotation() const { return m_xRot; }
    int yRotation() const { return m_yRot; }
    int zRotation() const { return m_zRot; }

    //virtual bool hasHeightForWidth() const { return true; }
    //virtual int  heightForWidth(int w) const { return w; }
    QSize sizeHint() const { return this->size(); }

    void sort(QVector<quint32>);

public slots:
    void slotResetRotation();
    void slotFeaturesChanged(int channel);
    void slotVisibleUnitsChanged(int channel, QStringList visibleUnits);
    void slotAxisChanged(int channel, ChannelColumn axis, FeatureID newFeature);
    void slotGlEditStateChanged(int channel, GLWidgetState editState);
    void slotChannelChanged(int channel);
    void slotSelectionChanged(int channel);
    void setAxisFeature(int axis, FeatureID id);
    void slotPointSizeChanged(int size);

protected:
    void initializeGL();
    void paintEvent(QPaintEvent *event);
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void initializeData();

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    void setupViewport(int width, int height);
    void GlDrawCoordinateAxes(int size);
    void line(QColor color, GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2);
    void point(QColor color, GLdouble x, GLdouble y, GLdouble z);
    void centroid(GLdouble x, GLdouble y, GLdouble z);
    void drawPoints();
    void loadGLTextures(QString image, int index);
    void drawLabels();
    void drawCoordinateAxes();
	int unitNumberFromKey(int key);

    int m_xPos;
    int m_yPos;
    int m_xRot;
    int m_yRot;
    int m_zRot;
    qreal m_pointSize;
    qreal m_scale;
    QPoint m_lastPos;
    bool   m_bMouseMoved;
    bool   m_bCtrl;

    QList<QColor>           m_unitColors;
    QVector<quint32>        m_selected;
    QList<QString>          m_resourceNames;
    QList<SdFeaturePktHdr>  m_features;
    int m_Xmax;
    int m_Ymax;
    int m_Zmax;
    int m_max;

    FeatureID m_featureX;
    FeatureID m_featureY;
    FeatureID m_featureZ;
    FeatureID getFeatureID(int axis);

    QColor    m_xColor;
    QColor    m_yColor;
    QColor    m_zColor;

    GLuint textures[FEATUREID_END];

    GLWidgetState m_selectionState;
    QPainterPath *m_selectionPath;

    bool processSingleSelection(int x, int y, bool clear);
    bool processSelection(int x, int y, int width, int height, bool clear);
    void updateCentroids(FeaturesAccum featuresData, QVector<quint32> &centroids, bool clear);
    void renderSelection(int size);
    bool wfIDValid(quint32 wfid);
    void indexToRGB(quint32 wfid, GLbyte &r, GLbyte &g, GLbyte &b);
    quint32 RGBToIndex(unsigned char r, unsigned char g, unsigned char b);
};

#endif // GLWIDGET_H

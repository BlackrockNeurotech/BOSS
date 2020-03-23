
#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <qopenglfunctions.h>
#include <QtWidgets>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QImage>
#include <QPainterPath>
#include <QStandardItem>
#include <qmatrix4x4.h>


#include "../model/channelconfigmodel.h"
#include "../backend/dataprocessing/continuous/timescan.h"

const int AXES_POINTS_PROGRAM = 0;
const int TEXTURE_PROGRAM = 1;
const int LABEL_OFFSET = 0;
const int AXES_OFFSET = 3;
const int POINTS_OFFSET = 6;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
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
//    QSize sizeHint() const { return this->size(); }
    QSize minimumSizeHint() const { return QSize(20, 20); }

    void sort(QVector<quint32>);

public slots:
    void slotResetRotation();
    void slotFeaturesChanged(int channel);
    void slotVisibleUnitsChanged(int channel, QStringList visibleUnits);
    void slotAxisChanged(int channel, ChannelColumn axis, FeatureID newFeature);
    void slotGlEditStateChanged(int channel, GLWidgetState editState);
    void slotChannelChanged(int channel);
    void slotSelectionChanged(int channel);
    void slotPointSizeChanged(int size);

protected:
    void initializeGL();
	void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void initializeData();

    void initLabels();
    void initAxes();

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    void setupViewport(int width, int height);
    void GlDrawCoordinateAxes(int size);
    void centroid(GLdouble x, GLdouble y, GLdouble z);
    void setPoints();
    void drawPoints();
    void drawLabels();
    void drawCoordinateAxes();
    void drawCrossHairs();      // This function assumes the the right program has been bound
	int  unitNumberFromKey(int key);
	void setAxisFeature(int axis, FeatureID id);

    int m_xPos;
    int m_yPos;
    int m_xRot;
    int m_yRot;
    int m_zRot;
    qreal m_pointSize;
    qreal m_scale;
    QPoint m_lastPos;
    bool   m_bCtrl;
    bool   m_bMouseMoved;

    QVector<quint32>        m_selected;
    QVector<quint32>        m_centroids;
    QList<QString>          m_resourceNames;
    QList<SdFeaturePktHdr>  m_features;
    int m_Xmax;
    int m_Ymax;
    int m_Zmax;
    int m_max;
	GLint m_viewport[4];

    FeatureID m_featureX;
    FeatureID m_featureY;
    FeatureID m_featureZ;
    FeatureID getFeatureID(int axis);

    QColor    m_xColor;
    QColor    m_yColor;
    QColor    m_zColor;



    QOpenGLTexture* textures[FEATUREID_END];
    QOpenGLShaderProgram *m_program[2]; // 0-Axis Labels, 1-Axis and Data points
    QOpenGLVertexArrayObject *m_vao[6]; // 0-2 Axis labels, 3-5-Axis
    GLuint m_vbo[7];                    // 0-2 Axis labels, 3-5-Axis, 6-Points
    GLuint m_ebo[6];                    // 0-2 Axis labels, 3-5-Axis 
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    int m_mvMatrixLoc[2];
    int m_modifierMatrixLoc;
    QMutex  m_mutexPoints;

    GLWidgetState m_selectionState;
    QPainterPath *m_selectionPath;
    QPainterPath m_adjustedSelectionPath;

    bool processSingleSelection(int x, int y, bool clear);
    bool processSelection(int x, int y, int width, int height, bool clear);
	void updateCentroids(FeaturesAccum featuresData, QVector<quint32> &centroids, bool clear);
	void renderSelection(int size);
	bool wfIDValid(quint32 wfid);
    void indexToRGB(quint32 wfid, GLbyte &r, GLbyte &g, GLbyte &b);
    quint32 RGBToIndex(unsigned char r, unsigned char g, unsigned char b);
};


#endif // GLWIDGET_H

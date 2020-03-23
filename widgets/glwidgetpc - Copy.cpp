#include "glwidgetpc.h"

#include <QMouseEvent>
#include <QTime>
#include <QGLShader>
#include <QVector3D>

#include <QtWidgets>
#include <QtOpenGL>

#include <math.h>
#include <stdlib.h>
#include "../model/configmodel.h"
#include "../widgets/editmenu.h"
#include "../backend/dataprocessing/common/unitassignment.h"

//#ifndef GL_MULTISAMPLE
//#define GL_MULTISAMPLE  0x809D
//#endif


#define Pi 3.14159265359


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    slotResetRotation();

    m_Xmax = 0;
    m_Ymax = 0;
    m_Zmax = 0;
    m_max = 0;
    m_scale = 1.0;

    m_xColor = Qt::red;
    m_yColor = Qt::green;
    m_zColor = Qt::blue;

    m_selectionPath = 0;
    m_selectionState = GLWIDGETSTATE_ROTATE;

    m_featureX = FEATUREID_PCA1;
    m_featureY = FEATUREID_PCA2;
    m_featureZ = FEATUREID_PCA3;

    setWindowTitle(tr("3D Clusters"));

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(featuresChanged(int)), this, SLOT(slotFeaturesChanged(int)));
    connect(ccm, SIGNAL(waveformSelectionChanged(int )), this, SLOT(slotSelectionChanged(int )));

    ConfigModel *cm = ConfigModel::instance();
    QStringList colors = cm->item(0, OFSMODEL_COLUMN_UNIT_COLORS)->data(Qt::DisplayRole).toStringList();
    connect(cm, SIGNAL(pointSizeChanged(int)), this, SLOT(slotPointSizeChanged(int)));

    for (int i = 0; i < colors.size(); ++i)
    {
        m_unitColors.push_back(QColor(colors[i]));
    }
	m_pointSize = .8 + cm->item(0, OFSMODEL_COLUMN_POINT_SIZE)->data(Qt::DisplayRole).toInt();

    // this is needed for keyboard input
    setFocusPolicy(Qt::StrongFocus);

    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA1.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA2.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA3.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA4.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA5.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA6.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA7.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA8.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA9.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PCA10.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PeakValue.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/ValleyValue.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/TimeToMax.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/TimeToMin.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/Area.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/PeakFWHM.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/ValleyFWHM.png");
    m_resourceNames.push_back(":/new/Bitmaps/Bitmaps/Time.png");

    setAutoFillBackground(false);
}


GLWidget::~GLWidget()
{
    if (m_selectionPath != 0)
        delete m_selectionPath;
}

/*
 * Helper function to normalize rotation angles
 * (basically we don't want these exceeding 359 DEG.)
 */
static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    m_xRot = angle;
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    m_yRot = angle;
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    m_zRot = angle;
}

void GLWidget::initializeData()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();

    //set the axis
    setAxisFeature(0, (FeatureID)ccm->data(ccm->index(channel, CC_X_3DAXIS), Qt::UserRole).toInt());
    setAxisFeature(1, (FeatureID)ccm->data(ccm->index(channel, CC_Y_3DAXIS), Qt::UserRole).toInt());
    setAxisFeature(2, (FeatureID)ccm->data(ccm->index(channel, CC_Z_3DAXIS), Qt::UserRole).toInt());

    // set the scale
    m_Xmax = 100;
    m_Ymax = 100;
    m_Zmax = 100;
    m_max = 100;

    // set the glmode - rotate/edit/edit once/centroids
    m_selectionState = (GLWidgetState)ccm->data(ccm->index(channel, CC_3D_EDIT_STATE), Qt::UserRole).toInt();
}

void GLWidget::initializeGL()
{
    for (int i = 0; i < m_resourceNames.size(); i++)
        loadGLTextures(m_resourceNames.at(i), i);

    qglClearColor(Qt::black);
    glClearDepth(1.0f);                                // Depth Buffer Setup

    initializeData();

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(ccm, SIGNAL(visibleUnitsChanged(int, QStringList)), this, SLOT(slotVisibleUnitsChanged(int, QStringList)));
    connect(ccm, SIGNAL(threeDAxisChanged(int, ChannelColumn, FeatureID)), this, SLOT(slotAxisChanged(int, ChannelColumn, FeatureID)));
    connect(ccm, SIGNAL(glEditStateChanged(int, GLWidgetState)), this, SLOT(slotGlEditStateChanged(int, GLWidgetState)));
}

void GLWidget::loadGLTextures(QString image, int index)
{
    QImage b;

    if (!b.load(image))
    {
        b.fill(qRgba(0, 0, 0, 0));
    }
    QImage img = QGLWidget::convertToGLFormat(b);
    textures[index] = this->bindTexture(img, GL_TEXTURE_2D, GL_RGBA, QGLContext::DefaultBindOption);
}

void GLWidget::drawLabels()
{
    glPushMatrix();
    glLoadIdentity();
    qreal z = m_max * 20;

    glTranslatef(0.0, 0.0, -z);

    int xRotation = m_xRot / 16.0;
    int yRotation = m_yRot / 16.0;
    int zRotation = m_zRot / 16.0;

    glRotatef(xRotation, 1.0, 0.0, 0.0);
    glRotatef(yRotation, 0.0, 1.0, 0.0);
    glRotatef(zRotation, 0.0, 0.0, 1.0);

    qreal scale = .2 *m_scale;
    glScalef(scale, scale, scale);
    qreal x = (m_max * 3.5) + (1 / m_scale);

    glEnable(GL_TEXTURE_2D);                    // Enable Texture Mapping
    glEnable(GL_BLEND);                         // Enable Blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QColor black(Qt::black);
    black.setAlpha(0);
    qglClearColor(black);

    qglColor(m_xColor);
    this->drawTexture(QPointF(x, 0), textures[(int)getFeatureID(0)]);

    glRotatef(90, 0.0, 0.0, 1.0);
    qglColor(m_yColor);
    this->drawTexture(QPointF(x, 0), textures[(int)getFeatureID(1)]);

    glRotatef(90, 0.0, 1.0, 0.0);
    qglColor(m_zColor);
    this->drawTexture(QPointF(x, 0), textures[(int)getFeatureID(2)]);

    glDisable(GL_TEXTURE_2D);                    // Only Enable Texture Mapping for the labels
    glDisable(GL_BLEND);

    glPopMatrix();
}

void GLWidget::paintEvent(QPaintEvent *event)
{
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glEnable(GL_MULTISAMPLE);

    qglClearColor(Qt::black);
    qreal pixelRatio = devicePixelRatio();
    setupViewport(width() * pixelRatio, height() * pixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    qreal z = m_max * 20;

    glTranslatef(0.0, 0.0, -z);

    glRotatef(m_xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(m_yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(m_zRot / 16.0, 0.0, 0.0, 1.0);

    glScalef(m_scale, m_scale, m_scale);

    drawCoordinateAxes();
    drawPoints();
    drawLabels();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    if (m_selectionPath == 0)
    {
        // QPainter calls swapBuffers, so this call is not needed if overpainting
        swapBuffers();
    }
    else
    {
        QPainter painter(this);
        painter.save();
        painter.setPen(QPen(Qt::white));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawPath(*m_selectionPath);
        painter.restore();
        painter.end();
    }
}

void GLWidget::drawCoordinateAxes()
{
        glLineWidth(1.0);
        glLineStipple(1, 0x0F0F);
        glEnable(GL_LINE_STIPPLE);
        qreal size = m_max * 1.1;

        // x-axis
        line(m_xColor, 0, 0, 0, size, 0, 0);
        line(m_xColor.dark(), 0, 0, 0, -size, 0, 0);
        // y-axis
        line(m_yColor, 0, 0, 0, 0, size, 0);
        line(m_yColor.dark(), 0, 0, 0, 0, -size, 0);
        // z-axis
        line(m_zColor, 0, 0, 0, 0, 0, size);
        line(m_zColor.dark(), 0, 0, 0, 0, 0, -size);

        glDisable(GL_LINE_STIPPLE);
}

void GLWidget::line(QColor color, GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2)
{
    qglColor(color);
    glBegin(GL_LINES);
    {
        glVertex3d(x1, y1, z1);
        glVertex3d(x2, y2, z2);
    }
    glEnd();
}

void GLWidget::drawPoints()
{
	ChannelConfigModel * ccm = ChannelConfigModel::instance();
	int channel = ccm->CurrentChannel();
	FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();
    UnitAccum unitData = ccm->data(ccm->index(channel, CC_SPIKEDATA_UNITS), Qt::UserRole).value<UnitAccum>();

	if ((featuresData) && (unitData))
	{
        QVector<quint32> selectedWfIDs = featuresData->getSelectionIndex();
        QVector<quint32> wfIDs = featuresData->getWaveformIDs();
		QVector<int> units = featuresData->getAssignments();
		QVector<int> usedUnits = unitData->getUnitNumbers(false);
		glPointSize(m_pointSize);
		FeatureID featureX = getFeatureID(0);
		FeatureID featureY = getFeatureID(1);
		FeatureID featurez = getFeatureID(2);

		for (int i = 0; i < usedUnits.size(); i++)
		{
			int unit = usedUnits[i];
			int count = unitData->waveformCount(unit);

			if (!unitData->isVisible(unit) || (count == 0))
				continue;

			QColor color = ConfigModel::instance()->getColor(unit);
			qglColor(color);
			glBegin(GL_POINTS);
			{
				for (int i = 0; i < wfIDs.size(); i++)
				{
					if (units[i] == unit)
					{
						double vX = featuresData->getFeatureValueForIndex(i, featureX) * 100;
						double vY = featuresData->getFeatureValueForIndex(i, featureY) * 100;
						double vZ = featuresData->getFeatureValueForIndex(i, featurez) * 100;

						glVertex3d(vX, vY, vZ);
					}
				}
			}
			glEnd();
		}
		qglColor(Qt::white);
		glBegin(GL_POINTS);
		{
			for (int i = 0; i < wfIDs.size(); i++)
			{
				qint32 id = wfIDs[i];

				if (selectedWfIDs.contains(id))
				{
					selectedWfIDs.removeOne(id);
					double vX = featuresData->getFeatureValueForIndex(i, featureX) * 100;
					double vY = featuresData->getFeatureValueForIndex(i, featureY) * 100;
					double vZ = featuresData->getFeatureValueForIndex(i, featurez) * 100;

					glVertex3d(vX, vY, vZ);
				}
			}
		}
		glEnd();
		foreach(uint id, m_centroids)
		{
			double vX = featuresData->getFeatureValue(id, featureX) * 100;
			double vY = featuresData->getFeatureValue(id, featureY) * 100;
			double vZ = featuresData->getFeatureValue(id, featurez) * 100;

			centroid(vX, vY, vZ);
		}
	}
	//ChannelConfigModel * ccm = ChannelConfigModel::instance();
 //   int channel = ccm->CurrentChannel();
 //   FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();
 //   
 //   if (featuresData)
 //   {
 //       QVector<quint32> wfIDs = featuresData->getWaveformIDs();
 //       QVector<quint32> selectedWfIDs = ccm->data(ccm->index(channel, CC_WAVEFORM_SELECTION), Qt::UserRole).value<QVector<quint32> >();

 //       QColor color(Qt::green);
 //       if (m_unitColors.size() > 0)
 //           color = m_unitColors.at(0);

 //       glPointSize(m_pointSize);

 //       QVector<int> units = featuresData->getAssignments();

	//	for (int i = 0; i < wfIDs.size(); i++)
	//	{
	//		qint32 wfID = wfIDs[i];
	//		int unit = units[i];

 //           if (i < units.size())
 //           {
 //               unit = units.at(i);
 //           }
 //           if (unit < m_unitColors.size())
 //               color = m_unitColors.at(unit);

 //           if (selectedWfIDs.contains(wfID))
 //               color = Qt::white;

 //           double vX = featuresData->getFeatureValue(wfID, getFeatureID(0)) * 100;
 //           double vY = featuresData->getFeatureValue(wfID, getFeatureID(1)) * 100;
 //           double vZ = featuresData->getFeatureValue(wfID, getFeatureID(2)) * 100;

 //           if (m_centroids.contains(wfID))
 //           {
 //               centroid(vX, vY, vZ);
 //           }
 //           else
 //           {
 //               if (ccm->unitVisible(channel, unit))
 //                   point(color, vX, vY, vZ);
 //           }
 //       }
 //   }
}

void GLWidget::point(QColor color, GLdouble x, GLdouble y, GLdouble z)
{
    qglColor(color);
    glBegin(GL_POINTS);
    {
        glVertex3d(x, y, z);
    }
    glEnd();
}

void GLWidget::centroid(GLdouble x, GLdouble y, GLdouble z)
{
    glLineWidth(2.0);
    qreal size = m_max / 30;

    // x-axis
    line(m_xColor, x, y, z, x + size, y, z);
    line(m_xColor.dark(), x, y, z, x - size, y, z);
    // y-axis
    line(m_yColor, x, y, z, x, y + size, z);
    line(m_yColor.dark(), x, y, z, x, y - size, z);
    // z-axis
    line(m_zColor, x, y, z, x, y, z + size);
    line(m_zColor.dark(), x, y, z, x, y, z - size);
}

void GLWidget::resizeGL(int width, int height)
{
    qreal pixelRatio = devicePixelRatio();
    setupViewport(width *pixelRatio, height *pixelRatio);
}

void GLWidget::setupViewport(int width, int height)
{
//    int side = qMin(width, height);
//    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    qreal half = m_max;
    qreal nr = half * 8;
    qreal fr = half * 30;

#ifdef QT_OPENGL_ES
    glOrthof(-half, half, -half, half, nr, fr);
#else
    glOrtho(-half, half, -half, half, nr, fr);
#endif
    glMatrixMode(GL_MODELVIEW);
}


//
//  Slots
//
void GLWidget::slotFeaturesChanged(int channel)
{
    this->update();
}

void GLWidget::slotChannelChanged(int channel)
{
    initializeData();
    this->update();
}

void GLWidget::slotGlEditStateChanged(int channel, GLWidgetState editState)
{
    Q_UNUSED(channel);

    // clear the selected waveform variable because it interferes with centroids being selected
    if (editState == GLWIDGETSTATE_CENTROIDS)
        m_selected.clear();

    m_selectionState = editState;
    update();
}

void GLWidget::slotAxisChanged(int channel, ChannelColumn axis, FeatureID newFeature)
{
    Q_UNUSED(channel);

    switch (axis)
    {
    case CC_X_3DAXIS:
        setAxisFeature(0, newFeature);
        break;
    case CC_Y_3DAXIS:
        setAxisFeature(1, newFeature);
        break;
    case CC_Z_3DAXIS:
        setAxisFeature(2, newFeature);
        break;
    }
}

void GLWidget::slotVisibleUnitsChanged(int channel, QStringList visibleUnits)
{
    Q_UNUSED(channel);
    update();
}

void GLWidget::slotSelectionChanged(int channel)
{
    ChannelConfigModel * ccm = ChannelConfigModel::instance();
    FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();

    if (featuresData)
    {
        QVector<quint32> m_selected = featuresData->getSelectionIndex();
        this->update();
    }
}

void GLWidget::slotResetRotation()
{
    m_xRot = 0;
    m_yRot = 0;
    m_zRot = 0;
    update();
}

void GLWidget::slotPointSizeChanged(int size)
{
    m_pointSize = size + .8;
    update();
}

void GLWidget::setAxisFeature(int axis, FeatureID id)
{
    switch (axis)
    {
    case 0:
        m_featureX = id;
        break;
    case 1:
        m_featureY = id;
        break;
    case 2:
        m_featureZ = id;
        break;
    }    
    this->update();
}

// return the Feature ID assignaed to the given axis
FeatureID GLWidget::getFeatureID(int axis)
{
    FeatureID id;

    switch (axis)
    {
    case 0:
        id = m_featureX;
        break;
    case 1:
        id = m_featureY;
        break;
    case 2:
        id = m_featureZ;
        break;
    }
    return id;
}


// call the selection rendering routine.
// Reads the pixel from the back buffer.
// The returned color is the index.
bool GLWidget::processSingleSelection(int x, int y, bool clear)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    QVector<quint32> *list = &m_selected;
	FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();

	if (featuresData)
	{
		QVector<int> units = featuresData->getAssignments();
		QVector<quint32> wfIDs = featuresData->getWaveformIDs();

		if (m_selectionState == GLWIDGETSTATE_CENTROIDS)
			list = &m_centroids;

		if (clear)
		{
			list->clear();
		}
        QVector<quint32> selectionIndex = *list;

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		unsigned char res[4];
		renderSelection(1);
		glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, res);
		quint32 index = RGBToIndex(res[0], res[1], res[2]);

		if (index < units.size())
		{
			if (ccm->unitVisible(channel, units.at(index)))
			{
				quint32 wfID = wfIDs[index];
                list->push_back(index);
                selectionIndex.push_back(index);
			}
		}
		if (m_selectionState != GLWIDGETSTATE_CENTROIDS)
		{
            featuresData->setSelectionIndex(selectionIndex);
        }
		else
		{
			ccm->setData(ccm->index(channel, CC_NBR_SORT_UNITS), QVariant(m_centroids.size()), Qt::UserRole);
		}
        return (selectionIndex.size() > 0) ? 1 : 0;
    }
	return 0;
}

// call the selection rendering routine.
// Reads the pixel from the back buffer.
// The returned color is the index.
bool GLWidget::processSelection(int x, int y, int width, int height, bool clear)
{
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	int channel = ccm->CurrentChannel();
	FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();

	if (featuresData)
	{
		QVector<int> units = featuresData->getAssignments();
		QVector<quint32> wfIDs = featuresData->getWaveformIDs();
		QVector<quint32> *list = &m_selected;

		if (m_selectionState == GLWIDGETSTATE_CENTROIDS)
			list = &m_centroids;

		if (clear)
			list->clear();

        QVector<quint32> selectionIndex = *list;
		quint8 r = 0;
		quint8 g = 0;
		quint8 b = 0;

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		unsigned char *res = new unsigned char[5 * width * height];

		while (true)
		{
			bool finished = true;

			renderSelection(width * height);
			glReadPixels(x, viewport[3] - y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, res);
			unsigned char *resp = res;

			for (int j = 0; j < height; j++)
			{
				for (int i = 0; i < width; i++)
				{
					r = *resp++;
					g = *resp++;
					b = *resp++;
					resp++;

					if ((r != 0xff) || (g != 0xff) || (b != 0xff))
					{
						QPointF p(x + i, y - j);

						if (m_selectionPath->contains(p))
						{
							quint32 index = RGBToIndex(r, g, b);

							if (index < units.size())
							{
								if (ccm->unitVisible(channel, units.at(index)))
								{
									quint32 wfID = wfIDs[index];
                                    list->push_back(index);
                                    selectionIndex.push_back(index);
                                    finished = false;
								}
							}
						}
					}
				}
			}
			if (finished)
				break;
		}
		delete res;
		ChannelConfigModel * ccm = ChannelConfigModel::instance();
		int channel = ccm->CurrentChannel();

		if (m_selectionState != GLWIDGETSTATE_CENTROIDS)
		{
            featuresData->setSelectionIndex(selectionIndex);
        }
		else
		{
            updateCentroids(featuresData, m_centroids, clear);
        }
        return (selectionIndex.size() > 0) ? 1 : 0;
    }
	return 0;
}

// update the centroid features in the model
void GLWidget::updateCentroids(FeaturesAccum featuresData, QVector<quint32> &centroids, bool clear)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    ClusterDataPtr clusterData = ccm->data(ccm->index(channel, CC_CENTROIDS), Qt::UserRole).value<ClusterDataPtr>();

    if (clear)
        clusterData->clearClusters();

    clusterData->addClusters(featuresData, centroids);
}

// render the selection in the frame buffer using indexes for color data
void GLWidget::renderSelection(int size)
{
    glMatrixMode(GL_MODELVIEW);
    qreal pixelRatio = devicePixelRatio();
    setupViewport(width() * pixelRatio, height() * pixelRatio);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glDisable(GL_MULTISAMPLE);  // don't use multiple fragment samples in computing the final color of a pixel
    qreal half = m_max;
    qreal z = half * 20;

    glTranslatef(0.0, 0.0, -z);

    glRotatef(m_xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(m_yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(m_zRot / 16.0, 0.0, 0.0, 1.0);

    glScalef(m_scale, m_scale, m_scale);

    // drawPoints

    if (size == 1)
        glPointSize(6.0);
    else
        glPointSize(1.5);

    ChannelConfigModel * ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();
	FeatureID featureX = getFeatureID(0);
	FeatureID featureY = getFeatureID(1);
	FeatureID featurez = getFeatureID(2);

    if (featuresData)
    {
        QVector<quint32> wfIDs = featuresData->getWaveformIDs();

		glBegin(GL_POINTS);
		{
			for (int i = 0; i < wfIDs.size(); i++)
			{
				qint32 wfID = wfIDs[i];

				if (!m_selected.contains(wfID) && !m_centroids.contains(wfID))
				{
					GLdouble vX = (GLdouble)featuresData->getFeatureValueForIndex(i, featureX) * 100;
					GLdouble vY = (GLdouble)featuresData->getFeatureValueForIndex(i, featureY) * 100;
					GLdouble vZ = (GLdouble)featuresData->getFeatureValueForIndex(i, featurez) * 100;
					GLbyte r = 0;
					GLbyte g = 0;
					GLbyte b = 0;
					indexToRGB(i, r, g, b);
					glColor3b(r, g, b);
					glVertex3d(vX, vY, vZ);
				}
			}
		}
		glEnd();
		glEnable(GL_MULTISAMPLE);
        qglClearColor(Qt::black);
    }
}

// split the waveform ID into color data
void GLWidget::indexToRGB(quint32 wfid, GLbyte &r, GLbyte &g, GLbyte &b)
{

    unsigned char cr, cg, cb;

    cr = wfid & 0x7f;
    wfid >>= 7;
    cg = wfid & 0x7f;
    wfid >>= 7;
    cb = wfid & 0x7f;
    r = cr;
    g = cg;
    b = cb;
}

// concatenate the color data into a waveform ID
quint32 GLWidget::RGBToIndex(unsigned char r, unsigned char g, unsigned char b)
{
    // the data comes back from the frame buffer shifted left, so shift it right
    r >>= 1;
    g >>= 1;
    b >>= 1;

    unsigned int wfid = b;
    wfid <<= 7;
    wfid |= g;
    wfid <<= 7;
    wfid |= r;

    return (quint32)wfid;
}

// is the given waveform ID valid
bool GLWidget::wfIDValid(quint32 wfid)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();

    if (featuresData)
    {      
        QVector<quint32> wfIDs = featuresData->getWaveformIDs();
        QVector<int> units = featuresData->getAssignments();

        int index = wfIDs.indexOf(wfid);

        if ((index != -1) && (index < units.size()))
        {
            if (ccm->unitVisible(channel, units.at(index)))
                return true;
        }
    }
    return false;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();

    if (event->buttons() & Qt::LeftButton)
    {
        if (m_selectionPath != 0)
            delete m_selectionPath;
        m_selectionPath = 0;

        if ((m_selectionState == GLWIDGETSTATE_EDIT))
        {
            m_selectionPath = new QPainterPath(event->pos());

            bool ctrl = event->modifiers() & Qt::ControlModifier;
            processSingleSelection(m_lastPos.x(), m_lastPos.y(), !ctrl);
        }
        else if (m_selectionState == GLWIDGETSTATE_CENTROIDS)
        {
            bool ctrl = event->modifiers() & Qt::ControlModifier;
            processSingleSelection(m_lastPos.x(), m_lastPos.y(), !ctrl);
        }
    }
    else if (event->buttons() & Qt::RightButton)
    {
        if (m_selectionPath != 0)
        {
            delete m_selectionPath;
            m_selectionPath = 0;
        }
        EditMenu menu(this);
        if(!menu.isEmpty())
            menu.exec(QCursor::pos());
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_selectionState == GLWIDGETSTATE_EDIT)
    {
        if (event->buttons() & Qt::LeftButton)
        {
            if (m_selectionPath != 0)
            {
				if (this->rect().contains(event->pos()))
					m_selectionPath->lineTo(event->pos());
            }
        }
        else if (event->buttons() & Qt::RightButton)
        {
            if (m_selectionPath != 0)
            {
                delete m_selectionPath;
                m_selectionPath = 0;
            }
        }
    }
    else
    {
		int dx = event->x() - m_lastPos.x();
		int dy = event->y() - m_lastPos.y();

		if (event->buttons() & Qt::LeftButton)
        {
            setXRotation(m_xRot + 8 * dy);
            setYRotation(m_yRot + 8 * dx);
        }
        else if (event->buttons() & Qt::RightButton)
        {
            setXRotation(m_xRot + 8 * dy);
            setZRotation(m_zRot + 8 * dx);
        }
		m_lastPos = event->pos();
	}
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_selectionState == GLWIDGETSTATE_EDIT)
    {
        if (m_selectionPath != 0)
        {
            if (m_selectionPath->elementCount() > 2)
            {
                m_selectionPath->closeSubpath();
                QRectF r = m_selectionPath->boundingRect();

                bool ctrl = event->modifiers() & Qt::ControlModifier;
                processSelection(r.left(), r.bottom(), (int)r.width(), (int)r.height(), !ctrl);
                delete m_selectionPath;
                m_selectionPath = 0;
            }
        }
		ChannelConfigModel *ccm = ChannelConfigModel::instance();
		int channel = ccm->CurrentChannel();
		ccm->emitSignal(channel, SIGNAL_WAVEFORM_SELECTION_CHANGED);
	}
    update();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (!this->hasFocus())
        return;
    if (!this->isVisible())
        return;

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();
    QVector<quint32> selectionIndexes = featuresData->getSelectionIndex();

    switch (event->key())
    {
    case Qt::Key_Plus:
    case Qt::Key_ZoomIn:                               // zoom in
        m_scale += 0.1;
        update();
        break;
    case Qt::Key_Minus:
    case Qt::Key_ZoomOut:                               // zoom out
        m_scale -= 0.1;
        update();
        break;
    case Qt::Key_E:
        ccm->setData(ccm->index(channel, CC_3D_EDIT_STATE), QVariant(GLWIDGETSTATE_EDIT));
        break;
    case Qt::Key_R:
        ccm->setData(ccm->index(channel, CC_3D_EDIT_STATE), QVariant(GLWIDGETSTATE_ROTATE));
        break;
    case Qt::Key_C:
        ccm->setData(ccm->index(channel, CC_3D_EDIT_STATE), QVariant(GLWIDGETSTATE_CENTROIDS));
        break;

    }
    if (selectionIndexes.isEmpty())
        return;

    int unit = unitNumberFromKey(event->key());

    if ((unit >= 0) /*|| (key == Qt::Key_Backspace) || (key == Qt::Key_Delete)*/)
    {
        // create the unit from the selection
        UnitAssignment setUnit;
        setUnit.assign(unit, EDITMODE_SELECT);
        featuresData->clearSelection();
    }
    QGLWidget::keyPressEvent(event);
}

/// @author Holly Ausbeck
/// @date   Aug 5, 2015
/// @brief  convert a key to an existing unit number
/// @param [in]  Qt::key enum
/// @param [out] existing unit number or -1 if the key didn't specify an existing unit
int GLWidget::unitNumberFromKey(int key)
{
	int unit = -1;

	switch (key)
	{
	case Qt::Key_0:
		unit = 0;
		break;
	case Qt::Key_1:
		unit = 1;
		break;
	case Qt::Key_2:
		unit = 2;
		break;
	case Qt::Key_3:
		unit = 3;
		break;
	case Qt::Key_4:
		unit = 4;
		break;
	case Qt::Key_5:
		unit = 5;
		break;
	case Qt::Key_6:
		unit = 6;
		break;
	case Qt::Key_7:
		unit = 7;
		break;
	case Qt::Key_8:
		unit = 8;
		break;
	case Qt::Key_9:
		unit = 9;
		break;
	}
	return unit;
}


void GLWidget::wheelEvent(QWheelEvent *event)
{
    qreal d = event->delta();

    if (d > 0)
        d = 0.1;
    else
        d = -0.1;

    m_scale += d;
    update();
}


#include "glwidgetpc.h"

#include <QMouseEvent>
#include <QTime>
#include <QGLShader>
#include <QVector3D>
#include <QMutex>

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
	: QOpenGLWidget(parent)
{
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    slotResetRotation();
    m_program[0] = NULL;
    m_program[1] = NULL;

    m_Xmax = 0;
    m_Ymax = 0;
    m_Zmax = 0;
    m_max = 0;
    m_scale = 1.0;

    m_xColor = Qt::red; 
    m_yColor = Qt::green;
    m_zColor = QColor("dodgerblue");

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
    connect(cm, SIGNAL(pointSizeChanged(int)), this, SLOT(slotPointSizeChanged(int)));

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
	
	QSurfaceFormat format;
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CompatibilityProfile);
    //format.setVersion(3, 3);
	setFormat(format);
       

    memset(m_vao, 0, sizeof(m_vao));
    memset(m_vbo, 0, sizeof(m_vbo));
    memset(m_ebo, 0, sizeof(m_ebo));
    memset(textures, 0, sizeof(QOpenGLTexture*)*FEATUREID_END);
    m_bCtrl = false;
    m_bMouseMoved = false;
}


void GLWidget::initializeGL()
{
    makeCurrent();
    initializeOpenGLFunctions();

    qDebug() << "Widget OpenGl: " << QOpenGLWidget::format().majorVersion() << "." << QOpenGLWidget::format().minorVersion();
    qDebug() << "Context valid: " << context()->isValid();
    qDebug() << "Really used OpenGl: " << context()->format().majorVersion() << "." << context()->format().minorVersion();
    qDebug() << "OpenGl information: VENDOR:       " << (const char*)glGetString(GL_VENDOR);
    qDebug() << "                    RENDERDER:    " << (const char*)glGetString(GL_RENDERER);
    qDebug() << "                    VERSION:      " << (const char*)glGetString(GL_VERSION);
    qDebug() << "                    GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);


    // Points buffers are generated elsewhere
    glGenBuffers(7, m_vbo);
    glGenBuffers(6, m_ebo);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepth(1.0f);                                // Depth Buffer Setup


    initAxes();
    initLabels();

    initializeData();

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(ccm, SIGNAL(visibleUnitsChanged(int, QStringList)), this, SLOT(slotVisibleUnitsChanged(int, QStringList)));
    connect(ccm, SIGNAL(threeDAxisChanged(int, ChannelColumn, FeatureID)), this, SLOT(slotAxisChanged(int, ChannelColumn, FeatureID)));
    connect(ccm, SIGNAL(glEditStateChanged(int, GLWidgetState)), this, SLOT(slotGlEditStateChanged(int, GLWidgetState)));
}



GLWidget::~GLWidget()
{
    makeCurrent();

    if (m_selectionPath != 0)
        delete m_selectionPath;

    for (int i(0); i < 7; ++i)
        if (m_vbo[i])
            glDeleteBuffers(1, &m_vbo[i]);


    for (int i(0); i < 6; ++i)
    {
        if (m_vao[i] != 0)
            delete m_vao[i];
        
        if (m_ebo[i])
            glDeleteBuffers(1, &m_ebo[i]);

    }

    for (int i(0); i < 2; ++i)
        if (m_program[i])
            delete m_program[i];

    for (int i(0); i < FEATUREID_END; ++i)
        if (textures[i])
            delete textures[i];
    doneCurrent();
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
    setPoints();
}

void GLWidget::initLabels()
{
    m_program[TEXTURE_PROGRAM] = new QOpenGLShaderProgram(context());
    m_program[TEXTURE_PROGRAM]->addShaderFromSourceCode(QOpenGLShader::Vertex,
        "#if __VERSION__ >= 140\n"
        "in vec3 position;\n"
        "in vec2 texCoord;\n"
        "out vec2 ourTexCoord;\n"
        "#else\n"
        "attribute vec3 position;\n"
        "attribute vec2 texCoord;\n"
        "varying vec2 ourTexCoord;\n"
        "#endif\n"
        "uniform mat4 mvMatrix;\n"
        "void main()\n"
        "{\n"
        "gl_Position = mvMatrix * vec4(position, 1.0); \n"
        "ourTexCoord = texCoord; \n"
        "}");
    m_program[TEXTURE_PROGRAM]->addShaderFromSourceCode(QOpenGLShader::Fragment,
        "#if __VERSION__ >= 140\n"
        "in vec2 ourTexCoord;\n"
        "out vec4 color; \n"
        "#else\n"
        "varying vec2 ourTexCoord;\n"
        "#endif\n"
        "uniform sampler2D ourTexture; \n"
        "uniform vec4 labelColor; \n"
        "void main()\n"
        "{\n"
        "#if __VERSION__ >= 140\n"
        "color = texture(ourTexture, ourTexCoord)*labelColor; \n"
        "#else\n"
        "gl_FragColor = texture2D(ourTexture, ourTexCoord)*labelColor; \n"
        "#endif\n"
        "}");

    m_program[TEXTURE_PROGRAM]->bindAttributeLocation("position", 0);
    m_program[TEXTURE_PROGRAM]->bindAttributeLocation("texCoord", 2);
    m_program[TEXTURE_PROGRAM]->link();
    m_mvMatrixLoc[TEXTURE_PROGRAM] = m_program[TEXTURE_PROGRAM]->uniformLocation("mvMatrix");


    QImage b;

    // Initialize texture data
    GLuint indices[] =
    {
        0, 1, 2,
        3, 0, 2,
        0, 1, 2,
        3, 0, 2,
        0, 1, 2,
        3, 0, 2
    };

    GLfloat vertices[] =
    {
        // x    y    z    textureCoords
        // x label
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.7f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.7f, 0.1f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.1f, 0.0f, 1.0f, 1.0f,        
        // y label
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.7f, 0.0f, 1.0f, 0.0f,
        0.1f, 0.7f, 0.0f, 1.0f, 1.0f,
        0.1f, 1.0f, 0.0f, 0.0f, 1.0f,
        // z label
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.7f, 0.0f, 0.0f,
        0.1f, 0.0f, 0.7f, 0.0f, 1.0f,
        0.1f, 0.0f, 1.0f, 1.0f, 1.0f
        

    };
    

    for (int axis(0); axis < 3; ++axis)
    {
        m_vao[axis] = new QOpenGLVertexArrayObject();
        m_vao[axis]->create();
        m_vao[axis]->bind();

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo[axis]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 20, &vertices[axis * 4 * 5], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo[axis]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, &indices[axis * 6], GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // TexCoord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);


        m_vao[axis]->release(); // Unbind VAO
    }




    for (int index = 0; index < m_resourceNames.size(); index++)
    {
        QString image = m_resourceNames.at(index);

        if (!b.load(image))
        {
            b.fill(qRgba(0, 0, 0, 0));
        }
        QOpenGLTexture *texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        QImage im = QImage(image).mirrored();
        QImage mask = im.createMaskFromColor(0, Qt::MaskOutColor);
        QPainter painter(&im);
        painter.setPen(QColor(0, 0, 0));
        painter.drawImage(im.rect(), mask, mask.rect());
        painter.end();

        texture->setData(im);
        texture->bind();
        texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
        textures[index] = texture;
        texture->release();
    }
}

void GLWidget::initAxes()
{
    m_program[AXES_POINTS_PROGRAM] = new QOpenGLShaderProgram(context());
    m_program[AXES_POINTS_PROGRAM]->addShaderFromSourceCode(QOpenGLShader::Vertex,
        "#if __VERSION__ >= 140\n"
        "in vec3 position;\n"
        "#else\n"
        "attribute vec3 position;\n"
        "#endif\n"
        "uniform mat4 mvMatrix;\n"
        "uniform mat4 modifierMatrix;\n"
        "void main()\n"
        "{\n"
        "gl_Position = mvMatrix * modifierMatrix * vec4(position, 1.0); \n"
        "}");
    m_program[AXES_POINTS_PROGRAM]->addShaderFromSourceCode(QOpenGLShader::Fragment,
        "#if __VERSION__ >= 140\n"
        "out vec4 color;\n"
        "#endif\n"
        "uniform vec4 changingColor; \n"
        "void main()\n"
        "{\n"
        "#if __VERSION__ >= 140\n"
        "color = changingColor; \n"
        "#else\n"
        "gl_FragColor = changingColor; \n"
        "#endif\n"

        "}");
    m_program[AXES_POINTS_PROGRAM]->bindAttributeLocation("position", 0);
    m_program[AXES_POINTS_PROGRAM]->link();
    m_mvMatrixLoc[AXES_POINTS_PROGRAM] = m_program[AXES_POINTS_PROGRAM]->uniformLocation("mvMatrix");
    m_modifierMatrixLoc = m_program[AXES_POINTS_PROGRAM]->uniformLocation("modifierMatrix");


    QImage b;

    // Initialize texture data
    GLuint indices[] =
    {
        0, 1,
        0, 1,
        0, 1
    };

    GLfloat vertices[] =
    {
        // x    y    z    
        // x axis
        1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,

        // y axis
        0.0f, 1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,

        // z axis
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, -1.0f
    };

    for (int axis(0); axis < 3; ++axis)
    {
        m_vao[AXES_OFFSET + axis] = new QOpenGLVertexArrayObject();
        m_vao[AXES_OFFSET + axis]->create();
        m_vao[AXES_OFFSET + axis]->bind();

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo[AXES_OFFSET + axis]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6, &vertices[axis * 3 * 2], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo[axis]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 2, &indices[axis * 2], GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);        
        
        m_vao[AXES_OFFSET + axis]->release(); // Unbind VAO
    }


}



void GLWidget::drawLabels()
{
    m_program[TEXTURE_PROGRAM]->bind();
    m_program[TEXTURE_PROGRAM]->setUniformValue(m_mvMatrixLoc[TEXTURE_PROGRAM], m_camera*m_world);

   
    for (int axis(0); axis < 3; ++axis)
    {
        QColor color;
        switch (axis)
        {
        case 0:
            color = m_xColor;
            break;
        case 1:
            color = m_yColor;
            break;
        case 2:
            color = m_zColor;
            break;

        }
            
        textures[(int)getFeatureID(axis)]->bind();
        glUniform1i(m_program[TEXTURE_PROGRAM]->uniformLocation("ourTexture"), 0);
        m_program[TEXTURE_PROGRAM]->setUniformValue(m_program[TEXTURE_PROGRAM]->uniformLocation("labelColor"), color);
        m_vao[axis]->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        m_vao[axis]->release();
        textures[(int)getFeatureID(axis)]->release();
    }   


    m_program[TEXTURE_PROGRAM]->release();
}

void GLWidget::paintGL()
{

	glClearColor(0.0, 0.0, 0.0, 1.0);
	qreal pixelRatio = devicePixelRatio();
    setupViewport(width() * pixelRatio, height() * pixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    m_world.setToIdentity();
    m_world.rotate(m_xRot / 16.0f, 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

    m_camera.setToIdentity();
    m_camera.scale(m_scale, m_scale, m_scale);

    drawLabels();
    drawCoordinateAxes();
    drawPoints();
    


    if (m_selectionPath != 0)
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
    m_program[AXES_POINTS_PROGRAM]->bind();
    m_program[AXES_POINTS_PROGRAM]->setUniformValue(m_mvMatrixLoc[AXES_POINTS_PROGRAM], m_camera*m_world);

    QMatrix4x4 m_modifier;
    m_modifier.translate(0.0, 0.0, 0.0);
    m_program[AXES_POINTS_PROGRAM]->setUniformValue(m_modifierMatrixLoc, m_modifier);

    drawCrossHairs();

    m_program[AXES_POINTS_PROGRAM]->release();
}


void GLWidget::drawCrossHairs()
{

    for (int axis(0); axis < 3; ++axis)
    {
        QColor color;
        switch (axis)
        {
        case 0:
            color = m_xColor;
            break;
        case 1:
            color = m_yColor;
            break;
        case 2:
            color = m_zColor;
            break;

        }

        m_program[AXES_POINTS_PROGRAM]->setUniformValue(m_program[AXES_POINTS_PROGRAM]->uniformLocation("changingColor"), color);
        m_vao[AXES_OFFSET + axis]->bind();
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
        m_vao[AXES_OFFSET + axis]->release();
    }
}


void GLWidget::setPoints()
{
    m_mutexPoints.lock();
    ChannelConfigModel * ccm = ChannelConfigModel::instance();
    int channel = ccm->CurrentChannel();
    FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();

    FeatureID featureX = getFeatureID(0);
    FeatureID featureY = getFeatureID(1);
    FeatureID featureZ = getFeatureID(2);

    if (featuresData)
    {
        QVector<quint32> wfIDs = featuresData->getWaveformIDs();
        int nPoints = wfIDs.size();
        QVector<GLdouble> vertexData(nPoints * 3);

        // Fill the data buffer        
        for (int j = 0; j < wfIDs.size(); j++)
        {
            vertexData[3 * j + 0] = featuresData->getFeatureValueForIndex(j, featureX);
            vertexData[3 * j + 1] = featuresData->getFeatureValueForIndex(j, featureY);
            vertexData[3 * j + 2] = featuresData->getFeatureValueForIndex(j, featureZ);
        }
        
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo[POINTS_OFFSET]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLdouble) * vertexData.size(), (GLvoid*)vertexData.constData(), GL_STREAM_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    m_mutexPoints.unlock();
}

void GLWidget::drawPoints()
{
    m_mutexPoints.lock();
    m_program[AXES_POINTS_PROGRAM]->bind();
    m_program[AXES_POINTS_PROGRAM]->setUniformValue(m_mvMatrixLoc[AXES_POINTS_PROGRAM], m_camera*m_world);

	ChannelConfigModel * ccm = ChannelConfigModel::instance();
	int channel = ccm->CurrentChannel();
	FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();
	UnitAccum unitData = ccm->data(ccm->index(channel, CC_SPIKEDATA_UNITS), Qt::UserRole).value<UnitAccum>();

    if (featuresData && unitData)
	{
		QVector<quint32> selectionIndexes = featuresData->getSelectionIndex();
		QVector<quint32> wfIDs = featuresData->getWaveformIDs();
		QVector<int> units = featuresData->getAssignments();
		QVector<int> usedUnits;
        QVector<int> count;
        for (int j = 0; j < units.size(); ++j)
        {
            if (usedUnits.contains(units[j]) == false)
            {
                usedUnits.append(units[j]);
                count.append(1);
            }
            else
            {
                int ind = usedUnits.indexOf(units[j]);
                count[ind] += 1;
            }
        }

		glPointSize(m_pointSize);
		
        GLuint ebo;
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo[POINTS_OFFSET]);
        glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(GLdouble), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		for (int i = 0; i < usedUnits.size(); i++)
		{
			int unit = usedUnits[i];
			

            if (!unitData->isVisible(unit) || (count[i] == 0))
				continue;
                        

			QColor color = ConfigModel::instance()->getColor(unit);
            m_program[AXES_POINTS_PROGRAM]->setUniformValue(m_program[AXES_POINTS_PROGRAM]->uniformLocation("changingColor"), color);

            // Fill the data buffer
            QVector<GLuint> indices(count[i]);
            int k = 0;
            for (int j = 0; j < wfIDs.size(); ++j)
            {
                if (units[j] == unit)
                {
                    indices[k] = j;
                    ++k;
                }
            }
           
            
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * count[i], (GLvoid*)&indices[0], GL_STREAM_DRAW);
            glDrawElements(GL_POINTS, count[i], GL_UNSIGNED_INT, 0);
            
                                    

		}
        
              
        if (selectionIndexes.size() != 0)
        {
            int count = selectionIndexes.size();
            QColor white(Qt::white);
            m_program[AXES_POINTS_PROGRAM]->setUniformValue(m_program[AXES_POINTS_PROGRAM]->uniformLocation("changingColor"), white);

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * count, (GLvoid*)selectionIndexes.constData(), GL_STREAM_DRAW);
            glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, 0);
            
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &ebo);        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        if (m_selectionState == GLWIDGETSTATE_CENTROIDS)
        {
            ClusterDataPtr pClusterData = ccm->data(ccm->index(channel, CC_CENTROIDS), Qt::UserRole).value<ClusterDataPtr>();

            for (int i = 0; i < pClusterData->nbrClusters(); i++)
            {
                double vX = pClusterData->getFeatureValueNormalized(featuresData, i, getFeatureID(0));
                double vY = pClusterData->getFeatureValueNormalized(featuresData, i, getFeatureID(1));
                double vZ = pClusterData->getFeatureValueNormalized(featuresData, i, getFeatureID(2));

                centroid(vX, vY, vZ);
            }
        }
	}

    m_program[AXES_POINTS_PROGRAM]->release();
    m_mutexPoints.unlock();
}


void GLWidget::centroid(GLdouble x, GLdouble y, GLdouble z)
{

    QMatrix4x4 m_modifier;
    m_modifier.translate(x, y, z);
    m_modifier.scale(float(m_pointSize * 0.05));
    m_program[AXES_POINTS_PROGRAM]->setUniformValue(m_modifierMatrixLoc, m_modifier);

    drawCrossHairs();

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
	glGetIntegerv(GL_VIEWPORT, m_viewport);

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
    setPoints();
    this->update();
}

void GLWidget::slotChannelChanged(int channel)
{
    initializeData();
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

    setPoints();
	update();
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
		QVector<quint32> centroids;

		if (m_selectionState == GLWIDGETSTATE_CENTROIDS)
			list = &centroids;

		if (clear)
		{
			list->clear();
		}
		QVector<quint32> selectionIndex = *list;

		this->makeCurrent();
		unsigned char res[4];
		renderSelection(1);

		glReadPixels(x, m_viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, res);
		quint32 index = RGBToIndex(res[0], res[1], res[2]);

		if (index < (quint32)units.size())
		{
			if (ccm->unitVisible(channel, units.at(index)))
			{
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
			updateCentroids(featuresData, centroids, clear);
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
		QVector<quint32> centroids;

		if (m_selectionState == GLWIDGETSTATE_CENTROIDS)
			list = &centroids;

		if (clear)
			list->clear();

		QVector<quint32> selectionIndex = *list;
		quint8 r = 0;
		quint8 g = 0;
		quint8 b = 0;

		unsigned char *res = new unsigned char[5 * width * height];
		this->makeCurrent();

		while (true)
		{
			bool finished = true;

			renderSelection(width * height);
			glReadPixels(x, m_viewport[3] - y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, res);
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

                        if (m_adjustedSelectionPath.contains(p))
						{
							quint32 index = RGBToIndex(r, g, b);

							if (index < (quint32)units.size())
							{
								if (ccm->unitVisible(channel, units.at(index)))
								{
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

		if (m_selectionState != GLWIDGETSTATE_CENTROIDS)
		{
			featuresData->setSelectionIndex(selectionIndex);
		}
		else
		{
			updateCentroids(featuresData, centroids, clear);
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
				if (!m_selected.contains(i))
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
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	int channel = ccm->CurrentChannel();
	m_selectionState = (GLWidgetState)ccm->data(ccm->index(channel, CC_3D_EDIT_STATE), Qt::UserRole).toInt();
	
	m_lastPos = event->pos();
    m_bMouseMoved = false;

    if (event->buttons() & Qt::LeftButton)
    {
		if (m_selectionPath != 0)
            delete m_selectionPath;
        m_selectionPath = 0;

        m_bCtrl = event->modifiers() & Qt::ControlModifier;

        if (!m_bCtrl && (m_selectionState != GLWIDGETSTATE_ROTATE))
		{
			// Clear all selections
			FeaturesAccum featuresData = ccm->data(ccm->index(channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();
			featuresData->clearSelection();
		}
        if (m_selectionState == GLWIDGETSTATE_EDIT)
        {
			m_selectionPath = new QPainterPath(event->pos());
			update();
        }
        else if (m_selectionState == GLWIDGETSTATE_CENTROIDS)
        {
			update();
        }
    }
    else if (event->buttons() & Qt::RightButton)
    {
        if (m_selectionPath != 0)
        {
            delete m_selectionPath;
            m_selectionPath = 0;
        }
        EditMenu *menu = new EditMenu(this);
        if (!menu->isEmpty())
            menu->exec(QCursor::pos());

        menu->deleteLater();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_bMouseMoved = true;

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
                qreal pixelRatio = devicePixelRatio();

                // Adjust selection path for pixel ratio
                m_adjustedSelectionPath = QPainterPath();
                for (int i(0); i < m_selectionPath->elementCount(); ++i)
                {

                    QPointF p = m_selectionPath->elementAt(i);
                    QPointF new_p(p.x()*pixelRatio, p.y()*pixelRatio);
                    if (i == 0)
                        m_adjustedSelectionPath.moveTo(new_p);
                    else
                        m_adjustedSelectionPath.lineTo(new_p);
                }

                QRectF adjusted_r(r.x() * pixelRatio, r.y() * pixelRatio, r.width() * pixelRatio, r.height() * pixelRatio);

                bool ctrl = event->modifiers() & Qt::ControlModifier;
                processSelection(adjusted_r.left(), adjusted_r.bottom(), (int)adjusted_r.width(), (int)adjusted_r.height(), !ctrl);
                m_adjustedSelectionPath = QPainterPath();
            }
            else
            {
                qreal pixelRatio = devicePixelRatio();
                processSingleSelection(m_lastPos.x()*pixelRatio, m_lastPos.y()*pixelRatio, !m_bCtrl);
            }
            delete m_selectionPath;
            m_selectionPath = 0;
        }
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        int channel = ccm->CurrentChannel();
        ccm->emitSignal(channel, SIGNAL_WAVEFORM_SELECTION_CHANGED);
    }
    else if (m_selectionState == GLWIDGETSTATE_CENTROIDS)
    {
        if (!m_bMouseMoved)
        {
            qreal pixelRatio = devicePixelRatio();
            processSingleSelection(m_lastPos.x() * pixelRatio, m_lastPos.y() * pixelRatio, !m_bCtrl);
        }
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
	QOpenGLWidget::keyPressEvent(event);
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
        d = 0.01;
    else
        d = -0.01;

    m_scale += d;
    update();
}


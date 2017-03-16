#ifndef GROUNDRENDERER_H_Q6HNJOSV
#define GROUNDRENDERER_H_Q6HNJOSV

#include <iostream> // remove
#include <memory>
#include <vector>

#include <QEntity>
#include <QMutex>
#include <QOpenGLVertexArrayObject>
#include <QQmlParserStatus>
#include <QRectF>
#include <QtGui/QOpenGLExtraFunctions>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtQuick/QQuickItem>
#include <QtQuick/qquickwindow.h>

#include "app/CircularOpenGLBuffer.h"

#include "app/qobjs/Viewport.h"
#include "app/qobjs/GroundModel.h"

class GroundModel;

class GroundRenderer : public QObject
                     , protected QOpenGLExtraFunctions
{
    Q_OBJECT
    Q_PROPERTY(GroundModel* model READ model WRITE setModel NOTIFY modelChanged)

   public:
    using PointType = Point<3, 4>;

    /**
     * Configure our buffer (usage, etc)
     */
    GroundRenderer();

    /**
     * Set our model, then call the default constructor
     */
    GroundRenderer(GroundModel* model);

    ~GroundRenderer() = default;

    /**
     * Pointer to our GroundModel
     */
    auto model() -> GroundModel* { return model_; };

    /**
     * Set our GroundModel, wire up the NewPoints signals
     */
    auto setModel(GroundModel* model) -> void;

    /** Getter for byte stride */
    auto byteStride() const -> unsigned int { return byte_stride_; };

    /** Set byte stride */
    auto setByteStride(unsigned int bs) -> void { byte_stride_ = bs; };

    /** Set ship */
    auto setShip(Qt3DCore::QEntity* ship) -> void { ship_ = ship; }

    /**
     * Set the maximum number of points the buffer can hold
     */
    auto setBufferPointCapacity(const unsigned int mc) -> void { max_buffer_point_capacity_ = mc; }

    /** Set point size */
    auto setPointSize(const qreal s) -> void { pointSize_ = s>0 ? s : 1; }

    /** Compute shader url setter */
    auto setComputeShader(const QUrl& shader) -> void { compute_shader_ = shader; }

    /** Vertex shader url setter */
    auto setVertexShader(const QUrl& shader) -> void { vertex_shader_ = shader; }

    /** Fragment shader url setter */
    auto setFragmentShader(const QUrl& shader) -> void { fragment_shader_ = shader; }

    /** Return number of vertices in the OpenGL buffer */
    auto vertexCount() const -> unsigned int;

    // #<{(|* Return number of components (i.e. floats) in a vertex |)}>#
    // auto vertexSize() -> unsigned int;
    // #<{(|* Return number of components (i.e. floats) in a vertex |)}>#
    // auto setVertexSize(unsigned int s) -> void { n_vertexComponents = s };

    /**
     * Return number of components (floats) in a point
     */
    auto vertexComponents() const -> unsigned int;

    /**
     * Setup our viewports
     * TODO No longer used, should probably be removed.
     */
    auto setViewports(const QList<Viewport*>& viewports) -> void { viewports_ = viewports; }

    /**
     * Set our window
     */
    auto setWindow(QQuickWindow* window) -> void { window_ = window; }

  signals:
    void modelChanged(GroundModel* model);

   public slots:
    /**
     * Initiate painting/rendering
     */
    void paint();

    /**
     * New vectorized points available, load them into tmp_point_buffer_
     */
    void NewPoints(const std::vector<PointType>& data);


   private:
    /**
     * Initializes OpenGL content, creates the render and shader programs,
     * finds the location of variables, and binds data.
     */
    auto initializeGL() -> void;

    /**
     * Copies new points found in our tmp_point_buffer_ into our CircularBuffer
     */
    auto loadNewPoints() -> void;

    /**
     * Run the compute shader
     */
    auto compute() -> void;

    /**
     * Pain the points to the frame buffer
     */
    auto paintGL() -> void;

    /**
     * Store the vertex attribute bindings for the program.
     */
    auto setupVertexAttribs() -> void;

    /**
     * Given a normalized rect, figure out where to paint the points in the
     * window.
     */
    auto setViewport(const QRectF& vp) -> const QSize;

    /**
     * Non-owning pointer back to our model (ths model supplies LCM points)
     */
    GroundModel* model_ = nullptr;

    /**
     * Render program, calls our vertex and frag shaders.
     */
    std::unique_ptr<QOpenGLShaderProgram> renderProgram_ = nullptr;

    /**
     * Our compute shader responsible for translating ground points with
     * respect to ship movement.
     */
    std::unique_ptr<QOpenGLShaderProgram> computeProgram_ = nullptr;

    /**
     * Non-owning pointer to our QWindow
     */
    QQuickWindow* window_ = nullptr;

    /**
     * Circular OpenGL buffer, this is derived from the QOpenGLBuffer, and
     * allows us to read/write from a circular buffer.
     */
    CircularOpenGLBuffer pointOpenGLBuffer_{QOpenGLBuffer::VertexBuffer};

    // TODO remove
    unsigned int compute_count = 0u;

    /**
     * Vertex array object.  Not 100% sure what this is for, but without it
     * vertices aren't bound to the vertex shader.
     */
    QOpenGLVertexArrayObject vao_;

    int loc_projection_     = -1;
    int loc_worldMatrix_    = -1;
    int loc_viewMatrix_     = -1;
    int loc_modelView_      = -1;
    int loc_viewportMatrix_ = -1;
    int loc_Particles_      = -1;
    int loc_vertexPosition_ = -1;
    int loc_transform_      = -1;
    int loc_Col0_           = -1;
    int loc_Col1_           = -1;
    int loc_zLow_           = -1;
    int loc_zHigh_          = -1;


    QList<Viewport*> viewports_;

    /**
     * Mutex protecting our temporary buffer
     */
    QMutex tmpBuffer_;

    /**
     * The different threads block me from being able to bind
     * to the OpenGL context, so new points are put on this
     * buffer, and then synced into the OpenGL buffer when
     * rendered.
     */
    std::vector<PointType> tmp_point_buffer_;

    /**
     * Number of components in a vertex, so 4 is x,y,z,padding.
     *
     * TODO this might be removed when points include intensity.. depending on how that goes
     */
    unsigned int n_vertexComponents = 4;

    /**
     * Byte stride in the buffer.  Each vertex takes up
     * vertexComponents*GLfloats bytes
     */
    unsigned int byte_stride_ = n_vertexComponents*sizeof(GLfloat);

    /**
     * Holds a non-owning pointer to the ship model, used for translation in
     * the compute shader
     */
    Qt3DCore::QEntity* ship_ = nullptr;

    /**
     * Last position of the ship, used for computing translation
     */
    QVector3D last_pos_{0.0f, 0.0f, 0.0f};

    /** Compute shader url */
    QUrl compute_shader_;

    /** Vertex shader url */
    QUrl vertex_shader_;

    /** Fragment shader url */
    QUrl fragment_shader_;

    /** Max point capacity of OpenGL buffer */
    unsigned int max_buffer_point_capacity_ = 1024 * 10;

    /** Point size used in vertex shader */
    qreal pointSize_ = 1;

    /** Colours used in fragment shader for colour interpolation */
    QVector4D col0_{0.0f, 0.0f, 1.0f, 1.0f};
    QVector4D col1_{1.0f, 0.0f, 1.0f, 1.0f};

    /**
     * For elevation colour scheme, these are the ranges the colour is
     * interpolated in
     */
    qreal zLow_  = -10;
    qreal zHigh_ =  10;

};

#endif /* end of include guard: GROUNDRENDERER_H_Q6HNJOSV */

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */

#ifndef GROUNDMODEL_H_ZDY0ZNJO
#define GROUNDMODEL_H_ZDY0ZNJO

#include <QDebug>
#include <QEntity>
#include <QQmlParserStatus>
#include <QtQuick/QQuickItem>

#include "app/Point.h"
#include "app/qobjs/Viewport.h"
#include "app/qobjs/GroundRenderer.h"

class GroundRenderer;

class GroundModel : public QQuickItem
{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(qreal pointSize READ pointSize WRITE setPointSize NOTIFY pointSizeChanged)
    Q_PROPERTY(unsigned int maxBufferPointCapacity READ maxBufferPointCapacity WRITE setMaxBufferPointCapacity NOTIFY maxBufferPointCapacityChanged)
    Q_PROPERTY(unsigned int byteStride READ byteStride WRITE setByteStride NOTIFY byteStrideChanged)

    // TODO Change to origo or something when cordinates are figured out
    Q_PROPERTY(Qt3DCore::QEntity* ship READ ship WRITE setShip NOTIFY shipChanged)

    Q_PROPERTY(QUrl computeShader READ computeShader WRITE setComputeShader NOTIFY computeShaderChanged)
    Q_PROPERTY(QUrl vertexShader READ vertexShader WRITE setVertexShader NOTIFY vertexShaderChanged)
    Q_PROPERTY(QUrl fragmentShader READ fragmentShader WRITE setFragmentShader NOTIFY fragmentShaderChanged)

    // Debugging flag to deactive OpenGL
    Q_PROPERTY(bool deactivePoints READ deactivePoints WRITE setDeactivePoints)

   public:
    using PointType = Point<3, 4>;

    /**
     * Connects the windowChanged signal to handleWindowChanged
     */
    GroundModel();

    ~GroundModel() = default;

    Q_INVOKABLE void callSimulatePoints()
    {
#ifdef DEBUG_POINTS
        SimulateInitialPoints();
#else
        std::cout << "Enable DEBUG_POINTS" << std::endl;
#endif
    }

    /** Return point size */
    auto pointSize() const -> qreal { return pointSize_; }

    /** Set point size */
    auto setPointSize(const qreal s) -> void { pointSize_ = s>0 ? s : 1; }

    /** Return maximum number of points in the buffer */
    auto maxBufferPointCapacity() const -> unsigned int { return max_buffer_point_capacity_; }

    /** Set the maximum number of buffer points */
    auto setMaxBufferPointCapacity(const unsigned int mc) -> void { max_buffer_point_capacity_ = mc; }

    /** Return number of vertices in the OpenGL buffer */
    auto byteStride() const -> unsigned int { return byte_stride_; };

    /** Set byte stride */
    auto setByteStride(const unsigned int bs) -> void { byte_stride_ = bs; };

    /** Returns the ship pointer */
    auto ship() -> Qt3DCore::QEntity* { return ship_; };

    /** Set ship */
    auto setShip(Qt3DCore::QEntity* ship) -> void;

    /** Compute shader url getter */
    auto computeShader() -> const QUrl { return compute_shader_; }

    /** Compute shader url getter */
    auto setComputeShader(const QUrl& shader) -> void { compute_shader_ = shader; }

    /** vertex shader url getter */
    auto vertexShader() -> const QUrl { return vertex_shader_; }

    /** vertex shader url getter */
    auto setVertexShader(const QUrl& shader) -> void { vertex_shader_ = shader; }

    /** Fragment shader url getter */
    auto fragmentShader() -> const QUrl { return fragment_shader_; }

    /** Fragment shader url getter */
    auto setFragmentShader(const QUrl& shader) -> void { fragment_shader_ = shader; }

    auto deactivePoints() const -> bool { return deactive_points_; }
    auto setDeactivePoints(bool deactive) -> void { deactive_points_ = deactive; }

    /**
     * Perform some initialization here now that the object is fully created
     */
    virtual auto componentComplete() -> void override;

   signals:
    /**
     * Signal emitted when new vectorized data arrives (likely generated
     * testing data)
     */
    void NewPoints(const std::vector<PointType>& data);

    /** Signal emitted if the byte size ever changes */
    void byteStrideChanged(unsigned int byte_stride);

    /** Signal emitted if the ship model changes */
    void shipChanged(Qt3DCore::QEntity* new_ship);

    /** Signal emitted if the buffer capacity is changed */
    void maxBufferPointCapacityChanged(unsigned int mc);

    /** Signal emitted if pointSize is changed */
    void pointSizeChanged(qreal pointSize);

    // TODO These signals aren't yet listened to, and thus won't cause OpenGL
    //      to re-initialize with the new programs.
    void computeShaderChanged();
    void vertexShaderChanged();
    void fragmentShaderChanged();



   public slots:
    /**
     * Synchronize the GroundModel to the GroundRenderer
     */
    void sync();

    /**
     * Delete the renderer and any other points we have
     */
    void cleanup();

   private slots:
    void handleWindowChanged(QQuickWindow *win);

   protected:

#ifdef DEBUG_POINTS
    /**
     * Simulate ground plane
     */
    auto SimulateInitialPoints() -> void;
#endif

    /**
     * List of viewports we display in
     */
    QList<Viewport*> viewports_;

    /**
     * Ground Renderer
     */
    std::unique_ptr<GroundRenderer> renderer_ = nullptr;

    /** Byte stride.  Initialization value is simply a guess */
    unsigned int byte_stride_ = 4*sizeof(GLfloat);

    /** Non owning pointer to Ship entity, used for transforming points */
    Qt3DCore::QEntity* ship_ = nullptr;

    /** Compute shader */
    QUrl compute_shader_;

    /** Vertex shader */
    QUrl vertex_shader_;

    /** Fragment shader */
    QUrl fragment_shader_;

    /** Max buffer point capacity */
    unsigned int max_buffer_point_capacity_ = 1024 * 100;

    /** Point size used in vertex shader */
    qreal pointSize_ = 1;

    /**
     * Debugging flag to deactivate OpenGL
     */
    bool deactive_points_ = false;
};

#endif /* end of include guard: GROUNDMODEL_H_ZDY0ZNJO */

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */

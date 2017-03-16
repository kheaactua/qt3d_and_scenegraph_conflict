#include "app/qobjs/GroundRenderer.h"

#include <iomanip> // TODO remove

#include <QQmlProperty>

#include "app/Memory.hpp"
#include "app/helpers.h"

GroundRenderer::GroundRenderer()
{
    pointOpenGLBuffer_.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    pointOpenGLBuffer_.setMaxCapacity(max_buffer_point_capacity_ * byte_stride_);
}
GroundRenderer::GroundRenderer(GroundModel* model)
    : model_(model)
{
    // Little redundant, but this function will also setup signals
    setModel(model);

    // Call default constructor
    GroundRenderer();
}

auto GroundRenderer::setModel(GroundModel* model) -> void
{
    model_ = model;

    connect(model_, SIGNAL(NewPoints(const std::vector<PointType>&)), this, SLOT(NewPoints(const std::vector<PointType>&)), Qt::DirectConnection);
}

auto GroundRenderer::NewPoints(const std::vector<PointType>& data) -> void
{
    tmpBuffer_.lock();
    std::copy(
        data.begin(),
        data.end(),
        std::back_inserter(tmp_point_buffer_)
    );
    tmpBuffer_.unlock();
}

auto GroundRenderer::paint() -> void
{
    if (!model_) return;

    initializeGL();

    loadNewPoints();

    if (ship_) {
        // No point in running translations if there isn't a ship

        // TODO Perhaps only call this every X frame.
        compute();
    }

    paintGL();
}


auto GroundRenderer::initializeGL() -> void /*{{{*/
{
    if (!renderProgram_ && !computeProgram_) {

        if (compute_shader_.isEmpty())
        {
            qFatal("Compute shader not provided.");
        }
        else if (vertex_shader_.isEmpty())
        {
            qFatal("Vertex shader not provided.");
        }
        else if (fragment_shader_.isEmpty())
        {
            qFatal("Fragment shader not provided.");
        }

        initializeOpenGLFunctions();

        computeProgram_ = App::Memory::make_unique<QOpenGLShaderProgram>();
        computeProgram_->addShaderFromSourceFile(QOpenGLShader::Compute, helpers::urlToLocalFileOrQrc(compute_shader_));
        computeProgram_->bindAttributeLocation("Particles", 0);
        loc_Particles_ = 0; // write to position 0, this is accepted in the shader.

        computeProgram_->link();

        // char infoLog[10000];
        // glGetProgramPipelineInfoLog(computeProgram_->programId(),  sizeof(infoLog), NULL, infoLog);
        // qDebug() << infoLog;

        if (!computeProgram_->bind()) {
            qFatal("Compute program could not bind!");
        }

        loc_transform_ = computeProgram_->uniformLocation("translation");

        // Setup our vertex buffer object.
        pointOpenGLBuffer_.create();
        pointOpenGLBuffer_.bind();
        if (pointOpenGLBuffer_.Allocate())
        {
            // Pack this with zeros
            pointOpenGLBuffer_.ZeroGLfloat();
        }
        else
        {
            qWarning("[GroundRenderer::initializeGL]: Could not allocate OpenGL buffer");
        }

        computeProgram_->release();

        renderProgram_ = App::Memory::make_unique<QOpenGLShaderProgram>();
        renderProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex,   helpers::urlToLocalFileOrQrc(vertex_shader_));
        renderProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, helpers::urlToLocalFileOrQrc(fragment_shader_));

        renderProgram_->link();

        loc_vertexPosition_ = renderProgram_->attributeLocation("vertexPosition");

        renderProgram_->bind();
        loc_projection_     = renderProgram_->uniformLocation("projectionMatrix");
        loc_worldMatrix_    = renderProgram_->uniformLocation("worldMatrix");
        loc_viewMatrix_     = renderProgram_->uniformLocation("viewMatrix");
        loc_viewportMatrix_ = renderProgram_->uniformLocation("viewportMatrix");

        loc_Col0_ = renderProgram_->uniformLocation("col0");
        loc_Col1_ = renderProgram_->uniformLocation("col1");

        loc_zLow_ = renderProgram_->uniformLocation("zLow");
        loc_zHigh_ = renderProgram_->uniformLocation("zHigh");

        // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
        // implementations this is optional and support may not be present
        // at all. Nonetheless the below code works in all cases and makes
        // sure there is a VAO when one is needed.
        vao_.create();
        QOpenGLVertexArrayObject::Binder vaoBinder(&vao_);

        // Store the vertex attribute bindings for the program.
        setupVertexAttribs();

        renderProgram_->release();
    }
} /*}}}*/

auto GroundRenderer::loadNewPoints() -> void // {{{
{
    if (!renderProgram_ && !computeProgram_) return;

    tmpBuffer_.lock();
    if (!tmp_point_buffer_.size()) {
        tmpBuffer_.unlock();
        return;
    }

    if (!computeProgram_->bind()) {
        qWarning("[GroundRenderer::loadNewPoints] Could not bind compute program");
        return;
    }

    if (!pointOpenGLBuffer_.bind()) {
        qWarning("[GroundRenderer::loadNewPoints]: Couldn't bind buffer to context");
        return;
    }

    auto point_count = tmp_point_buffer_.size();
    pointOpenGLBuffer_.write(&tmp_point_buffer_[0], point_count * sizeof(PointType));

    // Clean our vector
    tmp_point_buffer_.clear();

    // Unlock our vector
    tmpBuffer_.unlock();

    pointOpenGLBuffer_.release();
} // }}}

auto GroundRenderer::compute() -> void // {{{
{
    if (!computeProgram_->bind())
    {
        qWarning("Could not bind compute program");
        return;
    }

    // This function is guarded against running without a ship_, but just in case..
    if (ship_)
    {
        auto shipPosition = QQmlProperty::read(ship_, "position").value<QVector3D>();
        auto transform = last_pos_ - shipPosition;
        last_pos_ = shipPosition;
        computeProgram_->setUniformValue(loc_transform_, transform);
    }
    else
    {
        computeProgram_->setUniformValue(loc_transform_, QVector3D(0.0f, 0.0f, 0.0f));
    }

    // Bind the buffer to the OpenGL Context
    if (!pointOpenGLBuffer_.bind())
    {
        qWarning("Could not bind pointOpenGLBuffer");
        return;
    }

    // Bind the buffer to the shader.  See http://stackoverflow.com/a/42453021/1861346
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pointOpenGLBuffer_.bufferId());

    // Perform computation.  The topology is hard programmed (1024 being the
    // maximum number of guaranteed shaders available on any GPU) until there
    // is a need to make it customizable.
    glDispatchCompute(1024, 1, 1);

    pointOpenGLBuffer_.release();
} // }}}

auto GroundRenderer::paintGL() -> void // {{{
{
    // Bind the buffer to the OpenGL Context
    if (!pointOpenGLBuffer_.bind()) {
        qWarning("Could not bind pointOpenGLBuffer");
        return;
    }

    if (pointOpenGLBuffer_.HasData()) {

        QOpenGLVertexArrayObject::Binder vaoBinder(&vao_);
        renderProgram_->bind();

        glEnable(GL_PROGRAM_POINT_SIZE);
        renderProgram_->setUniformValue(loc_Col0_,  col0_);
        renderProgram_->setUniformValue(loc_Col1_,  col1_);
        renderProgram_->setUniformValue(loc_zLow_,  static_cast<GLfloat>(zLow_));
        renderProgram_->setUniformValue(loc_zHigh_, static_cast<GLfloat>(zHigh_));
        renderProgram_->setUniformValue("zRangeI",  static_cast<GLfloat>(1/(zHigh_-zLow_)));

        renderProgram_->setUniformValue("pointSize", static_cast<GLfloat>(pointSize_));
        renderProgram_->setUniformValue("maxDistance", 1/1500.0f); // TODO this will be removed when an actual colour scheme is implemented.

        foreach (auto viewport, viewports_)
        {
            // Only render this if the viewport is active.
            if (viewport->active())
            {
                // Configure out viewport (x,y, width, height)
                auto vsize = viewport->paddedRect();
                glViewport(vsize.x(), vsize.y(), vsize.width(), vsize.height());

                // Grab our camera
                auto camera = viewport->camera();

                QMatrix4x4 worldMatrix;  // https://open.gl/transformations#TransformationsinOpenGL
                                         // this is their Model matrix.  It basically
                                         // transforms the points into the world frame,
                                         // but because our points are currently in the
                                         // world frame, it's left as the identity matrix

                QMatrix4x4 viewMatrix(camera->viewMatrix());
                QMatrix4x4 viewportMatrix(viewport->viewportMatrix());
                QMatrix4x4 projectionMatrix = camera->projectionMatrix();

                // - ModelView matrix is the concatenation of Model matrix and View Matrix.
                // - View Matrix defines the position (location and orientation) of the
                //   camera, while model matrix defines the frame's position of the
                //   primitives you are going to draw.
                // - Projection matrix defines the characteristics of your camera,
                //   such as clip planes, field of view, projection method etc.  The
                //   camera position is not a factor in this matrix

                renderProgram_->setUniformValue(loc_viewMatrix_, viewMatrix);
                renderProgram_->setUniformValue(loc_worldMatrix_, worldMatrix);
                renderProgram_->setUniformValue(loc_projection_, projectionMatrix);
                renderProgram_->setUniformValue(loc_viewportMatrix_, viewportMatrix);

                glDrawArrays(GL_POINTS, 0, vertexCount());
            }
        }

        renderProgram_->release();
    }

    pointOpenGLBuffer_.release();


    // Not sure if this is strictly needed for this example, but generally
    // useful for when mixing with raw OpenGL.
    window_->resetOpenGLState();

} // }}}

auto GroundRenderer::setupVertexAttribs() -> void
{
    pointOpenGLBuffer_.bind();
    glEnableVertexAttribArray(loc_vertexPosition_);

    // 0 = Offset from the start of the buffer object
    // 3 = the number of components to be updated per vertex, and can be either
    //     1, 2, 3, 4.  I dunno why it's 3, but that works.  The value should
    //     be 4 IMHO because we have four components, and the vertex shader
    //     expects a vec4.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, byteStride(), 0);

    pointOpenGLBuffer_.release();
}

auto GroundRenderer::setViewport(const QRectF& vp) -> const QSize {
    const auto s = window_->size() * window_->devicePixelRatio();
    const auto x      = s.width()  * vp.x();
    const auto y      = s.height() * vp.y();
    const auto width  = s.width()  * (vp.width()  - vp.x());
    const auto height = s.height() * (vp.height() - vp.y());
    glViewport(x, y, width, height);

    return QSize(width, height);
}

auto GroundRenderer::vertexComponents() const -> unsigned int
{
    return n_vertexComponents;
}

auto GroundRenderer::vertexCount() const -> unsigned int
{
    auto s = pointOpenGLBuffer_.SizeInBytes();
    if (s > 0)
    {
        return s/byteStride();
    }
    else
    {
        return 0;
    }
}

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos foldmethod=marker : */

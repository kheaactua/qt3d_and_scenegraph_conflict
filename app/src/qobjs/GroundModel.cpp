#include "app/qobjs/GroundModel.h"

#include <chrono>
#include <thread>

#include <QtGlobal>

#include "app/Memory.hpp"
#include "app/Point.h"

GroundModel::GroundModel()
{
    connect(this, &QQuickItem::windowChanged, this, &GroundModel::handleWindowChanged);
}

#ifdef DEBUG_POINTS
auto GroundModel::SimulateInitialPoints() -> void
{
    // Set up a plane as a test
    auto len = 50.0f;
    auto points_per_side = 10u;
    Coord<float> blc{-len/2.0f, -len/2.0f, 2};

    std::vector<GroundModel::PointType> points;
    auto idx = 0;
    auto step = len/(points_per_side - 1);

    for (unsigned int i = 0; i<points_per_side; i++)
    {
        for (unsigned int j = 0; j<points_per_side; j++)
        {
            points.emplace_back(
                blc.x + (static_cast<float>(i * step)),
                blc.y + (static_cast<float>(j * step)),
                blc.z
            );
            idx++;
        }
    }

    Q_EMIT NewPoints(points);
}

#endif

auto GroundModel::cleanup() -> void
{
    if (renderer_)
    {
        renderer_.reset(nullptr);
    }
}

auto GroundModel::componentComplete() -> void
{
    // Let's see if we can see our viewports
    viewports_ = findChildren<Viewport*>();
}

auto GroundModel::handleWindowChanged(QQuickWindow* win) -> void
{
    if (win)
    {
        connect(win, &QQuickWindow::beforeSynchronizing,   this, &GroundModel::sync,    Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &GroundModel::cleanup, Qt::DirectConnection);


        // The default behavior of the scene graph is to clear the framebuffer
        // before rendering. Since we render before the scene graph, we need to
        // turn this clearing off. This means that we need to clear ourselves
        // in the paint() function.
        win->setClearBeforeRendering(false);
    }
}

auto GroundModel::sync() -> void
{
    if (deactive_points_)
    {
        if (renderer_) cleanup();

        return;
    }

    if (!renderer_)
    {
        renderer_ = App::Memory::make_unique<GroundRenderer>(this);
        connect(window(), &QQuickWindow::beforeRendering, renderer_.get(), &GroundRenderer::paint, Qt::DirectConnection);
    }

    renderer_->setWindow(window());
    renderer_->setPointSize(pointSize_);
    renderer_->setByteStride(byte_stride_);
    renderer_->setBufferPointCapacity(max_buffer_point_capacity_);
    renderer_->setComputeShader(compute_shader_);
    renderer_->setVertexShader(vertex_shader_);
    renderer_->setFragmentShader(fragment_shader_);
    renderer_->setShip(ship_);
    renderer_->setViewports(viewports_);
}

auto GroundModel::setShip(Qt3DCore::QEntity* ship) -> void
{
    if (ship)
    {
        ship_ = ship;
    }
    else
    {
        qWarning("Could not set ship on Ground Model");
        ship_ = nullptr;
    }
};

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */

#include "app/qobjs/Viewport.h"

#include <algorithm>

#include <QQuickWindow>

Viewport::Viewport() {
}

auto Viewport::componentComplete() -> void
{
    viewportMatrix_.setToIdentity();
    viewportMatrix_.viewport(rect_);
};

auto Viewport::setNormalizedRect(const QRectF& rect) -> void
{
    rect_ = rect;
    viewportMatrix_.setToIdentity();
    viewportMatrix_.viewport(rect_);

    // Is this viewport now big enough to be active?
    // The threashold is just some arbitrary number.  Note the width/height is
    // normalized between [0,1]
    const auto threshold = 0.05f;
    setActive(rect_.width() > threshold && rect_.height() > threshold);
    Q_EMIT activeChanged(active_);

    Q_EMIT normalizedPaddedRectChanged(normalizedPaddedRect());
}

auto Viewport::paddedRect() const -> QRectF
{
    if (!window()) return QRectF(0, 0, 1, 1);

    // Get the window size
    const auto s = window()->size() * window()->devicePixelRatio();

    const auto x      = (s.width()   * rect_.x())     + paddingLeft_;
    const auto width  = (s.width()   * rect_.width()) - paddingLeft_;

    /*
       Given Y is offset from the top, we convert it to yp (y^prime) which is
       with reference to the bottom

    y1 = 0 +-----------------+
           |                 |  height  = 8
           |                 |  padding = 2
           + ^ padding       |
           |                 |
           |                 |
           |                 |
           |                 |
           |                 |
    y2 = 8 +-----------------+  = yp

    */

    const auto y2 = (s.height()  * (rect_.y() + rect_.height()));
    const auto yp = s.height() - y2; // inverse the reference frame
    const auto yh = (s.height() * rect_.height()) - paddingTop_;

    QRect ret(x, yp, width, yh);
    return ret;
}

auto Viewport::normalizedPaddedRect() const -> QRectF
{
    if (!window()) return QRectF(0, 0, 1, 1);

    const auto s = window()->size() * window()->devicePixelRatio();

    const auto normLeft = paddingLeft_/s.width();
    const auto normTop = paddingTop_/s.height();

    return QRectF(
        rect_.x() + normLeft,
        rect_.y(),
        std::max(rect_.width() - normLeft, 0.001), // When this is 0-, the viewport defaults to taking up 100% of the window, so this ensures it never goes below the arbitrary threshold 0.001
        rect_.height() - normTop
    );
}

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */

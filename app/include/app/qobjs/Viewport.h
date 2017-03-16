#ifndef VIEWPORT_H_HWIRR2W3
#define VIEWPORT_H_HWIRR2W3

#include <QQuickItem>
#include <QObject>
#include <QRectF>
#include <QCamera>
#include <QMatrix4x4>
#include <QQmlParserStatus>

class Viewport : public QQuickItem
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QRectF normalizedRect READ normalizedRect WRITE setNormalizedRect NOTIFY normalizedRectChanged)
    Q_PROPERTY(QRectF normalizedPaddedRect READ normalizedPaddedRect NOTIFY normalizedPaddedRectChanged)
    Q_PROPERTY(Qt3DRender::QCamera* camera READ camera WRITE setCamera NOTIFY cameraChanged)

    /**
     * Flag used to deactivate rendering in this viewport.  This is used from
     * the side viewports are collapsed.
     */
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

    /**
     * Inner viewport padding.  This is effectively the distance between the
     * start of the viewport partition (where the slider bar is) and the width
     * of the viewport slider.
     */
    Q_PROPERTY(qreal paddingLeft READ paddingLeft WRITE setPaddingLeft)
    Q_PROPERTY(qreal paddingTop READ paddingTop WRITE setPaddingTop)

    /**
     * Name of the viewport.  Primarily used for debugging.
     */
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    Viewport();
    ~Viewport() = default;

    /**
     * Perform some initialization here now that the object is fully created
     */
    virtual auto componentComplete() -> void override;

    /**
     * Invoked after class creation, but before any properties have been set.
     */
    virtual auto classBegin() -> void override {};

    auto active() const -> bool { return active_; };
    auto setActive(const bool active) -> void { active_ = active; };

    auto paddingLeft() const -> qreal { return paddingLeft_; }
    auto setPaddingLeft(const qreal pad) -> void { paddingLeft_ = pad; };

    auto paddingTop() const -> qreal { return paddingTop_; }
    auto setPaddingTop(const qreal pad) -> void { paddingTop_ = pad; };

    auto normalizedRect() const -> QRectF { return rect_; }

    /**
     * Returns the non-normalized rect taking padding into account.  The values
     * returned here are from the BOTTOM left.
     */
    auto paddedRect() const -> QRectF;

    /**
     * Returns the normalized padding rect
     */
    auto normalizedPaddedRect() const -> QRectF;

    /**
     * Set the normalized rect for the viewport.  Like the basic QML viewport,
     * this Viewport's x/y start at the TOP/left, not bottom left.
     */
    auto setNormalizedRect(const QRectF& rect) -> void;

    auto name() const -> QString { return name_; }
    auto setName(const QString& name) -> void { name_ = name; }

    Qt3DRender::QCamera* camera() { return camera_; }
    auto setCamera(Qt3DRender::QCamera* camera) -> void { camera_ = camera; }

    auto viewportMatrix() const -> const QMatrix4x4 { return viewportMatrix_; }

signals:
    void normalizedRectChanged(QRectF rect);
    void normalizedPaddedRectChanged(QRectF rect);
    void activeChanged(bool active);
    void cameraChanged();

private:
    QRectF rect_{qreal(0), qreal(0), qreal(1), qreal(1)};
    Qt3DRender::QCamera* camera_;

    /**
     * Viewport matrix.
     *
     * Note, default construction yeilds an identity matrix
     */
    QMatrix4x4 viewportMatrix_;

    /**
     * Active flag.  Our OpenGL scene will check this to see whether this
     * viewport should be rendered.
     */
    bool active_ = false;

    /** Inner left viewport padding in pixels */
    qreal paddingLeft_ = 0;

    /** Inner right viewport padding in pixels */
    qreal paddingTop_ = 0;

    /** Viewport name.  Primarily a debugging tool */
    QString name_;
};

#endif /* end of include guard: VIEWPORT_H_HWIRR2W3 */

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */

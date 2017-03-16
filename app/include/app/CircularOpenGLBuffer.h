#ifndef CIRCULAROPENGLBUFFER_H_P3ZIZVXT
#define CIRCULAROPENGLBUFFER_H_P3ZIZVXT

#include <QOpenGLBuffer>

/**
 * Circualr OpenGL buffer.  Reads and writes will start at a certain offset,
 * and will handle circling over the buffer if it loops.  Note, the read/write
 * members have different signatures, meaning regular QOpenGLBuffer read/writes
 * could still be performed
 *
 * TODO Implement reading function. (currently nothing reads, so it just isn't
 *      defined)
 */
class CircularOpenGLBuffer : public QOpenGLBuffer
{

public:

    CircularOpenGLBuffer()
        : QOpenGLBuffer()
    {}
    CircularOpenGLBuffer(QOpenGLBuffer::Type type)
        : QOpenGLBuffer(type)
    {}

    virtual ~CircularOpenGLBuffer() = default;

    /**
     * Write data to the current location.  Not the buffer must be bound for
     * the write operation to work
     */
    auto write(const void* data, unsigned int count) -> void;

    /**
     * Zero buffer with floats.  This will fill the buffer with zero floats
     * without increasing the size.  The buffer must be bound and allocated for
     * this to work.
     *
     * This will also reset the buffer size (not capacity though) and position
     */
    auto ZeroGLfloat() -> void;

    /**
     * Allocate the buffer.  This will adjust the circular parameters, and
     * clear any contents (via calling QOpenGLBuffer::allocate)
     *
     * Note: QOpenGLBuffer does not mark these members as virtual, so I can't
     *       specify the overwrite
     */
    auto allocate(const void* data, int count) -> void;
    auto allocate(int count) -> void;

    /**
     * Set the maximum capacity of the the buffer in bytes. This does NOT
     * allocate the space, this is a simply setter.
     */
    auto setMaxCapacity(const unsigned int max_capacity) -> void;

    /**
     * Allocates the buffer to max_capacity.  The buffer must be bound for
     * this operation to work.  Also, as it calls QOpenGLBuffer::allocate,
     * all existing contents of the buffer will be lost.
     */
    auto Allocate() -> bool;

    /**
     * Simple function that returns whether the buffer has data or not. (based
     * on the cached size, this doesn't perform the actual check that would
     * require that the buffer be bound the the current context.)
     */
    auto HasData() -> bool { return size_ > 0; }

    /**
     * Returns the number of bytes in the buffer.  This should be used instead
     * of QOpenGLBuffer::size() has it can return the cached size (so it
     * doesn't need to be bound), and returns the size of the buffer being
     * used, not the maximum capacity
     */
    auto SizeInBytes() const -> unsigned int { return size_; }

    /**
     * Getter for max capacity (bytes)
     */
    auto maxCapacity() -> unsigned int { return max_capacity_; }

protected:

    /**
     * Reset the size and position to zero, or rather bring the pointer back to
     * the start of the circle.
     */
    auto ResetPosition() -> void
    {
        size_ = 0;
        pos_ = 0;
    };

    /**
     * Maximum capcity of circular buffer.  We will not allocate beyond this
     * point.  The initial value is simply a guess at a decent value.
     */
    unsigned int max_capacity_ = 1024*200*  4*sizeof(GLfloat);

    /**
     * Current size of buffer.  Basically how many bytes are currently in the
     * buffer.  During a read/write, the pointer will loop once it reaches this
     * number.
     *
     * Currently this can only increase, or be reset to zero when an allocation
     * is performed.
     */
    unsigned int size_ = 0;

    /**
     * Pointer to current position in the buffer
     */
    unsigned int pos_ = 0;
};

#endif /* end of include guard: CIRCULAROPENGLBUFFER_H_P3ZIZVXT */

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */

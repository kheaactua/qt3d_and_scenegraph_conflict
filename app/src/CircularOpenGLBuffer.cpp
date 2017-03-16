
#include "app/CircularOpenGLBuffer.h"

#include <cassert>
#include <algorithm>
#include <QtGlobal>
#include <QDebug>

auto CircularOpenGLBuffer::write(const void* data, unsigned int count) -> void
{
    Q_ASSERT_X(max_capacity_ > 0, "Writing", "A zero max capacity will cause an infinit loop");

    // Position in our source data
    auto dp = reinterpret_cast<const unsigned char*>(data);

    // TODO improve this so that if we loop, only write what we absolutely have
    //      to
    while (count>0) {
        unsigned int remaining_space = std::min(max_capacity_ - pos_, count);
        if (!remaining_space) {
            // We're at the end of the loop
            pos_=0;
            continue;
        }
        QOpenGLBuffer::write(pos_, dp, remaining_space);
        pos_  += remaining_space % max_capacity_;
        dp    += remaining_space;
        size_  = std::min(size_ + remaining_space, max_capacity_);
        count -= remaining_space;
    }
    // qDebug() << "[CircularOpenGLBuffer::write] size after write = " << size_ << " B / " << (size_/(sizeof(GLfloat) * 4)) << " points (" << (size_/float(max_capacity_)*100.0f) << "% full)";
}

auto CircularOpenGLBuffer::ZeroGLfloat() -> void
{
    // Pack this with zeros
    auto zero_count = maxCapacity()/sizeof(GLfloat);
    auto zeros = new GLfloat[zero_count]();
    QOpenGLBuffer::write(0, zeros, zero_count * sizeof(GLfloat));
    delete[] zeros;

    ResetPosition();
}


auto CircularOpenGLBuffer::setMaxCapacity(unsigned int max_capacity) -> void
{
    max_capacity_ = max_capacity;
}

auto CircularOpenGLBuffer::Allocate() -> bool
{
    if (QOpenGLBuffer::size() != static_cast<int>(max_capacity_))
    {
        QOpenGLBuffer::allocate(max_capacity_);
        if (QOpenGLBuffer::size() == -1)
        {
            qWarning("[CircularOpenGLBuffer::Allocate]: Could not allocate buffer.");
            return false;
        }
        else
        {
            qDebug() << "[CircularOpenGLBuffer::Allocate]: Allocated " << QOpenGLBuffer::size() << " bytes for circular buffer.";
        }

        ResetPosition();
    }
    return true;
}

auto CircularOpenGLBuffer::allocate(const void* data, int count) -> void
{
    QOpenGLBuffer::allocate(data, count);
    ResetPosition();
}

auto CircularOpenGLBuffer::allocate(int count) -> void
{
    QOpenGLBuffer::allocate(count);
    ResetPosition();
}

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */

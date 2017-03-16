#ifndef POINT_H_1QMINBUZ
#define POINT_H_1QMINBUZ

#include <array>
#include <iomanip>
#include <iostream>
#include <tuple>

#include <Qt3DRender/QAttribute>

/**
 * Out point structure used internally in the C++, and that is read into Qt
 * QByteArrays.
 *
 * Currently, colours should be input as a value between 0-255
 *
 * @param VexSize Length of the position and colour vectors.  Again, after more
 *                testing, this might be removed and hard coded in. (TODO)
 * @param Vec3Len Length of the vec3s we're using, this is set to four for
 *                performance reasons.
 *
 * The resulting point & Qt byte representation should be:
 *   --------------------------------------------------------------
 *   float, float, float, float : x,y,z, BLANK   : 4 * 4 = 16 bytes
 *   double                     : timestamp (us) : 8 * 0 =  0
 *   unsigned short             : intensity      : 2 * 0 =  0
 *   unsigned char              : padding        :
 *   --------------------------------------------------------------
 *                              :                :         16 bytes
 *
 *  TODO: @hkwok seems to know more about SIMD than me, ask him if I should
 *        pad this out to 64 bits.  I'm already padding the coords out...
*/

template <size_t VertexSize=3, size_t Vec3Len=4>
struct Point
{
    using PositionComponent = float;
    using Position   = std::array<PositionComponent, Vec3Len>; // If vec length isn't divisible by 4,
                                                               // expect weird padding for subsequent
                                                               // components.
    using Timestamp = double;  // TODO this is the time in microsecons, but there's no conceivable
                               // use for that, more appropriate would be to save an age in the w
                               // component of position as a float.  This is because it's more
                               // important to know if the point is a few seconds old than it is to
                               // know if it's a few decades old (which will never happen,
                               // especially with a finite buffer size.)
    using Intensity  = unsigned short;
    // using Padding    = std::array<char, 32 - sizeof(Position) - sizeof(Timestamp) - sizeof(Intensity)>;
    using Padding    = std::array<char, 16 - sizeof(Position)>;

    /** XYZ coordinate of the point */
    Position pos;

    /** Point timestamp (microseconds) */
    // Timestamp timestamp = 0;

    /** Intensity of the laser return */
    // Intensity intensity = 0;

    /** Stritly here for alignment, aiming for a total size of 32 bytes */
    // Padding padding;


    /**
     * ENUM of all the paramters on a point
     */
    enum class Attribute : int {
        Position,
        Timestamp,
        Intensity,
        Padding,
    };

    /**
     * ENUM of the size of the Qt attributes.  So a char = 1, double = 4, etc..
     */
    enum class AttributeSize : int {
        Position  = sizeof(PositionComponent),
        Timestamp = sizeof(Timestamp),
        Intensity = sizeof(Intensity),
        Padding   = sizeof(Padding),
    };

    /**
     * ENUM of the length of the attributes.  This will be 1 for scalars, and n
     * for a vector of length n
     */
    enum class AttributeLength : int {
        Position  = Vec3Len,
        Timestamp = 1,
        Intensity = 1,
        Padding   = 1,
    };

    /**
     * ENUM of the vertex size held in each array.  TODO when I figure out why this is different than the length, explain that here.
     */
    enum class AttributeVertexSize : int {
        Position  = VertexSize,
        Timestamp = 1,
        Intensity = 1,
        Padding   = 1,
    };

    /**
     * Offsets to find data in the eventual QByteArray
     */
    enum class AttributeOffset : int {
        Position  = 0,
        // Timestamp = (static_cast<int>(AttributeSize::Position) * static_cast<int>(AttributeLength::Position)),
        // Intensity = (static_cast<int>(AttributeSize::Position) * static_cast<int>(AttributeLength::Position)) + (static_cast<int>(AttributeSize::Timestamp) * static_cast<int>(AttributeLength::Timestamp)),
        // Padding   = (static_cast<int>(AttributeSize::Position) * static_cast<int>(AttributeLength::Position)) + (static_cast<int>(AttributeSize::Timestamp) * static_cast<int>(AttributeLength::Timestamp)) + (static_cast<int>(AttributeSize::Position) * static_cast<int>(AttributeLength::Intensity)),
        Padding   = (static_cast<int>(AttributeSize::Position) * static_cast<int>(AttributeLength::Position)),
    };


    /**
     * Default value is all zeros.  The code recognizes this as an unintialzied
     * point, and won't copy it into the QBuffer.
     */
    Point()
    {
        pos.fill(0);
        // timestamp = 0;
        // intensity = 0;
    };

    /**
     * Constructor from x, y, z
     */
    Point(const PositionComponent x, const PositionComponent y, const PositionComponent z)
    {
        setPos(x, y, z);
    }

    /**
     * C++ to Qt Type:  Return the proper Qt type to be used for a given input
     * (Point::Attribute::*)  This is conceptually a hash map.
     *
     * The types given by this map MUST match (in size) to the types we use in
     * this structure.  This is because often for writing, the sizeof() opp is
     * used to decide out much to copy, and we can't do a sizeof() on the Qt
     * ENUM types.
     */
    static auto ParameterQtType(const typename Point<VertexSize, Vec3Len>::Attribute attr) -> Qt3DRender::QAttribute::VertexBaseType
    {
        switch (attr) {
            case Point<VertexSize, Vec3Len>::Attribute::Position:
                return Qt3DRender::QAttribute::Float;
            case Point<VertexSize, Vec3Len>::Attribute::Timestamp:
                return Qt3DRender::QAttribute::Double;
            case Point<VertexSize, Vec3Len>::Attribute::Intensity:
                return Qt3DRender::QAttribute::UnsignedShort;
            case Point<VertexSize, Vec3Len>::Attribute::Padding:
                return Qt3DRender::QAttribute::Byte;
            default:
                throw std::string("Unhandled point parameter type requested.");
                return Qt3DRender::QAttribute::Float;
        }
    }

    private:

     template <std::size_t VL = Vec3Len>
     auto setPos(const PositionComponent x, const PositionComponent y, const PositionComponent z) ->
         typename std::enable_if<VL == 3, void>::type
     {
         pos[0] = x;
         pos[1] = y;
         pos[2] = z;
     }

     template <std::size_t VL = Vec3Len>
     auto setPos(const PositionComponent x, const PositionComponent y, const PositionComponent z) ->
         typename std::enable_if<VL == 4, void>::type
     {
         pos[0] = x;
         pos[1] = y;
         pos[2] = z;
         pos[3] = PositionComponent(0);
     }
};


template <size_t VertexSize=3, size_t Vec3Len=4>
auto operator<<(std::ostream& ss, const Point<VertexSize, Vec3Len>& point) -> std::ostream&
{
    ss << "p[" << std::setprecision(5) <<  point.pos[0] << "," << point.pos[1] << "," << point.pos[2] << "]";
    return ss;
}


/**
 * Small container for coordiantes.  Mostly used in testing functions right now
 *
 * TODO evaluate if this is still needed.  Right now it's only used in testing
 *      code
 */
template <typename T = float>
struct Coord
{
    T x = T(0);
    T y = T(0);
    T z = T(0);

    Coord() = default;

    Coord(const T x_in, const T y_in, const T z_in)
    {
        x = x_in;
        y = y_in;
        z = z_in;
    }
};

#endif /* end of include guard: POINT_H_1QMINBUZ */

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */

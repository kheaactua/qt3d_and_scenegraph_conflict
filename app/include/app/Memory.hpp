#ifndef APPMEMORY_HPP_ZTWD3VEK
#define APPMEMORY_HPP_ZTWD3VEK

#include <memory>

namespace App
{
namespace Memory
{

/**
 * This is a "use future" class to enable us to use c++14's
 * version of make_unique
 */
template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params)
{
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

} // end Memory
} // end App

#endif /* end of include guard: APPMEMORY_HPP_ZTWD3VEK */

// vim: sw=4 ts=4 sts=4 expandtab ffs=unix :

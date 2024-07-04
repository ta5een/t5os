#pragma once

namespace lib
{

template <class T>
struct RemoveReference
{
    using Type = T;
};

template <class T>
struct RemoveReference<T &>
{
    using Type = T;
};

template <class T>
struct RemoveReference<T &&>
{
    using Type = T;
};

template <typename T>
typename RemoveReference<T>::Type &&move(T &&arg)
{
    return static_cast<typename RemoveReference<T>::Type &&>(arg);
}

} // namespace lib

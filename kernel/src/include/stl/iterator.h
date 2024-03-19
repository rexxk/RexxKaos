#pragma once

#include "core/types.h"


struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};


template<typename _Category, typename _Tp, typename _Distance = ptrdiff_t, typename _Pointer = _Tp*, typename _Reference = _Tp&>
struct iterator
{
    typedef _Category iterator_category;
    typedef _Tp value_type;
    typedef _Distance distance_type;
    typedef _Pointer pointer;
    typedef _Reference reference;
};


template<typename _Iterator>
struct iterator_traits
{
    typedef typename _Iterator::iterator_category iterator_category;
    typedef typename _Iterator::value_type value_type;
    typedef typename _Iterator::distance_type distance_type;
    typedef typename _Iterator::pointer pointer;
    typedef typename _Iterator::reference reference;
};


template<typename _Tp>
struct iterator_traits<_Tp*>
{
    typedef random_access_iterator_tag iterator_category;
    typedef _Tp value_type;
    typedef ptrdiff_t distance_type;
    typedef _Tp* pointer;
    typedef _Tp& reference;
};

template<typename _Tp>
struct iterator_traits<const _Tp*>
{
    typedef random_access_iterator_tag iterator_category;
    typedef _Tp value_type;
    typedef ptrdiff_t distance_type;
    typedef const _Tp* pointer;
    typedef const _Tp& reference;
};

template<typename _Iter>
inline typename iterator_traits<_Iter>::iterator_category __iterator_category(const _Iter&)
{
    return typename iterator_traits<_Iter>::iterator_category();
}

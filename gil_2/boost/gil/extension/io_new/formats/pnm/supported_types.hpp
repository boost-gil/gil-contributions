/*
    Copyright 2008 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_PNM_SUPPORTED_TYPES_HPP
#define BOOST_GIL_EXTENSION_IO_PNM_SUPPORTED_TYPES_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning \n
///
/// \date 2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/mpl/not.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/gil/channel.hpp>
#include <boost/gil/color_base.hpp>

namespace boost { namespace gil { namespace detail {

// Read Support

template< pnm_image_type::type ASCII_Type
        , pnm_image_type::type Binary_Type
        >
struct png_rw_support_base
{
    static const pnm_image_type::type _asc_type = ASCII_Type;
    static const pnm_image_type::type _bin_type = Binary_Type;
};

template< typename Channel
        , typename ColorSpace
        >
struct pnm_read_support : read_support_false
                        , png_rw_support_base< 0
                                             , 0
                                             > {};

template< typename BitField, bool Mutable >
struct pnm_read_support< packed_dynamic_channel_reference< BitField
                                                         , 1
                                                         , Mutable
                                                         >
                       , gray_t
                       > : read_support_true
                         , png_rw_support_base< pnm_type_mono_asc
                                              , pnm_type_mono_bin
                                              > {};

template<>
struct pnm_read_support< bits8
                       , gray_t
                       > : read_support_true
                         , png_rw_support_base< pnm_type_gray_asc
                                              , pnm_type_gray_bin
                                              > {};


template<>
struct pnm_read_support< bits8
                       , rgb_t
                       > : read_support_true
                         , png_rw_support_base< pnm_type_color_asc
                                              , pnm_type_color_bin
                                              > {};

// Write support

template< typename Channel
        , typename ColorSpace
        >
struct pnm_write_support : write_support_false
{};

template< typename BitField, bool Mutable >
struct pnm_write_support< packed_dynamic_channel_reference< BitField
                                                          , 1
                                                          , Mutable
                                                          >
                        , gray_t
                        > : write_support_true
                          , png_rw_support_base< pnm_type_mono_asc
                                               , pnm_type_mono_bin
                                               > {};


template<>
struct pnm_write_support< bits8
                        , gray_t
                        > : write_support_true
                          , png_rw_support_base< pnm_type_gray_asc
                                               , pnm_type_gray_bin
                                               > {};


template<>
struct pnm_write_support< bits8
                        , rgb_t
                        > : write_support_true
                          , png_rw_support_base< pnm_type_color_asc
                                               , pnm_type_color_bin
                                               > {};

} // namespace detail

template< typename Pixel >
struct is_read_supported< Pixel
                        , pnm_tag
                        >
    : mpl::bool_< detail::pnm_read_support< typename channel_type< Pixel >::type
                                          , typename color_space_type< Pixel >::type
                                          >::is_supported
                >
{
    typedef detail::pnm_read_support< typename channel_type< Pixel >::type
                                    , typename color_space_type< Pixel >::type
                                    > parent_t;

    static const pnm_image_type::type _asc_type = parent_t::_asc_type;
    static const pnm_image_type::type _bin_type = parent_t::_bin_type;
};

template< typename Pixel >
struct is_write_supported< Pixel
                         , pnm_tag
                         >
    : mpl::bool_< detail::pnm_write_support< typename channel_type< Pixel >::type
                                           , typename color_space_type< Pixel >::type
                                           >::is_supported
                > {};

} // namespace gil
} // namespace boost


#endif // BOOST_GIL_EXTENSION_IO_PNM_SUPPORTED_TYPES_HPP

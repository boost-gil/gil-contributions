/*
    Copyright 2007-2008 Christian Henning, Andreas Pokorny
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_TIFF_SUPPORTED_TYPES_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_TIFF_SUPPORTED_TYPES_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief 
/// \author Christian Henning and Andreas Pokorny \n
///         
/// \date   2007-2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

namespace boost { namespace gil { namespace detail {

struct tiff_rw_support {
    BOOST_STATIC_CONSTANT(bool,is_supported=true);
};

} // namespace detail
} // namespace gil
} // namespace boost


#endif // BOOST_GIL_EXTENSION_IO_TIFF_SUPPORTED_TYPES_HPP_INCLUDED

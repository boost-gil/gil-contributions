/*
    Copyright 2008 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_PNM_IO_READ_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_PNM_IO_READ_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief 
/// \author Christian Henning \n
///         
/// \date 2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <boost/bind.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io_new/pnm_tags.hpp>

#include "base.hpp"
#include "reader_base.hpp"
#include "io_device.hpp"
#include "typedefs.hpp"

namespace boost { namespace gil { namespace detail {

template< typename View
        , typename DummyT = void // needed for enable_if
        >
struct row_buffer_helper_
{
    typedef typename View::value_type element_t;
    typedef std::vector< element_t > buffer_t;
    typedef typename buffer_t::iterator iterator_t;

    row_buffer_helper_( int size )
        : row_buffer( size / num_channels< View >::value ) {}

    iterator_t begin() { return row_buffer.begin(); }
    iterator_t end()   { return row_buffer.end();   }

    element_t* data()  { return &row_buffer[0]; }

    buffer_t& buffer()
    {
        return row_buffer;
    }

private:

    buffer_t row_buffer;
};

template< typename View >
struct row_buffer_helper_< View
                        , typename enable_if
                                < typename mpl::and_
                                    < typename is_bit_aligned< typename View::reference >::type
                                    , typename is_homogeneous< typename View::reference >::type
                                    >::type
                                >::type
                        >
{
    typedef byte_t element_t;
    typedef std::vector< element_t > buffer_t;
    typedef bit_aligned_pixel_iterator< typename View::reference > iterator_t;


    row_buffer_helper_( int size )
    : row_buffer( size )
    {
        // number of full bytes
        _c = ( size
               * num_channels<typename View::reference>::value
               * channel_type<typename View::reference>::type::num_bits
             )
             >> 3;


        // number of remaining bits
        _r = size 
           * num_channels<typename View::reference>::value
           * channel_type<typename View::reference>::type::num_bits
           - ( _c << 3 );
    }

    iterator_t begin()
    {
        return iterator_t( &row_buffer.front(), 0 );
    }

    iterator_t end()
    {
        return ( _r == 0 )
               ? iterator_t( &row_buffer.back() + 1, 0  )
               : iterator_t( &row_buffer.back()    , _r );
    }

    element_t* data()  { return &row_buffer[0]; }

    buffer_t& buffer() { return row_buffer; }

private:

    buffer_t row_buffer;

    // number of full bytes
    int _c;

    // number of remaining bits
    int _r;

    // For instance 25 pixels of rgb2 type would be:
    // overall 25 pixels * 3 channels * 2 bits/channel = 150 bits
    // c = 18 bytes
    // r = 6 bits
 };

template<typename View,typename D = void>
struct row_buffer_helper_view_
    : row_buffer_helper_<View>
{
    row_buffer_helper_view_( int width ) 
      :  row_buffer_helper_<View>(width)
    {}
};


template<typename View>
struct row_buffer_helper_view_<View,
    typename enable_if<typename is_bit_aligned< View >::type>::type
    >
    : row_buffer_helper_<View>
{
    row_buffer_helper_view_( int width ) 
        : row_buffer_helper_<View>( width )
    {}
};


template< typename View, typename T >
struct calc_pitch {};

template< typename View >
struct calc_pitch< View, mpl::false_ >
{
    static uint32_t do_it( uint32_t width ) { return width * num_channels< View >::value; }
};

template< typename View >
struct calc_pitch< View, mpl::true_ >
{
    static uint32_t do_it( uint32_t width ) {  return ( width + 7 ) >> 3; }
};


template< typename Device
        , typename ConversionPolicy
        >
class reader< Device
            , pnm_tag
            , ConversionPolicy
            > 
    : public reader_base< pnm_tag
                        , ConversionPolicy
                        >
{
private:

    typedef typename ConversionPolicy::color_converter_type cc_t;

public:
    reader( Device& device )
    : _io_dev( device )
    {}

    reader( Device&     device
          , const cc_t& cc
          )
    : reader_base< pnm_tag
                 , ConversionPolicy >( cc )
    , _io_dev( device )
    {}

    image_read_info<pnm_tag> get_info()
    {
        image_read_info<pnm_tag> ret;

        // read signature
        io_error_if( read_char() != 'P', "Invalid PNM signature" );

        ret._type = read_char() - '0';

		io_error_if( ret._type < pnm_type_mono_asc || ret._type > pnm_type_color_bin
		           , "Invalid PNM file (supports P1 to P6)"
		           );

        ret._width  = read_int();
        ret._height = read_int();

        if( ret._type == pnm_type_mono_asc || ret._type == pnm_type_mono_bin )
        {
            ret._max_value = 1;
        }
        else
        {
            ret._max_value = read_int();

		    io_error_if( ret._max_value > 255
		               , "Unsupported PNM format (supports maximum value 255)"
		               );
        }

        return ret;
    }

    template<typename View>
    void apply( const View& view )
    {
        typedef bit_aligned_image1_type< 1, gray_layout_t >::type mono_t;

		switch( this->_info._type )
		{
            // reading mono text is reading grayscale but with only two values
			case pnm_type_mono_asc:  { read_text_data< gray8_view_t >( view ); break; }
			case pnm_type_gray_asc:  { read_text_data< gray8_view_t >( view ); break; }
			case pnm_type_color_asc: { read_text_data< rgb8_view_t  >( view ); break; } 
			
			case pnm_type_mono_bin:  { read_bin_data< mono_t::view_t >( view ); break; }
			case pnm_type_gray_bin:  { read_bin_data< gray8_view_t   >( view ); break; } 
			case pnm_type_color_bin: { read_bin_data< rgb8_view_t    >( view ); break; }
		}
    }

private:
/*
    template< typename View_Src
            , typename View_Dst
            >
    void read_text_data( const View_Dst& view )
    {
        typedef typename View_Dst::y_coord_t y_t;

        uint32_t pitch = this->_info._width * num_channels< View_Src >::value;

        std::vector< byte_t > row( pitch );
        View_Src v = interleaved_view( _info._width
                                     , 1
                                     , (typename View_Src::value_type*) &row.front()
                                     , pitch
                                     );

        typename View_Src::x_iterator beg = v.row_begin( 0 ) + this->_settings._top_left.x;
        typename View_Src::x_iterator end = beg + this->_settings._dim.x;

        char buf[16];

        for( y_t y = 0; y < view.height(); ++y )
        {
            for( uint32_t x = 0; x < pitch; ++x )
            {
                for( uint32_t k = 0; ; )
                {
					int ch = _io_dev.getc_unchecked();

					if( isdigit( ch ))
					{
                        buf[ k++ ] = ch;
					}
					else if( k )
					{
						buf[ k ] = 0;
						break;
					}
					else if( ch == EOF || !isspace( ch ))
					{
						return;
					}
                }

                row[x] = atoi( buf );
            }

            this->_cc_policy.read( beg
                                 , end
                                 , view.row_begin( y )
                                 );

        }
    }
*/
    template< typename View_Src
            , typename View_Dst
            >
    void read_text_data( const View_Dst& view )
    {
        typedef typename View_Dst::y_coord_t y_t;

        uint32_t pitch = this->_info._width * num_channels< View_Src >::value;

        std::vector< byte_t > row( pitch );
        View_Src v = interleaved_view( _info._width
                                     , 1
                                     , (typename View_Src::value_type*) &row.front()
                                     , pitch
                                     );

        typename View_Src::x_iterator beg = v.row_begin( 0 ) + this->_settings._top_left.x;
        typename View_Src::x_iterator end = beg + this->_settings._dim.x;

        char buf[16];

        for( y_t y = 0; y < view.height(); ++y )
        {
            for( uint32_t x = 0; x < pitch; ++x )
            {
                for( uint32_t k = 0; ; )
                {
					int ch = _io_dev.getc_unchecked();

					if( isdigit( ch ))
					{
                        buf[ k++ ] = ch;
					}
					else if( k )
					{
						buf[ k ] = 0;
						break;
					}
					else if( ch == EOF || !isspace( ch ))
					{
						return;
					}
                }

                // when reading mono images we need to reverse the values.
                if( this->_info._max_value == 1 )
                {
                    // 0 is white
                    row[x] = ( atoi( buf ) != 0 ) ? 0 : 255;
                }
                else
                {
                    row[x] = atoi( buf );
                }
            }

            this->_cc_policy.read( beg
                                 , end
                                 , view.row_begin( y )
                                 );

        }
    }

    template< typename View_Src
            , typename View_Dst
            >
    void read_bin_data( const View_Dst& view )
    {
        typedef typename View_Dst::y_coord_t y_t;
        typedef is_bit_aligned< View_Src::value_type >::type is_bit_aligned_t;

        uint32_t pitch = calc_pitch< View_Src, is_bit_aligned_t >::do_it( this->_info._width );

        typedef row_buffer_helper_view_< View_Src > rh_t;
        rh_t rh( pitch );

        typename rh_t::iterator_t beg = rh.begin() + this->_settings._top_left.x;
        typename rh_t::iterator_t end = beg + this->_settings._dim.x;

        // for bit_aligned images we need to negate all bytes in the row_buffer
        negate_bits< rh_t::buffer_t, is_bit_aligned_t > neg;

        for( y_t y = 0; y < view.height(); ++y )
        {
            _io_dev.read( reinterpret_cast< byte_t* >( rh.data() )
                        , pitch
                        );

            neg( rh.buffer() );

            this->_cc_policy.read( beg
                                 , end
                                 , view.row_begin( y )
                                 );
        }
    }

private:

    // Read a character and skip a comment if necessary.
    char read_char()
    {
        char ch;

        if(( ch = _io_dev.getc() ) == '#' )
        {
            // skip comment to EOL
            do
            {
                ch = _io_dev.getc();
            }
            while (ch != '\n' && ch != '\r');
        }

        return ch;
    }

	unsigned int read_int()
	{
		char ch;

        // skip whitespaces, tabs, and new lines
		do
		{
			ch = read_char();
		}
		while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

		if( ch < '0' || ch > '9' )
		{
			io_error( "Unexpected characters reading decimal digits" );
		}

		unsigned val = 0;

        do
        {
			unsigned dig = ch - '0';

			if( val > INT_MAX / 10 - dig )
			{
				io_error( "Integer too large" );
			}

			val = val * 10 + dig;

			ch = read_char();
		}
		while( '0' <= ch && ch <= '9' );

		return val;
	}

private:

    Device& _io_dev;
};

} // detail
} // gil
} // boost

#endif // BOOST_GIL_EXTENSION_IO_PNM_IO_READ_HPP_INCLUDED

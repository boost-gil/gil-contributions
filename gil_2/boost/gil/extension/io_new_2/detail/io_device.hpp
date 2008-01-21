/*
  Copyright 2007-2008 Andreas Pokorny
  Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
  or a copy at http://opensource.adobe.com/licenses.html)
*/

#ifndef BOOST_GIL_EXTENSION_IO_DETAIL_IO_DEVICE_HPP_INCLUDED
#define BOOST_GIL_EXTENSION_IO_DETAIL_IO_DEVICE_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>
#include <boost/gil/extension/io_new_2/detail/base.hpp>

namespace boost { namespace gil { namespace detail {
/*!
 * Implements the IODevice concept c.f. to \ref IODevice required by Image libraries like
 * libjpeg, libtiff and libpng. 
 *
 * \todo switch to a sane interface as soon as there is 
 * something good in boost. I.E. the IOChains library
 * would fit very well here. 
 *
 * This implementation is based on FILE*.
 */
class file_stream_device
{
public:
    struct read_tag {};
    struct write_tag {};
    file_stream_device( std::string const& file_name, read_tag )
        : file(0),_close(true)
    {
        io_error_if( ( file = fopen( file_name.c_str(), "rb" )) == NULL
                , "file_stream_device: failed to open file" );
    }

    file_stream_device( std::string const& file_name, write_tag )
        : file(0),_close(true)
    {
        io_error_if( ( file = fopen( file_name.c_str(), "wb" )) == NULL
                , "file_stream_device: failed to open file" );
    }

    file_stream_device( FILE * filep)
        : file(filep),_close(false)
    {
    }

    ~file_stream_device()
    {
        if(_close)
            fclose(file);
    }

    size_t read( unsigned char * data, int count )
    {
        return fread(data, 1, count, file );
    }

    void write( unsigned char const * data, int count )
    {
        fwrite(data, 1, count, file);
    }

    //!\todo replace with std::ios::seekdir?
    void seek( long count, int whence )
    {
        fseek(file, count, whence );
    }


    void flush()
    {
        fflush(file);
    }
private:
    file_stream_device( file_stream_device const& );
    file_stream_device& operator=( file_stream_device const& );
    FILE* file;
    bool _close;
};

/**
 * Input stream device
 */
class istream_device
{
public:
    istream_device( std::istream & in )
        : in(in)
    {
    }

    size_t read( unsigned char * data, int count )
    {
        size_t cr = 0;
        do{
            size_t c = in.readsome(reinterpret_cast<char*>(data), count );
            count -= c;
            data += c;
            cr += c;
        } while(count && in );
        return cr;
    }

    void seek( long count, int whence )
    {
        in.seekg(
                count,
                whence == SEEK_SET
                ?std::ios::beg
                :(whence == SEEK_CUR
                    ?std::ios::cur
                    :std::ios::end)
                );
    }

    void write( unsigned char const* data, int count )
    {
        //throw?
    }

    void flush() {}

private:
    std::istream& in;
};

/**
 * Output stream device
 */
class ostream_device
{
public:
    ostream_device( std::ostream & out )
        : out(out)
    {
    }

    size_t read( unsigned char * data, int count )
    {
        //throw?
    }

    void seek( long count, int whence )
    {
        out.seekp(
                count,
                whence == SEEK_SET
                ?std::ios::beg
                :(whence == SEEK_CUR
                    ?std::ios::cur
                    :std::ios::end)
                );
    }

    void write( unsigned char const* data, int count )
    {
        out.write(reinterpret_cast<char const*>(data), count);
    }

    void flush()
    {
        out << std::flush;
    }

private:
    std::ostream& out;
};


/**
 * Metafunction to detect input devices. 
 * Should be replaced by an external facility in the future.
 */
template<typename IODevice>
struct is_input_device : mpl::false_{};

template<>
struct is_input_device<file_stream_device> : mpl::true_{};
template<>
struct is_input_device<istream_device> : mpl::true_{};

template<typename T, typename D = void>
struct is_adaptable_input_device : mpl::false_{};

template<typename T>
struct is_adaptable_input_device<T,
    typename enable_if<is_base_and_derived<std::istream,T> >::type
    >
  : mpl::true_
{
    typedef istream_device device_type;
};

template<>
struct is_adaptable_input_device<FILE*,void>
  : mpl::true_
{
    typedef file_stream_device device_type;
};



/**
 * Metafunction to detect output devices. 
 * Should be replaced by an external facility in the future.
 */
template<typename IODevice>
struct is_output_device : mpl::false_{};
template<>
struct is_output_device<file_stream_device> : mpl::true_{};
template<>
struct is_output_device<ostream_device> : mpl::true_{};

template<typename IODevice,typename D=void>
struct is_adaptable_output_device : mpl::false_{};

template<typename T>
struct is_adaptable_output_device<T,
    typename enable_if<is_base_and_derived<std::ostream,T> >::type
    >
  : mpl::true_
{
    typedef ostream_device device_type;
};

template<>
struct is_adaptable_output_device<FILE*,void>
  : mpl::true_
{
    typedef file_stream_device device_type;
};


} // ns detail
} // ns gil
} // ns boost

#endif

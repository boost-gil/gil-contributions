
#include <SDL.h>

#include <boost/test/unit_test.hpp>

#include <boost/gil/extension/sdl2/service.hpp>
#include <boost/gil/extension/sdl2/window.hpp>

using namespace boost;
using namespace gil;
using namespace sdl;


struct redraw_handler
{
    template< typename View >
    void operator() ( View v )
    {
        fill_pixels( v, typename View::value_type( 255, 0, 0, 0 ));
    }
};

BOOST_AUTO_TEST_CASE( test_sdl_basic )
{
    service sdl_service;

    window< redraw_handler > w;
    sdl_service.add_window( w );

    sdl_service.run();
}

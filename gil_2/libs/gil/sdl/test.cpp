// test.cpp : Defines the entry point for the console application.
//

#pragma warning( disable: 4675 )

#include "boost/any.hpp"

#include "boost/thread/xtime.hpp"

#include "boost/gil/gil_all.hpp"

#include "boost/gil/extension/io/bmp_io.hpp"
#include "boost/gil/extension/sdl/sdl_wrapper.hpp"

using namespace std;
using namespace boost;
using namespace gil;

// called from window's thread
class draw_something : public painter_base
                     , public keyboard_event_base
{
public:

   draw_something( const rgb8_view_t& view ) : _view( view ) {}

   virtual void key_up( SDL_Surface* screen )
   {
      cout << "key up event" << endl;
   }

   virtual void render( SDL_Surface* screen )
   {
      for( int y = 0; y < screen->h; ++y )
      {
         rgb8_view_t::x_iterator x_it = _view.row_begin( y );

         for( int x = 0; x < screen->w; ++x )
         {
            const rgb8_pixel_t& p = x_it[x];

            set_pixel( p, screen, x, y );
         }
      }
   }

private:

   const rgb8_view_t& _view;
};

int main( int argc, char* argv[] )
{
   sdl_service ss;

   rgb8_image_t img;
   bmp_read_image( "flower.bmp", img );

   draw_something painter( view( img ));
   sdl_window win( view( img ).width()
                 , view( img ).height()
                 , &painter
                 , &painter               );

<<<<<<< .mine
   while( true )
   {
      // Poll for events, and handle the ones we care about.
      SDL_Event event;
      while( SDL_PollEvent( &event ))
      {
         switch (event.type) 
         {
            case SDL_KEYDOWN:
            {
               break;
            }
=======
   ss.add_window( win );
>>>>>>> .r102

<<<<<<< .mine
            case SDL_KEYUP:
            {
               // If escape is pressed, return (and thus, quit)
               if( event.key.keysym.sym == SDLK_ESCAPE )
                  return 0;

               break;
            }

            case SDL_QUIT:
            {
               return 0;
            }

         } //switch
      } // while
   } // while

=======
   ss.run();

>>>>>>> .r102
	return 0;
}
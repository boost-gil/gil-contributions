// Copyright 2007 Christian Henning.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/*************************************************************************************************/

#ifndef USER_EVENTS_HPP
#define USER_EVENTS_HPP

namespace boost { namespace gil { namespace sdl { 

struct keyboard_event_base
{
   virtual void key_up( const bgra8_view_t& sdl_view ) =0;
};

struct redraw_event_base
{
   virtual void redraw( const bgra8_view_t& sdl_view ) =0;
};

struct timer_event_base
{
   virtual void time_elapsed() =0;
};

struct quit_event_base
{
   virtual void quit() =0;
};


} } } // namespace boost::gil::sdl

#endif // GIL_SDL_CONVERTERS_HPP

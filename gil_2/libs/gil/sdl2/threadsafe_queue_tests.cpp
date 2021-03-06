/*
    Copyright 2013 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#include "stdafx.h"

#include <boost/test/unit_test.hpp>


#include <boost/thread.hpp>

#include <boost/gil/extension/sdl2/threadsafe_queue.hpp>

using namespace boost::gil::sdl;

BOOST_AUTO_TEST_CASE( test_threadsafe_queue )
{
    threadsafe_queue<int> queue;

    // create consumer thread which will print out
    // all ints from the queue
    boost::thread consumer( [&] ()
    {
        int i = 0;
        
        while( i != 69 )
        {
            queue.wait_and_pop( i );
            std::cout << i << std::endl;
        }
    });

    // create producer thread which will send the consumer some integers
    boost::thread producer( [&] () 
    {
        for( int i = 0; i < 10; ++i )
        {
            queue.push( i );
        }

        queue.push( 69 );
    });

    producer.join();
    consumer.join();
}

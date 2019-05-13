// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cassert>
#include <string>
#include <string_view>
#include <unistd.h>

#include <daw/daw_benchmark.h>

#include "daw/daw_process.h"
#include "daw/daw_collection_channel.h"

extern bool can_run;
bool can_run = true;

static std::vector<int> mul( std::vector<int> vec, int multiplier ) {
	for( int & item: vec ) {
		item *= multiplier;
	}
	return vec;
}

static void show( std::vector<int> const & vec ) {
	std::cout << "{ ";
	if( !vec.empty( ) ) {
		auto it = vec.begin( );
		std::cout << *it;
		++it;
		while( it != vec.end( ) ) {
			std::cout << ", " << *it;
			++it;
		}
	}
	std::cout << " }\n";
}

int main( ) {
	auto chan = daw::process::collection_channel<int>( );
	static std::vector<int> const message = { 2, 5, 6, 7 };

	auto proc = daw::process::fork_process(
	  [&chan]( unsigned int t ) {
	  	int multiplier = 0;
		  while( can_run ) {
			  puts( "child: sleeping\n" );
			  sleep( t );
			  puts( "child: awake\n" );
			  chan.write( mul( message, multiplier++ ) );
		  }
	  },
	  2 );

	int multiplier = 0;
	while( can_run ) {
		puts( "parent: awaiting child\n" );
		auto val = chan.read( );
		std::cout << "parent: message from child ";
		show( val );
		daw::expecting( val, mul( message, multiplier++ ) );
	}
}

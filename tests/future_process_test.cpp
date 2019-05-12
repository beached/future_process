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

#include <iostream>
#include <numeric>

#include "daw/future_process.h"

int main( ) {
	auto const a = []( int b ) {
		sleep( 5 );
		return b * b;
	};

	auto f1 = daw::process( a, 5 );
	auto f2 = daw::process( a, 10 );

	std::cout << f1.get( ) + f2.get( ) << '\n';

	std::vector<std::future<int>> futs{};

	for( size_t n = 0; n < 100; ++n ) {
		futs.push_back( daw::process( a, n ) );
	}
	auto sums = std::accumulate( futs.begin( ), futs.end( ), 0,
	                             []( int s, auto &f ) { return s + f.get( ); } );

	std::cout << "sums: " << sums << '\n';
}

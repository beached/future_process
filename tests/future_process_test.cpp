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
#include <unistd.h>

#include <daw/daw_benchmark.h>

#include "daw/daw_future_process.h"


struct A {
	double a = 0.0;
	float b = 0.0;
	unsigned long long c = 0;

	void show( ) const {
		std::cout << "a: " << a << " b: " << b << " c: " << c << '\n';
	}
};

int main( ) {
	auto const func = []( int b ) {
		sleep( 5 );
		return b * b;
	};

	auto f1 = daw::process::async( func, 5 );
	auto f2 = daw::process::async( func, 10 );

	std::cout << f1.get( ) + f2.get( ) << '\n';

	std::vector<std::future<int>> futs{};

	for( size_t n = 0; n < 100; ++n ) {
		futs.push_back( daw::process::async( func, n ) );
	}
	auto sums = std::accumulate( futs.begin( ), futs.end( ), 0,
	                             []( int s, auto &f ) { return s + f.get( ); } );

	std::cout << "sums: " << sums << '\n';

	auto const func2 = []( int arg ) {
		A result{};
		result.a = arg * 1.23456;
		result.b = arg * 10.0f * 1.23456f;
		result.c = static_cast<unsigned long long>( arg ) * 12345600ULL;

		return result;
	};

	auto f3 = daw::process::async( func2, 5 );
	auto f4 = daw::process::async( func2, 10 );

	f3.get( ).show( );
	f4.get( ).show( );

	using result_t = std::variant<int, double, A>;

	auto f5 = daw::process::async( []( ) -> result_t { return 5.5; } );

	auto r5 = f5.get( );
	daw::expecting( r5.index( ) == 1 );
	std::cout << std::get<1>( r5 ) << '\n';
}

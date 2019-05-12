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

#include <cstdio>

#include <daw/daw_process.h>
#include <daw/daw_semaphore.h>

extern bool can_run;

bool can_run = true;

int main( ) {
	auto sem_a = daw::process::semaphore( );
	auto sem_b = daw::process::semaphore( );

	auto proc = daw::process::fork_process( [&]( unsigned int t ) {
		while( can_run ) {
			puts( "child: sleeping\n" );
			sleep( t );
			puts( "child: awake\n" );
			sem_b.post( );
			puts( "child: awaiting parent acknowledgement\n" );
			sem_a.wait( );
			puts( "child: got parent's acknowledgement\n" );
		}
	}, 2 );

	while( can_run ) {
		puts( "parent: awaiting child\n" );
		sem_b.wait( );
		puts( "parent: got child's post\n" );
		sem_a.post( );
		puts( "parent: sent child's post\n" );
	}
}

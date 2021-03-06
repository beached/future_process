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

#pragma once

#include <cstddef>
#include <functional>
#include <future>
#include <sys/wait.h>
#include <type_traits>
#include <unistd.h>

#include <daw/daw_exception.h>

#include "daw_channel.h"
#include "daw_process.h"

namespace daw::process {
	template<typename Function, typename... Arguments,
	         typename Ret = std::remove_cv_t<std::remove_reference_t<
	           std::invoke_result_t<Function, Arguments...>>>>
	std::future<Ret> async( Function &&func, Arguments &&... arguments ) {

		return std::async(
		  std::launch::async,
		  []( auto &&f, auto &&... args ) -> Ret {
			  // Child
			  auto mem = daw::process::shared_memory<Ret>( );
			  auto sem = daw::process::semaphore( );
			  auto proc = daw::process::fork_process( [&]( ) {
				  mem.write( std::invoke( *std::forward<Function>( f ),
				                          std::forward<Arguments>( args )... ) );
				  sem.post( );
			  } );

			  // Parent
			  proc.join( );
			  daw::exception::daw_throw_on_false<std::runtime_error>(
			    sem.try_wait( ), "Error running callable" );
			  return mem.read( );
		  },
		  std::forward<Function>( func ), std::forward<Arguments>( arguments )... );
	}
} // namespace daw::process

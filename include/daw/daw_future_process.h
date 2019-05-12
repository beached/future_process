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
#include <unistd.h>

#include <daw/cpp_17.h>
#include <daw/daw_parser_helper_sv.h>
#include <daw/daw_random.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

#include "daw_semaphore.h"
#include "daw_shared_memory.h"

namespace daw::process {
	template<typename Function, typename... Arguments,
	         typename Ret =
	           daw::remove_cvref_t<std::invoke_result_t<Function, Arguments...>>>
	std::future<Ret> async( Function &&func, Arguments &&... arguments ) {

		return std::async(
		  std::launch::async,
		  [func = daw::mutable_capture( std::forward<Function>( func ) )](
		    auto &&... args ) -> Ret {

			  auto shared_result = daw::shared_memory_t<Ret>( );
			  auto sem = daw::semaphore_t( );

			  auto pid = fork( );
			  daw::exception::daw_throw_on_true<std::runtime_error>(
			    pid < 0, "Error forking" );

			  if( pid == 0 ) {
				  // child
				  shared_result.write(
				    daw::invoke( *func, std::forward<decltype( args )>( args )... ) );
				  sem.post( );
				  exit( 0 );
			  } else {
				  // parent
				  int status = 0;
				  waitpid( pid, &status, WUNTRACED );
				  daw::exception::daw_throw_on_false<std::runtime_error>(
				    sem.try_wait( ), "Error running callable" );
				  return shared_result.read( );
			  }
		  },
		  std::forward<Arguments>( arguments )... );
	}
} // namespace daw::process
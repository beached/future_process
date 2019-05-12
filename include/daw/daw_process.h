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
#include <sys/wait.h>
#include <unistd.h>

#include <daw/cpp_17.h>
#include <daw/daw_traits.h>
#include <daw/daw_utility.h>

namespace daw::process {
	template<bool wait_on_pid = true>
	class fork_process {
		pid_t m_pid = -1;

	public:
		constexpr fork_process( ) noexcept = default;

		template<typename Function, typename... Args>
		fork_process( Function &&func, Args &&... args ) noexcept {
			m_pid = fork( );
			daw::exception::daw_throw_on_true<std::runtime_error>( m_pid < 0,
			                                                       "Error forking" );
			if( m_pid == 0 ) {
				try {
					(void)daw::invoke( std::forward<Function>( func ),
					                   std::forward<Args>( args )... );
				} catch( ... ) { exit( 1 ); }
				exit( 0 );
			}
		}

		fork_process( fork_process const & ) = delete;
		fork_process &operator=( fork_process const & ) = delete;

		constexpr fork_process( fork_process &&other ) noexcept
		  : m_pid( std::exchange( other.m_pid, 0 ) ) {}
		constexpr fork_process &operator=( fork_process &&rhs ) noexcept {
			if( this != &rhs ) {
				m_pid = std::exchange( rhs.m_pid, 0 );
			}
			return *this;
		}

		void wait( ) const noexcept {
			int status = 0;
			waitpid( m_pid, &status, WUNTRACED );
		}

		~fork_process( ) noexcept {
			if( auto tmp = std::exchange( m_pid, -1 ) > 0 ) {
				if( wait_on_pid ) {
					wait( );
				}
			}
		}
	};
} // namespace daw::process
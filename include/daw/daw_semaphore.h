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

#include <fcntl.h>
#include <semaphore.h>

#include <daw/daw_random.h>

namespace daw::process {
	class semaphore {
		sem_t *m_sem;
		bool m_is_copy = false;

		static sem_t *get_sem( int init_value ) noexcept {
			auto const name = std::to_string( randint<size_t>( ) );
			auto result =
			  sem_open( name.c_str( ), O_CREAT | O_EXCL, 0600, init_value );
			if( result == SEM_FAILED ) {
				return nullptr;
			}
			return result;
		}

		void cleanup( ) noexcept {
			if( !std::exchange( m_is_copy, true ) ) {
				if( auto tmp_sem = std::exchange( m_sem, nullptr ); tmp_sem ) {
					sem_close( tmp_sem );
				}
			}
		}

	public:
		semaphore( ) noexcept
		  : m_sem( get_sem( 0 ) ) {}

		explicit semaphore( int initial_value ) noexcept
		  : m_sem( get_sem( initial_value ) ) {}

		~semaphore( ) noexcept {
			cleanup( );
		}

		semaphore( semaphore const &other ) noexcept
		  : m_sem( other.m_sem )
		  , m_is_copy( true ) {}

		semaphore &operator=( semaphore const &rhs ) noexcept {
			if( this != &rhs ) {
				cleanup( );
				m_sem = rhs.m_sem;
			}
			return *this;
		}

		semaphore( semaphore &&other ) noexcept
		  : m_sem( std::exchange( other.m_sem, nullptr ) )
		  , m_is_copy( std::exchange( other.m_is_copy, true ) ) {}

		semaphore &operator=( semaphore &&rhs ) noexcept {
			if( this != &rhs ) {
				cleanup( );
				m_sem = std::exchange( rhs.m_sem, nullptr );
				m_is_copy = std::exchange( rhs.m_is_copy, true );
			}
			return *this;
		}

		void wait( ) {
			daw::exception::daw_throw_on_true<std::runtime_error>(
			  sem_wait( m_sem ) == -1, "Error waiting for data" );
		}

		bool try_wait( ) noexcept {
			return sem_trywait( m_sem ) == 0;
		}

		void post( ) noexcept {
			sem_post( m_sem );
		}
	};
} // namespace daw::process

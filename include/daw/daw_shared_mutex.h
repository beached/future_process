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

#include <pthread.h>

#include <daw/daw_exception.h>

#include "daw_shared_memory.h"

namespace daw::process {
	class shared_mutex {
		daw::process::shared_memory<pthread_mutex_t> m_mutex{};
		bool m_is_copy = false;

	public:
		shared_mutex( ) {
			pthread_mutexattr_t attr{};
			auto err = pthread_mutexattr_init( &attr );
			daw::exception::daw_throw_on_true( err );
			err = pthread_mutexattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
			daw::exception::daw_throw_on_true( err );

			err = pthread_mutex_init( m_mutex.data( ), &attr );
			daw::exception::daw_throw_on_true( err );
			err = pthread_mutexattr_destroy( &attr );
			daw::exception::daw_throw_on_true( err );
		}

		~shared_mutex( ) noexcept {
			if( !std::exchange( m_is_copy, true ) ) {
				pthread_mutex_destroy( m_mutex.data( ) );
			}
		}

		shared_mutex( shared_mutex const &other ) noexcept
		  : m_mutex( other.m_mutex )
		  , m_is_copy( true ) {}

		shared_mutex &operator=( shared_mutex const &rhs ) noexcept {
			if( this != &rhs ) {
				m_mutex = rhs.m_mutex;
				m_is_copy = true;
			}
			return *this;
		}

		shared_mutex( shared_mutex &&other ) noexcept
		  : m_mutex( other.m_mutex )
		  , m_is_copy( std::exchange( other.m_is_copy, true ) ) {}

		shared_mutex &operator=( shared_mutex &&rhs ) noexcept {
			if( this != &rhs ) {
				m_mutex = rhs.m_mutex;
				m_is_copy = std::exchange( rhs.m_is_copy, true );
			}
			return *this;
		}

		void lock( ) {
			auto const err = pthread_mutex_lock( m_mutex.data( ) );
			daw::exception::daw_throw_on_true( err );
		}

		void try_lock( ) {
			auto const err = pthread_mutex_trylock( m_mutex.data( ) );
			daw::exception::daw_throw_on_true( err );
		}

		void unlock( ) {
			auto const err = pthread_mutex_unlock( m_mutex.data( ) );
			daw::exception::daw_throw_on_true( err );
		}
	};
} // namespace daw::process

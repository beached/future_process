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

#include <cstring>
#include <sys/mman.h>
#include <type_traits>
#include <utility>

namespace daw::process {
	template<typename T>
	class shared_memory {
		volatile char *m_data;
		bool m_is_copy = false;

		static_assert( std::is_trivially_copyable_v<T> );
		static_assert( std::is_default_constructible_v<T> );

		void cleanup( ) noexcept {
			if( !std::exchange( m_is_copy, true ) ) {
				if( auto tmp_data = std::exchange( m_data, nullptr ); tmp_data ) {
					auto ptr =
					  const_cast<void *>( reinterpret_cast<volatile void *>( m_data ) );
					munmap( ptr, sizeof( T ) );
				}
			}
		}

	public:
		shared_memory( ) noexcept
		  : m_data( static_cast<volatile char *>(
		      mmap( nullptr, sizeof( T ), PROT_READ | PROT_WRITE,
		            MAP_SHARED | MAP_ANONYMOUS, -1, 0 ) ) ) {}

		~shared_memory( ) noexcept {
			cleanup( );
		}

		shared_memory( shared_memory const &other ) noexcept
		  : m_data( other.m_data )
		  , m_is_copy( true ) {}

		shared_memory &operator=( shared_memory const &rhs ) noexcept {
			if( this != &rhs ) {
				cleanup( );
				m_data = rhs.m_data;
			}
			return *this;
		}

		shared_memory( shared_memory &&other ) noexcept
		  : m_data( std::exchange( other.m_data, nullptr ) )
		  , m_is_copy( std::exchange( other.m_is_copy, true ) ) {}

		shared_memory &operator=( shared_memory &&rhs ) noexcept {
			if( this != &rhs ) {
				cleanup( );
				m_data = std::exchange( rhs.m_data, nullptr );
				m_is_copy = std::exchange( rhs.m_is_copy, true );
			}
			return *this;
		}

		T read( ) const noexcept {
			T result;
			auto ptr =
			  const_cast<void *>( reinterpret_cast<volatile void *>( m_data ) );
			memcpy( &result, ptr, sizeof( T ) );
			return result;
		}

		void write( T const &value ) noexcept {
			auto ptr =
			  const_cast<void *>( reinterpret_cast<volatile void *>( m_data ) );
			memcpy( ptr, &value, sizeof( T ) );
		}
	};
} // namespace daw::process

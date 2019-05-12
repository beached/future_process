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

#include <optional>

#include "daw_semaphore.h"
#include "daw_shared_memory.h"

namespace daw::process {
	template<typename T>
	class channel {
		daw::process::semaphore m_can_write{};
		daw::process::semaphore m_can_read{};
		daw::process::shared_memory<T> m_data{};

	public:
		channel( ) noexcept {
			m_can_write.post( );
		}

		void write( T const &value ) noexcept {
			m_can_write.wait( );
			m_data.write( value );
			m_can_read.post( );
		}

		bool try_write( T const &value ) noexcept {
			if( m_can_write.try_wait( ) ) {
				m_data.write( value );
				m_can_read.post( );
				return true;
			}
			return false;
		}

		T read( ) noexcept {
			m_can_read.wait( );
			auto result = m_data.read( );
			m_can_write.post( );
			return result;
		}

		std::optional<T> try_read( ) noexcept {
			if( m_can_read.try_wait( ) ) {
				auto result = m_data.read( );
				m_can_write.post( );
				return result;
			}
			return std::nullopt;
		}
	};
} // namespace daw::process

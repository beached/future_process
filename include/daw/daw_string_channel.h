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

#include <algorithm>
#include <array>
#include <optional>
#include <string>
#include <string_view>

#include "daw_channel.h"

namespace daw::process {
	template<typename CharT = char, size_t buff_size = 20>
	class string_channel {
		using buffer_t = std::array<CharT, buff_size>;
		daw::process::channel<std::optional<buffer_t>> m_channel{};

	public:
		string_channel( ) noexcept = default;

		inline void write( std::basic_string_view<CharT> sv ) noexcept {
			buffer_t buff = { 0 };
			while( !sv.empty( ) ) {
				auto const sz = std::min( buff_size, sv.size( ) );
				std::copy_n( sv.data( ), sz, buff.data( ) );
				if( sz < buff_size ) {
					buff[sz] = 0;
				}
				m_channel.write( buff );
				sv.remove_prefix( sz );
			}
			m_channel.write( std::nullopt );
		}

		inline std::basic_string<CharT> read( ) noexcept {
			std::basic_string<CharT> result{};
			auto msg = m_channel.read( );
			while( msg ) {
				result.reserve( result.size( ) + buff_size );
				auto const &buff = *msg;
				for( auto it = buff.begin( ); it != buff.end( ) and *it != 0;
				     ++it ) {
					result.push_back( *it );
				}
				msg = m_channel.read( );
			}
			return result;
		}
	};
} // namespace daw::process

cmake_minimum_required(VERSION 3.16)

set(SOURCES
        base32_crockford.hpp
        base32_default_crockford.hpp
        base32_default_hex.hpp
        base32_default_rfc4648.hpp
        base32_hex.hpp
        base32_rfc4648.hpp
        base64_default_rfc4648.hpp
        base64_default_url.hpp
        base64_default_url_unpadded.hpp
        base64_rfc4648.hpp
        base64_url.hpp
        base64_url_unpadded.hpp
        hex_default_lower.hpp
        hex_default_upper.hpp
        hex_lower.hpp
        hex_upper.hpp
        parse_error.hpp
        detail/base32.hpp
        detail/base64.hpp
        detail/codec.hpp
        detail/config.hpp
        detail/hex.hpp
        detail/stream_codec.hpp
        data/access.hpp
        data/raw_result_buffer.hpp
        )

set(LIBRARY cppcodec)

add_library(
        ${LIBRARY} STATIC
        ${SOURCES}
)

set_target_properties(${LIBRARY} PROPERTIES LINKER_LANGUAGE CXX)

target_compile_options(${LIBRARY} PUBLIC
        ${SPECIFIC_COMPILE_OPTIONS}
        )

target_link_options(${LIBRARY} PUBLIC
        ${SPECIFIC_LINK_OPTIONS}
        )

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

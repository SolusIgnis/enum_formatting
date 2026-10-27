# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BEMAN_ENUM_FORMATTING_USE_MODULES)
    target_sources(
        beman.enum_formatting
        PUBLIC
            FILE_SET CXX_MODULES FILES enum_formatting.cppm
            FILE_SET HEADERS
                FILES
                    config.hpp
                    enum_formatting.hpp
                    annotations.hpp
                    unexported/to_string.hpp
                    string.hpp
                    utility.hpp
                    format.hpp
                    "${PROJECT_BINARY_DIR}/include/beman/enum_formatting/config_generated.hpp"
    )
else()
    target_sources(
        beman.enum_formatting
        PUBLIC
            FILE_SET HEADERS
                FILES
                    config.hpp
                    enum_formatting.hpp
                    annotations.hpp
                    unexported/to_string.hpp
                    string.hpp
                    utility.hpp
                    format.hpp
                    "${PROJECT_BINARY_DIR}/include/beman/enum_formatting/config_generated.hpp"
    )
endif()

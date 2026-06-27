cmake_minimum_required(VERSION 3.16)

# HapInAVFoundation.framework: the HAP video codec (decode/encode, DXT/YCoCg, squish).
# Native CMake build (ninja + Xcode) of the HapInAVFoundation Xcode target. Linked by
# the Isadora app and the IzzyMoviePlayerMetal helper. (#234)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
if(CMAKE_HOST_APPLE AND NOT CMAKE_OSX_SYSROOT)
    execute_process(COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE ISADORA_MACOS_SDK_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CMAKE_OSX_SYSROOT "${ISADORA_MACOS_SDK_PATH}")
endif()

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "")

project(HapInAVFoundationFramework LANGUAGES C CXX OBJC)

option(ISADORA_USE_CCACHE "Use ccache to speed up rebuilds when available" ON)
if(ISADORA_USE_CCACHE AND NOT CMAKE_GENERATOR STREQUAL "Xcode")
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_OBJC_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    endif()
endif()

set(HERE "${CMAKE_CURRENT_SOURCE_DIR}")

set(HAP_SOURCES
    "${HERE}/external/hap/hap.c"
    "${HERE}/external/snappy/snappy-source/snappy-c.cc"
    "${HERE}/external/snappy/snappy-source/snappy-sinksource.cc"
    "${HERE}/external/snappy/snappy-source/snappy-stubs-internal.cc"
    "${HERE}/external/snappy/snappy-source/snappy.cc"
    "${HERE}/external/squish/squish-source/alpha.cpp"
    "${HERE}/external/squish/squish-source/clusterfit.cpp"
    "${HERE}/external/squish/squish-source/colourblock.cpp"
    "${HERE}/external/squish/squish-source/colourfit.cpp"
    "${HERE}/external/squish/squish-source/colourset.cpp"
    "${HERE}/external/squish/squish-source/maths.cpp"
    "${HERE}/external/squish/squish-source/rangefit.cpp"
    "${HERE}/external/squish/squish-source/singlecolourfit.cpp"
    "${HERE}/external/squish/squish-source/squish.cpp"
    "${HERE}/HapInAVFoundation/AVAssetAdditions.m"
    "${HERE}/HapInAVFoundation/AVAssetReaderHapTrackOutput.m"
    "${HERE}/HapInAVFoundation/AVAssetWriterHapInput.m"
    "${HERE}/HapInAVFoundation/AVPlayerItemAdditions.m"
    "${HERE}/HapInAVFoundation/AVPlayerItemHapDXTOutput.m"
    "${HERE}/HapInAVFoundation/CMBlockBufferPool.m"
    "${HERE}/HapInAVFoundation/HapDecoderFrame.m"
    "${HERE}/HapInAVFoundation/HapEncoderFrame.m"
    "${HERE}/source/DXT/ATEBC7Encoder.c"
    "${HERE}/source/DXT/DXTBlocks.c"
    "${HERE}/source/DXT/DXTBlocksSSSE3.c"
    "${HERE}/source/DXT/HapMetalDXTDecoder.m"
    "${HERE}/source/DXT/squish-c.cpp"
    "${HERE}/source/DXT/SquishEncoder.c"
    "${HERE}/source/DXT/YCoCgDXT.cpp"
    "${HERE}/source/DXT/YCoCgDXTEncoder.c"
    "${HERE}/source/PixelFormats.c"
    "${HERE}/source/SquishRGTC1Decoder.c"
    "${HERE}/source/Utility.c"
    "${HERE}/source/Utility.m"
    "${HERE}/source/YCoCg/ImageMath.c"
    "${HERE}/source/YCoCg/YCoCg.c"
)

add_library(HapInAVFoundation SHARED ${HAP_SOURCES})

# Sources use framework-style includes ("HapInAVFoundation/hap.h") that Xcode
# resolves via a header map, even for headers physically in source/ or external/.
# Stage every framework header under a single HapInAVFoundation/ dir of symlinks so
# those includes resolve under CMake too.
set(HAP_FWH "${CMAKE_CURRENT_BINARY_DIR}/fwheaders")
file(MAKE_DIRECTORY "${HAP_FWH}/HapInAVFoundation")
file(GLOB_RECURSE _hap_hdrs
    "${HERE}/HapInAVFoundation/*.h" "${HERE}/source/*.h" "${HERE}/external/hap/*.h")
foreach(_h ${_hap_hdrs})
    get_filename_component(_hn "${_h}" NAME)
    if(NOT EXISTS "${HAP_FWH}/HapInAVFoundation/${_hn}")
        file(CREATE_LINK "${_h}" "${HAP_FWH}/HapInAVFoundation/${_hn}" SYMBOLIC)
    endif()
endforeach()

# Xcode resolves these via header maps; CMake needs them explicit.
target_include_directories(HapInAVFoundation PRIVATE
    "${HAP_FWH}"                                  # "HapInAVFoundation/<hdr>.h"
    "${HERE}/HapInAVFoundation"
    "${HERE}/source"
    "${HERE}/source/DXT"
    "${HERE}/source/YCoCg"
    "${HERE}/external/hap"
    "${HERE}/external/snappy/snappy-source"
    "${HERE}/external/squish/squish-source"
)

# Consumers (#import <HapInAVFoundation/...>) get the staged framework headers.
target_include_directories(HapInAVFoundation INTERFACE "${HAP_FWH}")

target_compile_definitions(HapInAVFoundation PRIVATE
    OBJC_OLD_DISPATCH_PROTOTYPES=0
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG=1>
)

# ARC: the Xcode target sets CLANG_ENABLE_OBJC_ARC=YES; the ObjC sources rely on it.
# Without ARC the decoder's frame arrays are never retained, freed early, and their
# slots get reused -> "unrecognized selector" crash decoding HAP frames (#234).
target_compile_options(HapInAVFoundation PRIVATE
    $<$<COMPILE_LANGUAGE:OBJC,OBJCXX>:-fobjc-arc>)

target_link_libraries(HapInAVFoundation PRIVATE
    "-framework Foundation" "-framework AVFoundation" "-framework CoreMedia"
    "-framework CoreVideo" "-framework CoreFoundation" "-framework Metal"
    "-framework IOSurface" "-framework QuartzCore" "-framework Accelerate")

set(HAP_PUBLIC_HEADERS
    "${HERE}/HapInAVFoundation/HapInAVFoundation.h"
    "${HERE}/HapInAVFoundation/AVAssetAdditions.h"
    "${HERE}/HapInAVFoundation/AVAssetReaderHapTrackOutput.h"
    "${HERE}/HapInAVFoundation/AVAssetWriterHapInput.h"
    "${HERE}/HapInAVFoundation/AVPlayerItemAdditions.h"
    "${HERE}/HapInAVFoundation/AVPlayerItemHapDXTOutput.h"
    "${HERE}/HapInAVFoundation/CMBlockBufferPool.h"
    "${HERE}/HapInAVFoundation/HapDecoderFrame.h"
    "${HERE}/HapInAVFoundation/HapEncoderFrame.h"
    "${HERE}/HapInAVFoundation/HapMacros.h"
)

set_target_properties(HapInAVFoundation PROPERTIES
    # C_STANDARD 11: the app sets CMAKE_C_STANDARD 90 (gnu89) at its scope, inherited
    # by subdirs; hap.c uses C99 for-loop scoping, so pin a newer C standard (#234).
    C_STANDARD 11 C_EXTENSIONS ON
    CXX_STANDARD 14 CXX_EXTENSIONS ON
    FRAMEWORK TRUE
    FRAMEWORK_VERSION A
    MACOSX_FRAMEWORK_IDENTIFIER "com.Vidvox.HapInAVFoundation"
    MACOSX_FRAMEWORK_BUNDLE_VERSION "1"
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING "1.0"
    OUTPUT_NAME "HapInAVFoundation"
    PUBLIC_HEADER "${HAP_PUBLIC_HEADERS}"
)

# Prefix header injected portably (Xcode-attribute-only would be ignored by ninja).
target_compile_options(HapInAVFoundation PRIVATE
    -include "${HERE}/source/HapInAVFoundation_Prefix.pch")

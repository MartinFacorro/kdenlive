# SPDX-FileCopyrightText: 2012 Simon A. Eugster (Granjow) <simon.eu@gmail.com>
# SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

# Create a static library from kiss_fft
find_package(Qt${QT_MAJOR_VERSION}Core REQUIRED)
include_directories(${CMAKE_BINARY_DIR})
add_library(kiss_fft STATIC _kiss_fft_guts.h kiss_fft.c kiss_fftr.c)
set_property(TARGET kiss_fft PROPERTY C_STANDARD 99)
target_link_libraries(kiss_fft Qt${QT_MAJOR_VERSION}::Core)

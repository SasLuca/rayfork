option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" OFF)

# select lib type
set(RAYFORK_LIB_TYPE "STATIC" CACHE STRING "Set the library type to be STATIC (.lib/.a) or SHARED(.dll/.so)")
set_property(CACHE RAYFORK_LIB_TYPE PROPERTY STRINGS "STATIC" "SHARED")

if (NOT ${RAYFORK_LIB_TYPE} MATCHES "STATIC" AND
    NOT ${RAYFORK_LIB_TYPE} MATCHES "SHARED")
    message(FATAL_ERROR "RAYFORK_LIB_TYPE must be one of [ \"STATIC\", \"SHARED\" ] but was set to \"${RAYFORK_LIB_TYPE}\"")
endif()

# select gfx backend
set(RAYFORK_GFX_BACKEND "rayfork_graphics_backend_gl33" CACHE STRING "Set the gfx backend for rayfork")
set_property(CACHE RAYFORK_GFX_BACKEND PROPERTY STRINGS
    "rayfork_graphics_backend_gl33"
    "rayfork_graphics_backend_gl_es3"
    "rayfork_graphics_backend_metal"
    "rayfork_graphics_backend_directx")

if (NOT ${RAYFORK_GFX_BACKEND} MATCHES "rayfork_graphics_backend_gl33"    AND
    NOT ${RAYFORK_GFX_BACKEND} MATCHES "rayfork_graphics_backend_gl_es3"  AND
    NOT ${RAYFORK_GFX_BACKEND} MATCHES "rayfork_graphics_backend_metal"   AND
    NOT ${RAYFORK_GFX_BACKEND} MATCHES "rayfork_graphics_backend_directx")
    message(FATAL_ERROR "RAYFORK_GFX_BACKEND must be one of [ \"rayfork_graphics_backend_gl33\", \"rayfork_graphics_backend_gl_es3\", \"rayfork_graphics_backend_metal\", \"rayfork_graphics_backend_directx\" ] but was set to \"${RAYFORK_GFX_BACKEND}\"")
endif()
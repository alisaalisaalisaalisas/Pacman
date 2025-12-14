#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "unofficial::brotli::brotlienc" for configuration "Debug"
set_property(TARGET unofficial::brotli::brotlienc APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::brotli::brotlienc PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/libbrotlienc.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/libbrotlienc.dll"
  )

list(APPEND _cmake_import_check_targets unofficial::brotli::brotlienc )
list(APPEND _cmake_import_check_files_for_unofficial::brotli::brotlienc "${_IMPORT_PREFIX}/debug/lib/libbrotlienc.dll.a" "${_IMPORT_PREFIX}/debug/bin/libbrotlienc.dll" )

# Import target "unofficial::brotli::brotlidec" for configuration "Debug"
set_property(TARGET unofficial::brotli::brotlidec APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::brotli::brotlidec PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/libbrotlidec.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/libbrotlidec.dll"
  )

list(APPEND _cmake_import_check_targets unofficial::brotli::brotlidec )
list(APPEND _cmake_import_check_files_for_unofficial::brotli::brotlidec "${_IMPORT_PREFIX}/debug/lib/libbrotlidec.dll.a" "${_IMPORT_PREFIX}/debug/bin/libbrotlidec.dll" )

# Import target "unofficial::brotli::brotlicommon" for configuration "Debug"
set_property(TARGET unofficial::brotli::brotlicommon APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::brotli::brotlicommon PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/libbrotlicommon.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/libbrotlicommon.dll"
  )

list(APPEND _cmake_import_check_targets unofficial::brotli::brotlicommon )
list(APPEND _cmake_import_check_files_for_unofficial::brotli::brotlicommon "${_IMPORT_PREFIX}/debug/lib/libbrotlicommon.dll.a" "${_IMPORT_PREFIX}/debug/bin/libbrotlicommon.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

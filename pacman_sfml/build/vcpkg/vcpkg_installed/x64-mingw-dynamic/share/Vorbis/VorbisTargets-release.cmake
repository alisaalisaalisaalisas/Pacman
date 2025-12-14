#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Vorbis::vorbis" for configuration "Release"
set_property(TARGET Vorbis::vorbis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Vorbis::vorbis PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libvorbis.dll.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libvorbis.dll"
  )

list(APPEND _cmake_import_check_targets Vorbis::vorbis )
list(APPEND _cmake_import_check_files_for_Vorbis::vorbis "${_IMPORT_PREFIX}/lib/libvorbis.dll.a" "${_IMPORT_PREFIX}/bin/libvorbis.dll" )

# Import target "Vorbis::vorbisenc" for configuration "Release"
set_property(TARGET Vorbis::vorbisenc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Vorbis::vorbisenc PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libvorbisenc.dll.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libvorbisenc.dll"
  )

list(APPEND _cmake_import_check_targets Vorbis::vorbisenc )
list(APPEND _cmake_import_check_files_for_Vorbis::vorbisenc "${_IMPORT_PREFIX}/lib/libvorbisenc.dll.a" "${_IMPORT_PREFIX}/bin/libvorbisenc.dll" )

# Import target "Vorbis::vorbisfile" for configuration "Release"
set_property(TARGET Vorbis::vorbisfile APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Vorbis::vorbisfile PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libvorbisfile.dll.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libvorbisfile.dll"
  )

list(APPEND _cmake_import_check_targets Vorbis::vorbisfile )
list(APPEND _cmake_import_check_files_for_Vorbis::vorbisfile "${_IMPORT_PREFIX}/lib/libvorbisfile.dll.a" "${_IMPORT_PREFIX}/bin/libvorbisfile.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

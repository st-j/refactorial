#
# Function to find library LIBNAME and the corresponding
# include file INCLUDENAME.
# 
# CACHE entries set:
#
# ${PREFIX}_LIBRARIES     full path of library found
# ${PREFIX}_INCLUDE_DIR   path to include file
#
# Variables set:
#
# ${PREFIX}_FOUND   (PARENT_SCOPE) set to 1 if both the library and the
#                   include file were found
#
function(FindSimplePackage PREFIX LIBNAME INCLUDENAME FINDHINTS)
  # Try to find the library
  find_library(${PREFIX}_LIBRARIES NAMES ${LIBNAME} HINTS ${FINDHINTS}/lib)
  message(STATUS "Libs found:" ${${PREFIX}_LIBRARIES})
  # Try to find the includes
  find_path(${PREFIX}_INCLUDE_DIR ${INCLUDENAME} HINTS ${FINDHINTS}/include)
  message(STATUS "Include found:" ${${PREFIX}_INCLUDE_DIR})

  if(${PREFIX}_LIBRARIES AND ${PREFIX}_INCLUDE_DIR)
	set(${PREFIX}_FOUND 1 PARENT_SCOPE)
  endif(${PREFIX}_LIBRARIES AND ${PREFIX}_INCLUDE_DIR)

endfunction(FindSimplePackage PREFIX LIBNAME INCLUDENAME)

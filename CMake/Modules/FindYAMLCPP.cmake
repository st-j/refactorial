# 
# Find yaml-cpp libraries
#
# Sets the following variables in case YAMLCPP is found:
#
# YAMLCPP_FOUND
# YAMLCPP_LIBRARIES
# YAMLCPP_INCLUDE_DIR

include(FindSimplePackage)
FindSimplePackage(YAMLCPP yaml-cpp yaml-cpp/yaml.h "")

# handle the QUIETLY and REQUIRED arguments and set YAMLCPP_FOUND to TRUE if 
# all listed variables are TRUE
set(YAML_NOT_FOUND_MSG
  "The yaml-cpp library could not be found, but is required for PEST.
If you have already installed yaml-cpp, you might need to point cmake to the install location (TODO: how should the user do this?).
If you have not installed yaml-cpp yet, you can download the source code from http://code.google.com/p/yaml-cpp/
")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(YAMLCPP ${YAML_NOT_FOUND_MSG} YAMLCPP_LIBRARIES YAMLCPP_INCLUDE_DIR)



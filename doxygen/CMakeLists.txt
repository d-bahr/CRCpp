find_package(Doxygen COMPONENTS doxygen REQUIRED)

set(DOXYGEN_PROJECT_NAME "CRC++")
set(DOXYGEN_PROJECT_BRIEF "Easy to use and fast C++ CRC library.")
set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}")

set(DOXYGEN_BUILTIN_STL_SUPPORT "YES")
set(DOXYGEN_EXCLUDE_PATTERNS "${CMAKE_SOURCE_DIR}/test/*")
set(DOXYGEN_EXTRACT_ALL "YES")
set(DOXYGEN_HAVE_DOT "OFF") # Currently no relevant graphs
set(DOXYGEN_PREDEFINED "CRCPP_USE_CPP11" "CRCPP_INCLUDE_ESOTERIC_CRC_DEFINITIONS")
set(DOXYGEN_STRIP_FROM_PATH "${CMAKE_SOURCE_DIR}")
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
set(DOXYGEN_WARN_LOGFILE "${CMAKE_CURRENT_BINARY_DIR}/DoxygenWarnings.txt")

doxygen_add_docs(doxygen-doc
    "${CMAKE_SOURCE_DIR}"
)

# Prevent installation failure if the directory does not exist because Doxygen has not been executed
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html/")

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html/"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}/html"
        COMPONENT doc)

if(NOT NCURSESW_FOUND)
  if(NCURSESW_ROOT_DIR)
    find_path(NCURSESW_INCLUDE_DIR
      NAMES ncursesw/ncurses.h
      PATHS ${NCURSESW_ROOT_DIR}/include
      NO_DEFAULT_PATH
      )

    set(NCURSESW_INCLUDE_DIRS ${NCURSESW_INCLUDE_DIR})

    find_library(NCURSESW_LIBRARIES
      NAMES ncursesw ncurses
      PATHS ${NCURSESW_ROOT_DIR}/lib
      NO_DEFAULT_PATH
      )
  else()
    find_path(NCURSESW_INCLUDE_DIR
      NAMES ncursesw/ncurses.h
      PATH_SUFFIXES ncurses ncursesw
      )
    find_library(NCURSESW_LIBRARIES NAMES ncurses ncursesw)

    set(NCURSESW_INCLUDE_DIRS ${NCURSESW_INCLUDE_DIR})
  endif()

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Ncursesw  DEFAULT_MSG
    NCURSESW_LIBRARIES NCURSESW_INCLUDE_DIR)

  mark_as_advanced(
    NCURSESW_INCLUDE_DIRS
    NCURSESW_LIBRARIES
    )
endif()

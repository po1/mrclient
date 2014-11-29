if(NOT NCURSES_FOUND)
  if(NCURSES_ROOT_DIR)
    find_path(NCURSES_INCLUDE_DIR
      NAMES ncurses.h curses.h
      PATHS ${NCURSES_ROOT_DIR}/include
      NO_DEFAULT_PATH
      )

    set(NCURSES_INCLUDE_DIRS ${NCURSES_INCLUDE_DIR})

    find_library(NCURSES_LIBRARIES
      NAMES ncursesw ncurses
      PATHS ${NCURSES_ROOT_DIR}/lib
      NO_DEFAULT_PATH
      )
  else()
    find_path(NCURSES_INCLUDE_DIR
      NAMES ncurses.h curses.h
      )
    find_library(NCURSES_LIBRARIES NAMES ncurses ncursesw)

    set(NCURSES_INCLUDE_DIRS ${NCURSES_INCLUDE_DIR})
  endif()

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Ncurses  DEFAULT_MSG
    NCURSES_LIBRARIES NCURSES_INCLUDE_DIR)

  mark_as_advanced(
    NCURSES_INCLUDE_DIRS
    NCURSES_LIBRARIES
    )
endif()

include(FetchContent)

# --------------------------------------------------------------------

# Offline mode
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)
# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

message(STATUS "FetchContent: whirl-frontend")

FetchContent_Declare(
        whirl-frontend
        GIT_REPOSITORY https://github.com/artem/whirl-frontend.git
        GIT_TAG 3e016222ea499fa99b1541153cab3056e15294dc
)
FetchContent_MakeAvailable(whirl-frontend)

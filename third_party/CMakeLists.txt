include(FetchContent)

# --------------------------------------------------------------------

# Offline mode
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)
# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

message(STATUS "FetchContent: whirl-frontend")

FetchContent_Declare(
        whirl-frontend
        GIT_REPOSITORY https://gitlab.com/whirl-framework/whirl-frontend.git
        GIT_TAG 6fb468ecba2707808167d5d6215a2a787f723424
)
FetchContent_MakeAvailable(whirl-frontend)

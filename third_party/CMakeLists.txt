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
        GIT_TAG 56923174f23dd6c7937ece3a5d10b2f2b06ec7ed
)
FetchContent_MakeAvailable(whirl-frontend)

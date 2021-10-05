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
        GIT_TAG 7ca8a39c6c8c0158054449d67faab7b7fbaaedff
)
FetchContent_MakeAvailable(whirl-frontend)

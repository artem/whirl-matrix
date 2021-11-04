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
        GIT_TAG e9955258b613cc7db2850f497d1c4762c2a252cf
)
FetchContent_MakeAvailable(whirl-frontend)

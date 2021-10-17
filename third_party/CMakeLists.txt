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
        GIT_TAG ee48e193f9625c4aba08b967a5cb6ee054804b99
)
FetchContent_MakeAvailable(whirl-frontend)

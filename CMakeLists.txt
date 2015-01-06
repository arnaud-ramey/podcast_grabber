cmake_minimum_required (VERSION 2.6)
project (PodcastGrabber)

# activate all warnings
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

add_executable( PodcastGrabber.exe PodcastGrabber.cpp test.cpp)
target_link_libraries ( PodcastGrabber.exe pthread)


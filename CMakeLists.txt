project(kmagnet)

find_package(KDE4 REQUIRED)
find_package(LibKDEGames REQUIRED)
include (KDE4Defaults)

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES})

add_subdirectory( themes )
add_subdirectory( doc )
add_subdirectory( src )
add_subdirectory( icons )
add_subdirectory( data )
add_subdirectory( po )
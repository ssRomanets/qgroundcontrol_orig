
target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        Providers/TerrainTileCopernicus.cc
        Providers/TerrainTileCopernicus.h
        TerrainTile.cc
        TerrainTile.h
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Providers
)

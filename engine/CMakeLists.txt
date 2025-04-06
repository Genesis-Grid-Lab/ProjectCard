project(CardEngine)

file(GLOB_RECURSE source ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)


add_library(${PROJECT_NAME} SHARED ${source} ${headers} ${CMAKE_SOURCE_DIR}/vendor/stb_image/stb_image.cpp )

# target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/tbpch.h)

# includes directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include  
    ${CMAKE_SOURCE_DIR}/vendor/entt/src/entt
    ${CMAKE_SOURCE_DIR}/vendor/glad/include    
    ${CMAKE_SOURCE_DIR}/vendor/glm
    ${CMAKE_SOURCE_DIR}/vendor/stb
    ${CMAKE_SOURCE_DIR}/vendor/spdlog/include 
    ${CMAKE_SOURCE_DIR}/vendor/imgui
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
)


# link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC     
    glad    
    glm
    spdlog
    imgui
    EnTT     
)

set(CMAKE_C_STANDARD 99)

add_executable(comparer src/comparer.c src/needs_converter.c)
add_executable(converter src/converter.c src/needs_converter.c src/negative.c)

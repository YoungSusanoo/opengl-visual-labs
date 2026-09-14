add_rules("mode.debug", "mode.release")
set_languages("c++20")

-- Link directly against the system-installed dev packages via pkg-config,
-- bypassing xmake's own package repository (not reachable in this environment).
add_requires("pkgconfig::glfw3", "pkgconfig::glew")

target("task1")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("pkgconfig::glfw3", "pkgconfig::glew")
    add_includedirs("/usr/include") -- glm is header-only, no .pc file shipped
    add_syslinks("GL")
    set_rundir(os.projectdir()) -- so the relative "shaders" path resolves under `xmake run`

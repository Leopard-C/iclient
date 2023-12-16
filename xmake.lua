
add_rules("mode.debug", "mode.release")
set_policy("build.warning", true)
set_languages("c99", "cxx11")
add_cxflags("-Wreturn-type", "-Wsign-compare", "-Wunused", "-Wswitch")
add_cxflags("-Wno-deprecated-declarations")

add_includedirs("include")

target("iclient")
    set_kind("static")
    add_files("src/*.cpp")
    add_links("curl", "pthread", "dl")
    set_targetdir("lib/linux/$(arch)")

target("example.out")
    set_kind("binary")
    add_files("example/*.cpp")
    add_deps("iclient")
    set_targetdir("bin")

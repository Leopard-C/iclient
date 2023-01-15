
add_rules("mode.debug", "mode.release")
set_languages("c99", "cxx11")
add_cxflags("-Wreturn-type", "-Wsign-compare", "-Wunused", "-Wswitch")
add_cxflags("-Wno-deprecated-declarations")

target("iclient")
    set_kind("static")
    add_includedirs("include")
    add_files("src/*.cpp")
    set_targetdir("lib/linux")

target("example.out")
    set_kind("binary")
    add_includedirs("include")
    add_files("example/*.cpp")
    add_linkdirs("lib/linux")
    add_links("curl", "pthread", "dl", "iclient")
    set_targetdir("bin")


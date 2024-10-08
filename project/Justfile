[private]
@default:
    just --list --unsorted --no-aliases

[private]
qmake *args:
    qmake {{ args }}

# build the project
build: qmake
    make

# run the project
run *args: build
    ./out/main {{ args }}

# run and debug the project
debug: build
    gdb ./out/main

# run and profile the project
profile: build
    mkdir -p out/traces; cd out/traces; valgrind --tool=callgrind ../main

# force rebuild the project
force-build: clean
    bear -- make

# force rebuild and run the project
force-run *args: force-build
    ./out/main {{ args }}

# remove compilation artifacts
clean: qmake
    rm -rf out/traces
    make clean

# pruge all intermediate files
purge: qmake
    make distclean

alias b := build
alias r := run

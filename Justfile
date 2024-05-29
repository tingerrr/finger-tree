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
run: build
    ./out/main

# force rebuild the project
force-build: clean
    bear -- make

# force rebuild and run the project
force-run: force-build
    ./out/main

# remove compilation artifacts
clean: qmake
    make clean

# pruge all intermediate files
purge: qmake
    make distclean

alias b := build
alias r := run

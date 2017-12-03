#!/usr/bin/env bats

setup() {
    if [ ! -d ./expanded ];then mkdir expanded ;fi
}

@test "All paths are valid in astar" {
    if [ ! -f astar ];then
        bash ./run -full map-for-test/ca_cave.map astar > astar
    fi
    cat astar | grep subopt | awk '{if($14 == "invalid") exit 1;}'
    [ "$?" -eq 0 ]
}

@test "All paths are optimal in astar" {
    if [ ! -f astar ];then
        bash ./run -full map-for-test/ca_cave.map astar > astar
    fi
    cat astar | grep subopt | awk '{if($11 < 0.99999) exit 1;}'
    [ "$?" -eq 0 ]
}

@test "All paths are valid in jps" {
    if [ ! -f jps ];then
        bash ./run -full map-for-test/ca_cave.map jps > jps
    fi
    cat jps | grep subopt | awk '{if($14 == "invalid") exit 1;}'
    [ "$?" -eq 0 ]
}

@test "All paths are optimal in jps" {
    if [ ! -f jps ];then
        bash ./run -full map-for-test/ca_cave.map jps > jps
    fi
    cat jps | grep subopt | awk '{if($11 < 0.99999) exit 1;}'
    [ "$?" -eq 0 ]
}
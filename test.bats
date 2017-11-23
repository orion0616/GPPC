#!/usr/bin/env bats

setup() {
    if [ ! -d ./expanded ];then mkdir expanded ;fi
    bash ./run -full map-for-test/ca_cave.map > logfile
}

@test "All paths are valid" {
    cat logfile | grep subopt | awk '{if($14 == "invalid") exit 1;}'
    [ "$?" -eq 0 ]
}

@test "All paths are optimal" {
    cat logfile | grep subopt | awk '{if($11 < 0.99999) exit 1;}'
    [ "$?" -eq 0 ]
}
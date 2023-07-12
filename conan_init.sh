#!/bin/bash

build_type=Release # Build release by default

function print_usage() {
    echo "Usage: $(basename $0) [-b Release|Debug] [-h]"
}

while getopts b:h option
do
    case "${option}" in
        b) build_type=${OPTARG}
	   ;;
        ?|h) print_usage
	     exit
	     ;;
    esac
done

build_dir="build_$build_type"

rm -rf $build_dir
conan install . --output-folder=$build_dir --build=missing --settings=build_type=$build_type

if [ $? -eq 0 ]
then
   echo "Conan init complete"
   echo "Build command:"
   echo -e "\t cd ./$build_dir"
   echo -e "\t source conanbuild.sh"
   echo -e "\t cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$build_type"
   echo -e "\t cmake --build ."
else
   echo "conan init failure: $?"
fi

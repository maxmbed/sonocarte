# Sonocarte

## Prerequisite

Project uses Conan package. Ensure Conan is installed on host machine.

```shell
sudo apt install conan
```

## Build instruction

Run the init script. Conan automatically downloads and builds the necessary libraries.

```shell
./conan_init.sh
```

To build project

```shell
cd ./build_Release
source conanbuild.sh
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

For a Debug build, pass the `Debug` argument in the init script.

## Trouble shooting

### libalsa cannot be joined to Conan

Due to this known issue: https://github.com/conan-io/conan-center-index/issues/2474. libalsa cannot be joined to Conan. libalsa will therefor be relied to host machine package instead of Canon Center.

To install libalsa:

```shell
sudo apt install libasound2 libasound2-dev
```

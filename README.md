# Sonocarte

## Prerequisite

This repo uses Conan package to build target and depedencies.
Conan can be installed using python package installer in virtual environment

```shell
python -m venv conan-venv
source conan-venv/bin/activate
pip install conan
```

## Build target

Run the init script to automatically download and build necessary library depedencies on the go.

```shell
source ./conan-venv/bin/activate # if not done already
./conan_init.sh
```

Then build target

```shell
cd ./build_Release
source conanbuild.sh
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

To get a debug build, add the argument `-b Debug` to the init script.

## Trouble shooting

### libalsa cannot be joined with Conan

Related to this issue: https://github.com/conan-io/conan-center-index/issues/2474.
libalsa must be relied on host machine instead of Conan Center package.

#### Debian base

```shell
sudo apt install libasound2 libasound2-dev
```
#### Arch

```shell
sudo pacman -S alsa-lib
```


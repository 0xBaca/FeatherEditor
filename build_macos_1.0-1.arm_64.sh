#/bin/sh

build_name="feather_macos_arm64.zip"

rm -f ./builds/$build_name
cd feather-1.0 && make clean && make && zip $build_name feather && mv $build_name ../builds

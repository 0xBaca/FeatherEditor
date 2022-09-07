#/bin/bash

cd feather-1.0 && make clean && cd ..
cp feather-1.0/feather.1 /home/madi/Documents/Feather/sf_frame/debian/feather-1.0-1/debian/manpage.1.ex
cd feather-1.0 && tar czvf ../debian/feather-1.0-1.arm_64.tar.gz . && cd ..
tar -xf ./debian/feather-1.0-1.arm_64.tar.gz -C ./debian/feather-1.0-1
cp ./debian/control/control-arm64 ./debian/feather-1.0-1/debian/control
cp ./debian/rules/rules ./debian/feather-1.0-1/debian/rules
 
cd ./debian/feather-1.0-1/ && dpkg-buildpackage -b -rfakeroot -uc -us && cd ../..
rm -f builds/feather_deb_arm64_deb.zip
zip builds/feather_deb_arm64.zip debian/feather_1-1_arm64.deb
rm -f builds/feather_1-1_arm64.deb

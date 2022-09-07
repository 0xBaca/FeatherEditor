#/bin/sh

cd feather-1.0 && make clean && cd ..
rm -f rpmbuild/SOURCES/feather-1.0-1.arm_64.tar.gz 
tar czvf rpmbuild/SOURCES/feather-1.0-1.arm_64.tar.gz feather-1.0
rpmbuild --define "_topdir `pwd`/rpmbuild/" -ba ./rpmbuild/SPECS/arm.spec
rm -f builds/feather_rpm_arm64.zip
zip builds/feather_rpm_arm64.zip rpmbuild/RPMS/aarch64/feather-1.0-1.aarch64.rpm

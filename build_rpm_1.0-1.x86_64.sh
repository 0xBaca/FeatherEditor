#/bin/sh

cd feather-1.0 && make clean && cd ..
tar czvf rpmbuild/SOURCES/feather-1.0-1.x86_64.tar.gz feather-1.0
rpmbuild --define "_topdir `pwd`/rpmbuild/" -ba ./rpmbuild/SPECS/x86.spec
rm -f builds/feather_rpm_amd64.zip
zip builds/feather_rpm_amd64.zip rpmbuild/RPMS/x86_64/feather-1.0-1.x86_64.rpm

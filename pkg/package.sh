#!/bin/bash

version="1.0.0"

if [ "$#" -eq 1 ]; then
    if [ ! -d "usr" ]; then
        ./build.sh
    fi
else
    echo -e "Expected 1 argument, received $#."
    exit 1
fi

if [[ $1 == "debian" ]]; then

    build_path="git-loc-history_${version}_x86_64"
    mkdir -p $build_path/DEBIAN

    cp -a usr $build_path/usr
    cp debian/control $build_path/DEBIAN
    sed -i -e "s/VERSION/$version/g" $build_path/DEBIAN/control
    sed -i -e "s/INSTALLED_SIZE/$(du -s $build_path/usr | awk '{print $1}')/g" \
        $build_path/DEBIAN/control

    dpkg -b $build_path
    rm -rf $build_path

elif [[ $1 == "fedora" ]]; then

    build_path="rpmbuild"
    mkdir -p $build_path/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

    source_dir=git-loc-history-$version
    mkdir $build_path/SOURCES/$source_dir
    cp -a usr $build_path/SOURCES/$source_dir/usr
    cd $build_path/SOURCES/
    tar -czf $source_dir.tar.gz $source_dir
    cd ../../

    cp fedora/git-loc-history.spec $build_path/SPECS/git-loc-history.spec
    sed -i -e "s/VERSION/$version/g" $build_path/SPECS/git-loc-history.spec

    if [ -d "~/rpmbuild" ]; then
        mv ~/rpmbuild/ ~/rpmbuild-backup/
    fi

    mv $build_path ~/rpmbuild/
    cd ~/rpmbuild/
    rpmbuild -bb SPECS/git-loc-history.spec
    cd -
    mv ~/rpmbuild/ $build_path
    mv $build_path/RPMS/x86_64/git-loc-history-*.rpm ./git-loc-history_${version}_x86_64.rpm
    rm -rf $build_path

else
    echo -e "Unknown argument, must be \"debian\" or \"fedora\"."
    exit 1
fi

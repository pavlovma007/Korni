#!/bin/bash -x

#fuse: failed to exec fusermount3: No such file or directory
which fusermount3
if [[ $? == 1 ]] ; then 
	sudo apt-get install fuse3
fi 


set +e
APP=Korni
patharch=x86_64
#package="brave-browser_1.33.100_amd64.deb"
#URL="https://github.com/brave/brave-browser/releases/"
#if  [ ! -f $package ]; then
    #wget -c $URL$package
#else
    #echo "$package file exists"
#fi

mkdir -p ./$APP.AppDir
rm -rf ./$APP.AppDir/*
cd $APP.AppDir/
#find ../ -name *.deb -exec dpkg -x \{\} . \;
cp /usr/local/bin/korni3 . 
cp ../icon.jpg .
ldd `which korni3 `| awk '{print $3}' | xargs -I {} sh -c 'cp {} ./'


#cp opt/brave.com/brave/product_logo_48.png ./Brave.png

cat > $APP.desktop <<EOF
[Desktop Entry]
Type=Application
Name=korni3
Exec=korni3
Icon=icon.png
StartupNotify=false
Categories=Network;Database;Office;
Comment='платформа личного цифрового суверенитета'
EOF
#MimeType=text/html;text/xml;application/xhtml_xml;image/webp;x-scheme-handler/http;x-scheme-handler/https;x-scheme-handler/ftp;

#mkdir -p usr/bin/
#mv ./opt/brave.com/brave/* usr/bin/ ; rm -rf ./opt

AppRun="AppRun"
if  [ ! -f ../$AppRun ]; then
    wget -c https://github.com/AppImage/AppImageKit/releases/download/continuous/AppRun-${patharch} -O AppRun
else 
	cp ../AppRun . 
fi
chmod a+x AppRun


# generate_type2_appimage
cd ..
pwd 
VERSION=""
#'appimagetool-x86_64.AppImage' --appimage-extract
#VERSION=$VERSION './appimagetool-x86_64.AppImage' -v -n ./$APP.AppDir/
#ARCH='x86_64' VERSION=$VERSION './appimagetool-x86_64.AppImage'  ./$APP.AppDir/
mksquashfs Korni.AppDir/ Korni3.squashfs -root-owned -noappend
cat Korni.AppDir/korni3  > korni3.AppImage
cat Korni3.squashfs >> korni3.AppImage
chmod a+x korni3.AppImage
rm Korni3.squashfs

#if [ -d "./$APP.AppDir/" ] ; then
#  rm -rf ./$APP.AppDir
#fi

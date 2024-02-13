#!/bin/bash -x

#fuse: failed to exec fusermount3: No such file or directory
which fusermount3
if [[ $? == 1 ]] ; then 
	sudo apt-get install fuse3
fi 


set +e
APP=Korni
patharch=x86_64

mkdir -p ./$APP.AppDir
rm -rf ./$APP.AppDir/*
cd $APP.AppDir/
cp /usr/local/bin/korni3 . 
cp ../icon.png .
ldd `which korni3 `| awk '{print $3}' | xargs -I {} sh -c 'cp {} ./'

sed -i -e 's#/usr#././#g' korni3
sed -i -e 's#/usr#././#g' libc.so.6
sed -i -e 's#/usr#././#g' libcrypto.so.1.1
sed -i -e 's#/usr#././#g' libdl.so.2
sed -i -e 's#/usr#././#g' libgcc_s.so.1
sed -i -e 's#/usr#././#g' libm.so.6
sed -i -e 's#/usr#././#g' libpthread.so.0
sed -i -e 's#/usr#././#g' libsqlite3.so.0
sed -i -e 's#/usr#././#g' libstdc++.so.6
#
sed -i -e 's#/lib#././#g' korni3
sed -i -e 's#/lib#././#g' libc.so.6
sed -i -e 's#/lib#././#g' libcrypto.so.1.1
sed -i -e 's#/lib#././#g' libdl.so.2
sed -i -e 's#/lib#././#g' libgcc_s.so.1
sed -i -e 's#/lib#././#g' libm.so.6
sed -i -e 's#/lib#././#g' libpthread.so.0
sed -i -e 's#/lib#././#g' libsqlite3.so.0
sed -i -e 's#/lib#././#g' libstdc++.so.6
strings korni3 | grep /usr


cat > $APP.desktop <<EOF
[Desktop Entry]
Type=Application
Name=korni3
Exec=korni3
Icon=icon
StartupNotify=false
Categories=Utility;
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

mkdir -p ./usr # NEED that ! WhY ? - i dont know. MUST have it . fuck

# generate_type2_appimage
cd ..
pwd 
VERSION=""
# to korni3--x86_64.AppImage
#'appimagetool-x86_64.AppImage' --appimage-extract
ARCH='x86_64' VERSION=$VERSION './appimagetool-x86_64.AppImage' -v -n ./$APP.AppDir/
#ARCH='x86_64' VERSION=$VERSION './appimagetool-x86_64.AppImage'  ./$APP.AppDir/

#mksquashfs Korni.AppDir/ Korni3.squashfs -root-owned -noappend
#cat Korni.AppDir/korni3  > korni3.AppImage
#cat Korni3.squashfs >> korni3.AppImage
#chmod a+x korni3.AppImage
#rm Korni3.squashfs

## CLEAR TODO 
#if [ -d "./$APP.AppDir/" ] ; then
#  rm -rf ./$APP.AppDir
#fi

rm -f appimagetool-x86_64.AppImage
apt install libsdl2-2.0-0 libsdl2-dev gcc-mingw-w64-i686-win32 upx-ucl zip wget
wget https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage
chmod 0755 appimagetool-x86_64.AppImage
mkdir SDL2
wget https://www.libsdl.org/release/SDL2-devel-2.26.2-mingw.tar.gz
tar -xf SDL2-devel-2.26.2-mingw.tar.gz
rm -f SDL2-devel-2.26.2-mingw.tar.gz
cp -r SDL2-2.26.2/i686-w64-mingw32/* SDL2
rm -rf SDL2-2.26.2

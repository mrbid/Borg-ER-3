mkdir release
rm release/borg
rm release/borg.exe
rm release/borg.AppImage
rm release/borg.deb
rm release/borg_win.zip
clang main.c -Ofast -lSDL2 -lm -o release/borg
i686-w64-mingw32-gcc -std=c17 main.c -ISDL2/include -LSDL2/lib -Ofast -Wall -lmingw32 -lSDL2main -lSDL2 -o release/borg.exe
strip --strip-unneeded release/borg
strip --strip-unneeded release/borg.exe
upx --lzma --best release/borg
upx --lzma --best release/borg.exe
cp release/borg borg.AppDir/usr/bin/borg
./appimagetool-x86_64.AppImage borg.AppDir release/borg.AppImage
cp release/borg deb/usr/bin/borg
dpkg-deb --build deb release/borg.deb
cd release
zip borg_win.zip SDL2.dll borg.exe

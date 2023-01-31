mkdir release
rm -f release/borg
rm -f release/borg_native
rm -f release/borg.exe
rm -f release/borg.AppImage
rm -f release/borg.deb
rm -f release/borg_win.zip
clang main.c -Ofast -lSDL2 -lm -o release/borg
clang main.c -march=native -Ofast -lSDL2 -lm -o release/borg_native
i686-w64-mingw32-gcc -std=c17 main.c -ISDL2/include -LSDL2/lib -Ofast -Wall -lmingw32 -lSDL2main -lSDL2 -o release/borg.exe
strip --strip-unneeded release/borg
strip --strip-unneeded release/borg_native
strip --strip-unneeded release/borg.exe
upx --lzma --best release/borg
upx --lzma --best release/borg_native
upx --lzma --best release/borg.exe
cp release/borg borg.AppDir/usr/bin/borg
./appimagetool-x86_64.AppImage borg.AppDir release/borg.AppImage
cp release/borg deb/usr/bin/borg
dpkg-deb --build deb release/borg.deb
cp SDL2/bin/SDL2.dll release/SDL2.dll
strip --strip-unneeded release/SDL2.dll
upx --lzma --best release/SDL2.dll
cd release
zip borg_win.zip SDL2.dll borg.exe
rm -f SDL2.dll

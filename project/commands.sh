cd "C:\Users\Eltaj Gafarli\CLionProjects\PageDB"

$env:Path += ";C:\Users\Eltaj Gafarli\Downloads\x86_64-15.2.0-release-posix-seh-ucrt-rt_v13-rev1\mingw64\bin"

mkdir build
cd build

cmake -G "Ninja" ..
cmake --build .
.\PageDB.exe
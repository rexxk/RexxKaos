vendor/premake/linux/premake5 gmake2
make -j4
pushd src/bootloader
make
mv bootloader.bin ../../bin
popd

bin/Debug-linux-x86_64/DiskEMU/DiskEMU -c disk.json


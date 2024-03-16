#make clean

mkdir bin

pushd kernel/bootloader
make
mv bootloader.bin ../../bin/
popd

pushd kernel/src/asm
make
mv entry.o ../../../bin
#mv kernel.sys ../../bin
popd

pushd kernel/src/hal
make
mv *.o ../../../bin
popd

vendor/premake/linux/premake5 gmake2
make -j4

bin/Debug-linux-x86_64/DiskEMU/DiskEMU -c disk.json


#LZ4
cd lz4; make -j 4 > /dev/null; insmod lz4_compress.ko;
cd ..;

# SBULL
make -j 4 > /dev/null
./sbull_load;
./bench1;
./sbull_unload;
./sbull_load;
./bench2;
./sbull_unload;
make clean > /dev/null;

#LZ4
rmmod lz4_compress; cd lz4; make clean > /dev/null;
cd ..;

make
./sbull_load;
mkfs -t ext4 /dev/sbulla;
mount /dev/sbulla /mnt/sbulla;
fio --directory=/mnt/sbulla --name test --direct=1 --rw=randwrite --bs=4k --size=1M --numjobs=8 --time_based --runtime=50 --group_reporting --norandommap
umount /mnt/sbulla;
./sbull_unload;

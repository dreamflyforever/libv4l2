#!/usr/bin/bash
adb root
adb shell mount -o rw,remount /
adb push capture /data/
adb push chmod +x /data/capture
adb -s $1 shell mkdir -p /data/$1
count=0;
while (($count < 1000000));
do adb -s $1 shell /data/capture $2;
((count=$count+1));
adb -s $1 shell mv "*.yuv" /data/$1
adb -s $1 pull /data/$1
adb -s $1 shell rm /data/$1/* -f
echo $count
done;

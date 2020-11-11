count=0;
while (($count < 1000000));
do adb shell /data/capture  /dev/video3;
((count=$count+1));
sleep 1
adb shell mv *.yuv /data/yyy
adb pull /data/yyy
adb shell rm /data/yyy/* -f
echo $count
done;

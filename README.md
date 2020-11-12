# libv4l2
对Linux v4l2接口进行封装，降低操作难度，主要内容在libv4l2目录下

## 实例介绍
- example_captrue
  采集一张yuv图片
- example_video2lcd
  采集图像在lcd预览，这个需要在嵌入式平台使用，需要修改makefile的工具链
- example_qt
  采集图像在qt显示，这是单独的一块，内部已经含有v4l2的封装了
  
## 使用方法
将文件拷贝到Linux环境下，执行`make`，可以得到`capture`和`video2lcd`
### example_captrue

执行`capture /dev/video0`可以采集一张yuv图像，可以使用`ffplay -pixel_format yuyv422 -f rawvideo -video_size 640x480 pic.yuv`查看


![在这里插入图片描述](https://img-blog.csdnimg.cn/20190719101937797.PNG?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjQ2MjIwMg==,size_16,color_FFFFFF,t_70)

### example_video2lcd

执行`video2lcd /dev/video0`可以在lcd上预览，这个是在嵌入式平台运行的，需要在makefile需要工具链
![在这里插入图片描述](https://img-blog.csdnimg.cn/201907191626275.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjQ2MjIwMg==,size_16,color_FFFFFF,t_70)

### example_qt

这个是单独的一个模块，直接使用Qt IDE打开，点击运行即可，默认打开的是`/dev/video0`

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190719162235712.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjQ2MjIwMg==,size_16,color_FFFFFF,t_70)

## 详情请看
[深入学习Linux摄像头（一）v4l2应用编程](https://blog.csdn.net/weixin_42462202/article/details/96826526#21_Querying_Capabilities_46)

## bash test unit
./go.sh adb_devices /dev/video

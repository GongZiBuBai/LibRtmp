# LibRtmp

本例子实现了推送本地视频至流媒体服务器（以RTMP为例）。
是使用librtmp进行流媒体推送最简单的教程。

解决方案包含3个项目：
librtmp:编译librtmp静态库
librtmpTest(send): 将本地视频文件推送至流媒体服务器。
librtmpTest: 将流媒体数据保存成本地文件。

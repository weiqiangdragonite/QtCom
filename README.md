QtCom
=====





http://bbs.qter.org/forum.php?mod=viewthread&tid=94
http://bbs.qter.org/forum.php?mod=viewthread&tid=42&extra=page%3D1
http://mobile.51cto.com/symbian-272181.htm

Qt中并没有特定的串口控制类，现在大部分人使用的是第三方写的qextserialport类，
我们这里也是使用的该类。我们可以去官方下载:
https://code.google.com/p/qextserialport/wiki/Downloads

下载的文件为: qextserialport-1.2rc.zip

(1) doc文件夹中的文件内容是QextSerialPort类和QextBaseType的简单的说明，
    我们可以使用记事本程序将它们打开。
(2) examples文件夹中是几个例子程序，可以看一下它的源码，不过想运行它们好像
    会出很多问题啊。
(3) src文件夹中是QextSerialPort类的代码，真正使用到的源文件都存这个文件夹中。

在windows下需要用到的文件有：
qextserialport.cpp
qextserialport_global.h
qextserialport.h
qextserialport_p.h
qextserialport_win.cpp

在linux下需要的文件有：
qextserialport.cpp
qextserialport_global.h
qextserialport.h
qextserialport_p.h
qextserialport_unix.cpp




步骤一：设置串口参数，如：波特率，数据位，奇偶校验，停止位，数据流控制等。
步骤二：选择串口，如windows下的串口1为“com1”，Linux下为“ttyS0”等，并打开串口。
步骤三：读或写串口。
步骤四：关闭串口。

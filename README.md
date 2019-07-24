使用教程https://www.bilibili.com/video/av52111697/?p=2


使用说明


1.


因为不是直接解析静态elf文件，所以需要先将要打包的xxx运行起来,这样才可以获取到so相关信
息.直接在输入框内输入xxx即可.不需要带上路径.


2.


点击get so info就可以,获得xxx所依赖的so.


3.


依据所需要的so在列表中选取,也可全选,但是有不少是不需要的,请谨慎


4.


点击复制,复制完成后同时生成一个install.sh,在需要部署得pc上 运行这个脚本 
注意：如有权限问题请 chmod 或者sudo su切换为超级用户
      需要在可执行文件下有一个图标文件 启动后任务栏即可显示图标
      
5. 

如果自行设置了库目录,那么你可能会依然出现找不到依赖的so文件 本程序仅依照qt

默认的规则拷贝so文件到相应目录.此外的可能会造成失败.

logo from https://www.iconfont.cn/search/index?q=qtsfkuan

如有问题请告知.

eg.

install.sh

#!/bin/bash

appdir=$(pwd)

echo $appdir >> /etc/ld.so.conf.d/fakeroot-x86_64-linux-gnu.conf

ldconfig

touch /usr/share/applications/QtRelease.desktop

echo [Desktop Entry] >> /usr/share/applications/QtRelease.desktop

echo Name=QtRelease >> /usr/share/applications/QtRelease.desktop

echo Name[zh_CN]=QtRelease >> /usr/share/applications/QtRelease.desktop

echo Comment=QtRelease Client>> /usr/share/applications/QtRelease.desktop

echo Exec=$appdir/QtRelease >> /usr/share/applications/QtRelease.desktop

echo Icon=$appdir/logo.ico >> /usr/share/applications/QtRelease.desktop

echo Terninal=false >> /usr/share/applications/QtRelease.desktop

echo Type=Application >> /usr/share/applications/QtRelease.desktop

echo 'Categories=Application;' >> /usr/share/applications/QtRelease.desktop

echo Encoding=UTF-8 >> /usr/share/applications/QtRelease.desktop

echo StartupNotify=ture >> /usr/share/applications/QtRelease.desktop


eg. QtRelease.desktop

[Desktop Entry]

Name=QtRelease

Name[zh_CN]=QtRelease

Comment=QtRelease Client

Exec=/home/chf/Desktop/build-QtRelease-Desktop_Qt_5_12_0_GCC_64bit-Release/QtRelease

Icon=/home/chf/Desktop/build-QtRelease-Desktop_Qt_5_12_0_GCC_64bit-Release/logo.ico

Terninal=false

Type=Application

Categories=Application;

Encoding=UTF-8

StartupNotify=ture

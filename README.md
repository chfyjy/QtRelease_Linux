使用说明
1.
因为不是直接解析静态elf文件，所以需要先将要打包的xxx运行起来,这样才可以获取到so相关信
息.直接在输入框内输入xxx即可.不需要带上路径.
2.
点击获取dll信息就可以,获得xxx所依赖的so.
3.
依据所需要的so在列表中选取,也可全选,但是有不少是不需要的,请谨慎
4.
点击复制,复制完成后同时生成一个install.sh,在需要部署得pc上 运行这个脚本 
注意：如有权限问题请 chmod 或者sudo su切换为超级用户

如有问题请告知.

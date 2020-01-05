# AutoTG
auto test GUI or Game by writing lua scripts.

# 版本区别

periphery-o:one exe

periphery-t:two exe

# 编译依赖

[xv_dep](https://github.com/coolxv/xv_dep):编译依赖的第三方库及其头文件

# 编译目录布局
```
AutoTG
│   README.md
│
└───xv_dep
│   │
│   └───fflua
│       │   ...
│   
└───periphery-o
│   │
│   └───periphery_c
│   └───periphery_m
│   
└───periphery-t
│   │
│   └───periphery_c
│   └───periphery_m

```
# 运行依赖
[zh_script](https://github.com/coolxv/zh_script):运行时的示例脚本目录

[xv_res](https://github.com/coolxv/xv_res):运行时的依赖库，windows自身库，有的机器上没有

[de_srv](https://github.com/coolxv/de_srv):账号管理，登录认证的服务程序，在linux部署

[atmega32u4-usb-dev](https://github.com/coolxv/atmega32u4-usb-dev):硬件模拟键盘鼠标

[an_tool可选](https://github.com/coolxv/an_tool):对脚本进行加密，程序加壳等安全操作

# 运行目录布局
```
zh_script
│
└───使用说明
└───lib                   //xv_res
│   gp.lic
│   gp.script 
│   gp.lic
│   gps.conf 
│   gpu.conf
│   gpatc.dll             //gpatc.exe  
│   km.dll                //atmega32u4-usb-dev, keyboard and mouse by writing code control
│   gpatm.exe 
│   ...

```

# 程序使用

双击运行 gpatm.exe

# 莞香客户端——基础模块

客户端重新编写了原来的莞香客户端，用静态or动态链接的方式加载必须库，开源基础模块的目的为让所有初学者能从中获得编程的成就感，让更多的同学参与到客户端的开发中来， 授人渔永远比授人鱼重要。

![新莞香客户端](https://github.com/NullYing/GxClient/raw/master/images/NewGxClient.png)

## 基础模块介绍

* BaseFuntion —— 基础的功能函数，主要为加密解密函数，编码转换函数，中兴验证码识别函数，管理员权限检测函数等等
* http —— 网络函数，提供get与post方法函数，文件下载函数，文件post到服务器函数，302状态测试函数等网络连接
* Logger —— 日志记录函数，在armink的[EasyLogger](https://github.com/armink/EasyLogger)基础上制作成dll，提供不同等级的日志记录功能
* Weather —— 库调用demo，利用中国天气的API，和个人在中国天气上抓下载的根据IP地址定位城市，示例以上各个库的用法

以上各个库会根据客户端的改进而同步开源,API文件夹存放各个库的调用头文件

## 编译环境

* VS2015
* 使用的开源库：curl（http.dll编译必须）、zlib、libpng（用来识别验证码）

## 注意事项

如果使用该源码，请遵守[GPL开源协议](https://github.com/NullYing/GameUpdate/raw/master/LICENSE)

## 使用帮助

* QQ群: [437249421](http://jq.qq.com/?_wv=1027&k=27VPs1q)

# 莞香广科

[莞香广科](http://www.gxgk.cc)是学生自建的互联网创新团队。团队成立于2012年5月，以校园论坛起步，逐渐发展了校园网资源共享站、媒体中心、微信助手等项目。

![莞香广科](http://www.gxgk.cc/images/logo.png)

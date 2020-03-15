# QT串口调试助手
  一个基于QT的串口调试助手，实现了基本收发功能、绘图功能、数据保存、关键字高亮等功能，并支持STM32F1和F4系列的自动下载。
![mainwindow](mainwindow.png)
![mainwindow](graphwindow.png)
![mainwindow](scatterline.png)
![mainwindow1](multistring.png)
# 已知问题
  - 代码重构/优化/注释
  - 未打卡串口时，打开STM32-ISP再关闭时，弹出串口打开失败的问题
  - 
# 拟增功能
  - 多语言支持。
  - GBK编码支持
  - 发送文件改为非阻塞式的，需要显示进度
  - 打开串口时拔出串口，如何检测发送失败？serial.write不行
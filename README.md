# QT串口调试助手
  一个基于QT的串口调试助手，实现了基本收发功能、绘图功能、数据保存、关键字高亮等功能，并支持STM32F1和F4系列的自动下载。

绘图协议：

```c
//ASCII格式协议
//几条曲线就几个变量。
printf("{:%f,%f}\r\n", data1, data2);
```

![demo](screenshoot/demo.gif)
![mainwindow](screenshoot/mainwindow.png)
![mainwindow](screenshoot/mainwindow2.jpg)
![graphwindow](screenshoot/graphwindow.png)
![scatterline](screenshoot/scatterline.png)
![multistring](screenshoot/multistring.png)

# 计划清单
  - 代码重构/优化/注释
  - 测试stm32a07版本是否正常
  - ISP Tool测试自动下载电路 -ADC。以及使用体验完善
  
# 考虑中的功能
  - 多语言支持？
  - 自动滚屏开关
  - 串口热插拔检测。（需要WinAPI）
  - XYZModen协议支持
  - DeBug转log文件
  - 数值显示器列宽度是否支持手动调整（目前看来必要性不是很强）

# 暂时无法实现的功能
  - 图像Y轴只按显示的区域最大值调整（暂时没有合适的方法实现：1.自己造轮子在高密度绘图时容易遇到性能问题；2.图形库的API间接实现时，在Y轴不变化即直线的情形时会正反馈发散，可能是由于API对直线设为居中，但不限制）
  - 隐藏绘图数据（没有适合的办法辨别满足绘图协议但未接收完的数据）
  
# 奇思妙想
  - 布尔控件、滑动条控件显示
  - ASCII协议下实现绘图、文本、数值的分离显示：
    - ```{:...}``` 表示绘图数据，```...```为绘图数据，用逗号分隔
    - ```{TXT:...}``` 表示分类文本数据，```...```为文本内容，用分号/逗号分隔？
    - ```{NUM:...}``` 表示分类数值数据，```...```为数值内容，用逗号分隔
    - ```{RAW:...}``` 表示透传数据，```...```为透传内容，不做分隔，直接显示在数据显示区
  - 绘图器游标功能与差值显示
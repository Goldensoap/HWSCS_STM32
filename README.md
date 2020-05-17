# 多空间自组网网关（STM32部分）
## 项目简介
本项目为多空间自组网系统的**网关部分**，采用低功耗嵌入式设备（STM32f103），为自组网提供转换网关和一定程度的网内自动控制。
## 功能
- 双向传输信息、指令（上位机<——>自组网）
- 存储、分析自组网的传感信息
- LCD显示相关信息
## 环境依赖
- 硬件平台：STM32f103、LCD液晶屏
- 实时操作系统：FreeRTOS
## 部署步骤
- 烧录工程文件
- 连接无线接收器
- 开始工作
## 目录结构
```shell
├─CORE（CortexM3启动文件）
├─FreeRTOS（FreeRTOS相关源码）
│  ├─include 
│  └─portable（FreeRTOS关于具体设备的移植接口）
│      ├─Keil       
│      ├─MemMang    
│      └─RVDS       
│          └─ARM_CM3
├─HARDWARE（底层硬件相关，提供初始化、配置、操作硬件函数）
│  └─include        
├─OBJ（编译器生成目标文件夹）
├─STM32F10x_FWLib（STM32标准硬件驱动）
│  ├─inc
│  └─src
├─SYSTEM（网关系统相关，使用FreeRTOS API 设计运行任务）
│  ├─include（sys_task_pub.h //系统公共头文件，包含编译选项，任务定义以及公共队列区）
│  └─Interface（系统硬件接口层相关任务：同时操作 RTOS API和硬件）
└─USER（工程文件，主函数，测试文件，CortexM3针对STM32的设备支持文件）
```
## 更新日志
[更新日志](./更新日志.md)
## 协议
[MIT协议](LICENSE.md)
## 其他
### 任务创建格式建议
- 新创建一个任务，在sys_task_pub.h 中定义好优先级、栈堆、句柄和任务函数，去SYSTEM文件夹下新建任务.c 文件，同名头文件放入include
- 头文件引用sys_task_pub.h
- C文件引用同名头文件，定义好任务主体函数，定义好任务句柄。
- 公共队列，在队列创建的任务函数文件中定义实体。
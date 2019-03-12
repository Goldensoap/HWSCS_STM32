## 文件结构说明
- **CORE** //CortexM3启动文件
- **FreeRTOS** //FreeRTOS相关源码
  - **include** //头文件夹
  - **portable** //FreeRTOS关于具体设备的移植接口
  - FreeRTOS 模块.c
- **HARDWARE** //底层硬件相关，提供初始化、配置、操作硬件函数
  - **include** 头文件
  - 硬件操作.c
- **OBJ** //编译器生成目标文件夹
- **STM32F10xFWLib**  //STM32标准硬件驱动
- **SYSTEM** //采集系统相关，使用FreeRTOS API 设计运行任务
  - **include** //头文件夹
    - sys_task_pub.h //系统公共头文件，包含编译选项，任务定义以及公共队列区
  - **interface** //系统硬件接口层相关任务（同时操作 RTOS API和硬件）
  - 系统任务文件.c （与硬件无关，单纯操作RTOS API）
- **USER** //工程文件，主函数，测试文件，CortexM3针对STM32的设备支持文件


#任务创建格式建议
- 新创建一个任务，在sys_task_pub.h 中定义好优先级、栈堆、句柄和任务函数，去SYSTEM文件夹下新建任务.c 文件，同名头文件放入include
- 头文件引用sys_task_pub.h
- C文件引用同名头文件，定义好任务主体函数，定义好任务句柄。
- 公共队列，在队列创建的任务函数文件中定义实体。
# baremetal-hello

一个 baremetal (no os) 的 hello 程序，可通过 `-ffreestanding` 编译选项切换

# calculator

Android 计算器源码，使用 Java 语言编写

# concurrency

并发部分相关的示例代码

在头文件中包装了线程池 / 自旋锁 / mutex / 信号量 / 条件变量的实现

提供了 model checker 检查程序是否存在并发 bug

附有期中考试题

## fib

展示 go 的轻量化协程

## fish

使用条件变量同步线程，往年期中考试题

## hello

简单的线程并发打印示例

## mandelbrot

使用多线程加速渲染 Mandelbrot Set

## mem-ordering

表现多处理器间即时可见性的丧失

## memset-race

表现 memset 线程不安全

## pc

使用 go / mutex / 信号量 / 条件变量实现生产者-消费者模式，并使用 python 脚本检查正确性

## peterson

使用 peterson 算法实现互斥，表现内存访问顺序的丧失

## philosopher

经典的哲学家就餐问题

## printf-race

表现 printf 线程不安全

## stack-probe

探测每个线程栈空间的大小

## sum

经典的计数问题

## working-directory

探索线程之间工作目录是否共用

# dl

自定义一个动态链接二进制格式，配有相应的工具链

# flock

使用 flock 系统调用触发死锁，揭示了 Android 后台杀不死的邪术

# hanoi

非递归汉诺塔，显式模拟函数调用和返回的语义

# linux-minimal

最小的 linux

其中包含了两份 vmlinuz

- Linux kernel x86 boot executable bzImage, version 5.17.3 (vgalaxy@vgalaxy-virtualbox) #1 SMP PREEMPT Fri May 20 16:00:53 CST 2022, RO-rootFS, swap_dev 0X6, Normal VGA
- Linux kernel x86 boot executable bzImage, version 5.4.0-42-generic (buildd@lgw01-amd64-038) #46-Ubuntu SMP Fri Jul 10 00:24:02 UTC 2020, RO-rootFS, swap_dev 0XB, Normal VGA

两份 init

- busybox
- xv6 shell

还包含了一个设备驱动程序

# logisim

七段数码管模拟器，表明数字电路就是一个状态机

# minimal-hello

使用汇编语言进行系统调用，实现一个 baremetal 的 hello 程序

# persistence

持久化部分相关的示例代码

## fish-dir

通过 ln 命令在文件系统层面创建自动机

## loop

一个指向文件所在目录的软链接

## pci-probe

通过 abstract-machine 显示 qemu 模拟出的硬件信息

## disk.img

一个 fat 文件系统镜像

## ext2.h

ext2 文件系统源码

# relocation

在特定的编译条件下展示重定位的语义

# shell

xv6 shell 的 baremetal 实现

# virtualization

虚拟化部分相关的示例代码

## bootmain

abstract-machine 中加载操作系统内核部分的代码

## cow-test

测试写时复制

## dfs-fork

使用 fork 简化 dfs 的回溯

## dosbox-hack

利用 gdb 的后门进行游戏数据的定位和修改

## dsu

通过修改 jmp 的目标实现软件热更新

## env

实现简单的 env 命令

## execve / fork / exit

一些系统调用的示例代码，表现了系统调用的某种复杂性

## loader-static

在用户态实现静态链接程序的加载

## mmap

mmap 系统调用的示例代码，表现其 lazyiness

## popcount

展示编译优化给调试带来的挑战

## posix-spawn

摘自 spawn 函数手册页的示例代码

## signal-handler

信号处理程序示例代码

## unwind

通过 addr2line 实现简单的函数调用回溯信息的打印

## vdso

用于展现程序地址空间中的 vdso

# inst-count.py

使用 python 编写 gdb 脚本统计程序执行指令数
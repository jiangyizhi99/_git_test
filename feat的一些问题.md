1. tag 是什么?

enum nccode_t是NcCode的编码罗列，NcCode tag是nccode_t的编码，算是一个标识。

可以参考代码是这么写的

```c++
enum nccode_t{...};

struct NcCode: public Part
{
    public:
    NcCode(){}
    NcCode(nccode_t tag_)
    {
        nTag = tag_;
    }
    
    virtual ~NcCode(){}
};
```

2. NcStep 是不是一种 NcCode?

step的意义在于走多少距离和时间，NcCode当中应该包含了step的内容，但step或许是一种旧式的编码指令？为了保持软件的运维和通用性，所以将NcStep搬过来了，NcStep实际上是一种通信协议，NcCode也算某种协议，算是一种规范，所以NcStep应该算是一种NcCode。

3. tag 中有一行注释 0x00001000以下是解析器内部指令, 不会流出解析器, 这个是什么意思?

在数据流当中， 0x00001000以下的编码是解析器内部使用的，不会传导到后续的工作流当中，是解析器内部指令

4. AXDOUBLE 是什么?

轴名称，存一个跟轴相关的数据

5. NcStep 和 NcCode 的 tag 有什么区别? 

NcCode的tag是惟一的，NcStep的tag包含信息不是一种，多个信息

6. 写一个 NcStep 的 Tag, 其代表的含义是: G00 运动, 是**最后一条**运动, **倍率 100%**

0x00011201

7. 同步包是什么? 有哪些同步包?

数据包

\1. NcCodePrinter 这个模块的作用是什么

定义DLL应用程序的导出函数？如果外部传入为空，则生成错误日志。

GetProcAddress?得到啥的地址？

\2. 什么是进程? 线程? 纤程?

进程是一个程序的运行状态，进行是os分配资源(内存空间)的基本单位。

线程是一个进程中不同执行路径，执行调度的基本单位，线程共享进程的独立空间。

用户空间的线程，用户空间负责调度等，不涉及内核调用。

\3. 什么是 CWorker, 和 NcCodePrinter 是什么关系

CWorker是父类，一个基本class，然后会有 class CNcCodePrinter : public CWorker，CNcCodePrinter用于输出流水线中的NcCode

\4. 什么是流水线(Pipeline), 和 NcCodePrinter 是什么关系

Pipeline是一个多级串联的生产/消费者模型，包含了流水线模型的接口，流水线模型是现实世界生产流水线的类比，它和其他两个类比：worker（工人）和part（零件）一起实现多级生产/消费者模型。

在现实世界中，流水线大概是这个样子的：有多个级，级间有传送带，用于单向传递被加工零件，每个级都有工人，负责零件加工：他们从传送带取零件，自己将完成后，放到传送带上流传到下一级，在流水线的末端，得到的是成品。

pipeline对上述现实世界中流水线进行抽象：流水线有1个到多个级，每一级有且只有一名orker，级间用一个FIFO类型的数据结构链接，用于数据对象part零件在级间的缓冲。

流水线模型是为了给复杂的算法进行解耦，以便于将原本互相牵扯在一起的多个算法能够彼此隔离开来，让每一个算法都专注于算法本身，而数据在算法间的流转与缓冲等工作则交给模型来处理。

\5. 除了 NcCodePrinter, 还有哪些 Worker

BatchWorker	BezierTransitionWorker	   CmdBrokerWorker	CutterCompensationWorker

DxfWorker		EngParserWorker				EstimateWorker		ParserWorker

HighPrecisionContourSmoothWorker		   KineticPreProcessWorker

NccmdIssuerWorker									 NcCodePrinterWorker

NcexParserWorker										NcPreAnalysisWorker

NormalFollowWorker									 PltParserWorker		ScriptWorker

StepWorker		TechnologyWorker			  TinyMotionRemoverWorker .etc

\6. 流水线是怎么将 Worker 串联起来工作的

FIFO数据结构连接，

for(;<直到终止条件发生>;)

{

Part* p =Read();

Part* q = f(p);

Write(q);

}

\7. 流水线是怎么进行调度的？

数据传入传出？FIFO？

\8. 对外提供的接口是什么

流水线指针/pipeline名称

\9. GetProcAddress 这个函数的作用是什么

?与lua接口同名同参数同返回值的空函数，在改函数之前添加doxygen注释，方便生成相应的文档。

该函数只在lua中被调用，用于获取其他各个c++接口函数的指针

\10. Private.h 这个头文件是用来做什么的

实现一些错误信息日志生成的宏，包含一些封装掉的接口和参数

\11. c_nNumOfAxis 是用来做什么的 

nNumOfAxis是物理轴数，c_nNumOfAxis...没找到

const int c_nNumOfAxis = 6

猜测应该是区分6/9/16轴软件

\12. WRITE_ERROR_LOG 这个宏是用来做什么的? 为什么可以做到

写错误日志，输出错误信息，宏展开后直接替换宏代表的内容

\13. 如果调用 WRITE_ERROR_LOG 宏的时候, 传入了一个 2000 字符的字符串, 会发生什么

信息输入超过缓冲区，导致溢出，程序直接崩溃

\14. 如果调用 WRITE_ERROR_LOG 宏的时候, 传入了一个 1024 字符的字符串, 并且最后一个是转义字符’\’, 会发生什么

数据会溢出，程序会崩溃

\15. NCCODEPRINTER_API 这个宏是用来做什么的

dll中的关键字，导出/导入，应该可以理解成dll生成之后和外界的接口

在提供者那里方法应该被声明为_declspec(dllexport)

在使用者那里，方法应该被声明为_declspec(dllimport)

\16. version.h 这个头文件用来做什么的

定义了一些宏？具体是干啥用的没查到

\17. stdafx.h 和 stdafx.cpp 是用来做什么的

标准系统包含文件的包含文件？或是经常使用但是不常更改的、特定于项目的包含文件

只包括标准包含文件的源文件

\18. target.h 这个头文件是用来做什么的

？targetver.h将定义可用的最高版本的Windows平台

\19. dllmain.cpp 是用来做什么的

定义dll应用程序的入口点

\20. [CNcCodePrinter](http://CNcCodePrinter.Do)::Do 这个函数是用来做什么的, 简述下其处理的逻辑

读取NcCode，根据传入的打印控制开关，分别控制是否输出到文件

打开文件，如果tag不为end的时候，就循环读入数据，catch异常信息，将bool_bRet打成false，将文件关闭，返回刚刚那个bool值

\21. 在 Do 函数里使用 try / catch 捕获了两种异常, 字符串和 std::exception, 这些分别是适用于什么场景的? 如果在异常中添加 catch (...) 会发生什么?

\22. new (m_pWorker->GetName()) NcCode 这种 new 操作是什么语法


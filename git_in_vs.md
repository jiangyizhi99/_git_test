# gtest in vs

在GitHub上获取最新的gtest源码，git bash中git clone 下载，下载好代码进入Googletest/Googletest/msvc/2010，有两个sln，gtest.sln是静态运行库的解决方案，适合在没有安装vs运行库的环境下执行，gtest-md.sln是使用vs动态运行库的解决方案。

双击打开解决方案，按照vs提示的升级，电极确定，然后在解决方案管理器里面右键生成，编译gtest库。一般情况下编译会通过，编译完成后在*mscv/2010/gtest-md*目录下生成库文件。

编译生成好库文件以后，可以简历gtest工程来做测试，首先使用vs创建一个空白的c++项目，选择创建控制台项目，然后在项目中添加gtest的头文件和库文件。

右击项目-属性-c/c++-常规-附加包含目录-包含刚才从GitHub上下载下来的源码include文件路径

右击项目-属性-链接器-输入-附加依赖项-包含刚刚生成的库文件路径

创建main.cpp文件，编写main函数，运行gtest。

```c++
#include <gtest/gtest.h>
#include <stdlib.h> 
#include <stdio.h>

/*调用类初始化，初始化接收拥护命令行传入参数*/
class CGlobalEnvironment :public testing::Environment
{
public:
	virtual void SetUp()
	{
		//TODO: Do something before every case
	}

	virtual void TearDown()
	{
		//TODO: Do something after every case
	}
};

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	testing::AddGlobalTestEnvironment(new CGlobalEnvironment());
	testing::FLAGS_gtest_filter = "*";
    RUN_ALL_TESTS();	//自动运行我们做的测试用例集
	system("PAUSE"); 	//命令行停在当前窗口

	return 0;
}

```

使用gtest测试我们需要将被测试的源码加入到测试工程中来，然后使用TEST宏来编写对应的测试用例，TEST宏接收两个参数，第一个是测试用例集（testcase）的名字，第二个是测试用例（test）的名称，我们做单元测试的时候会运行很多的测试用例集，其中每个测试用例集中又有很多测试用例，gtest会自动按照我们的每个测试用例集来运行测试用例。



visual studio 2010

http://www.mamicode.com/info-detail-1778058.html

如果遇到

“vs 开发 qt 遇到 无法找到 Visual Studio 2010 的生成工具(平台工具集 =“v100”) 解决方案”


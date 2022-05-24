# Gtest

断言相关宏：ASSERT\EXPECT

1. ASSERT_*：当检查点失败时，退出当前函数（并非退出当前案例）
2. EXPECT_*：当检查点失败时，继续往下执行

用<<输出一些重要信息

ASSERT_*如果检查失败会立即终止函数，可能就会跳过程序后面的清理内存的步骤，可能会出现内存泄露的情况

TEST()宏的第一个参数是Test Case的名称，第二个参数是隶属于第一个Test Case参数的Test名称，Gtest会根据Test Case对测试结果进行分组，所以一些相关的Test应该要放在同一个Test Case中。

TEST()参数从一般到具体。第一个参数是测试套件的名称，第二个参数是测试套件中的测试名称。两个名称都必须是有效的c++标识符，***并且它们不应该包含任何下划线(_)***。一个测试的全名由它所包含的测试套件和它单独的名称组成。来自不同测试套件的测试可以具有相同的单独名称。

eg.

```c++
int Factorial (int n );//return the factorial of n （阶乘）

TEST (FactorialTest, HandleZeroInput){
    EXPECT_EQ(Factorial (0),1);
}

TEST(FactorialTest, HandlePositiveInput){
    EXPECT_EQ(Factorial(1),1);
    EXPECT_EQ(Factorial(2),2);
    EXPECT_EQ(Factorial(3),6);
    EXPECT_EQ(Factorial(8),40320);
}
```

## Test Fixtures ：对多个测试使用相同的数据配置

如果您发现自己正在编写两个或更多类似数据进行操作的测试，那么可以使用一个测试fixture，这允许您为几个不同的测试重用相同的对象配置。

如何创建一个fixture:

1. 从::testing::Test派生一个类。从它的主体开始，使用protected:，因为我们想要访问子类中的fixture成员
2. 在类内部，声明您计划使用的任何对象
3. 如果有必要，编写一个默认构造函数或SetUp()函数来为每个测试准备对象，一个常见的错误是将SetUp()拼写为带小写u的函数，在C++11中使用覆盖来确保拼写正确。
4. 如果有必要，编写析构函数或TearDown()函数来释放在SetUp()中分配的任何资源，要了解什么时候应该使用构造函数/西沟函数，什么时候应该使用SetUp/TearDown，请点击链接。https://google.github.io/googletest/faq.html#CtorVsSetUp
5. 如果需要，为您的测试定义要共享的子例程。

当使用fixture时，要使用TEST_F()而不是TEST()，因为它允许你访问测试fixture中的对象和子例程：

```c++
TEST_F(TestFixtureName, TestName){
    //测试体
}
```

与TEST()一样，第一个参数是测试套件的名称，但对于TEST_F()，这个参数必须是测试装置类(the name of the test fixture class)的名称。

不幸的是，c++宏系统不允许我们创建一个宏来处理这两种类型的测试，使用错误的宏会导致编译器错误。

此外，在TEST_F()中使用测试类fixture之前，必须首先定义它，否则将得到编译器错误"虚外部类声明"。

对于用TEST_F()定义的每个测试，googletest将在运行时创建一个新的测试fixture，通过SetUp立即初始化它，运行测试，通过调用TearDown进行清理，然后删除测试fixture。注意，同一个测试条件中的不同测试具有不同的测试fixture对象，并且googletest总是在创建下一个测试fixture之前删除一个测试fixture，googletest不会对多个测试重复使用同一个测试夹具。一个测试对fixture所做的任何更改都不会影响其他测试。

eg.为一个名为queue的FIFO队列类编写测试，它有以下接口(interface)

```c++
template<typename E>	//E 是元素类型
class Queue{
    public:
    Queue();
    void Enqueue(const E& element);
    E* Dequeue();		//return NULL if the queue is empty.
    size_t size() const;
    ...
};
```

首先，定义一个fixture类。按照约定，应该将其命名为FooTest，其中Foo是要测试的类。

```c++
class QueueTest : public ::testing::Test{
    protected:
    void SetUp() override{
        q1_.Enqueue(1);
        q2_.Enqueue(2);
        q2_.Enqueue(3);
    }
    
    // void TearDown() override{}
    
    Queue<int> q0_;
    Queue<int> q1_;
    Queue<int> q2_;
};

TEST_F(QueueTest, IsEmptyInitially){
    int* n = q0_.Dequeue();
    EXPECT_EQ(n,nullptr);
    
    n = q1_.Dequeue();
    ASSERT_NE(n,nullptr);
    EXPECT_EQ(*n,1);
    EXPECT_EQ(q1_.size(),0);
    delete n;
    
    n = q2_.Dequeue();
    ASSERT_NE(n,nullptr);
    EXPECT_EQ(*n,2);
    EXPECT_EQ(q1_.size(),1);
    delete n;
}
```




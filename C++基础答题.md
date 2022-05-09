如下的问题都是基于 姜-《对象和类概念.md》。下面的题目都没有经过调试，可能有一些拼写和语法错误，请顺便修正 (姚子南- 2022-0505).



1、下面的程序有什么问题，如果有问题应该如何修正

```
class Student{
  public:
        Student(const char * pName_){
            m_pName = (char*)malloc(strlen(pName_));
            memcpy(m_pName,pName_,strlen(pName_));
        }
        int GetAge(){return m_nAge;}
        char* GetName(){return m_pName;}
  private:
       int m_nAge;
       char* m_pName;  
};

void main(){
    Student s1("xiaowang");
    Sdudent s2 = s1;
}
```

2、实现一个复数类 CComplex，要求可以实现复数的四则运算，并且可以通过 << 打印复数的值

```
//把复数类的实现补充完整
clss CComplex{
};

voi main(){
    CComplex c1(3,4);
    CComplex c2(5,6);
    CComplex c3 = c1 + c2;
    c1 -= c2;
    c3 += c1;   
    CComplex c4 = c1 * c2;
    std::cout << c4 << std::endl;
}
```

3、根据程序的执行结果，实现类 CBird 和 CParrot

```
//实现bird和parrot类
class CBird; class CParrot;

void main(){
    CBird * _b = new CParrot();
    _b->Fly();
    
    _b = new CBird();
    _b->Fly();
}

函执执行打印如下：
Parrot is flying
Bird is flying
```

4、实现类B，以及一个继承于B的类A，要求B中的所有成员函数外部都看见，但所有的成员变量外部都不可见；同时希望类A可以访问类B中的所有成员








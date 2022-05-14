#include <iostream>

class Student {
public:
    Student(const char* pName_) {
        m_pName = (char*)malloc(strlen(pName_));
        if (m_pName) {
        memcpy(m_pName, pName_, strlen(pName_));
        }
    }
    int GetAge() { return m_nAge; }
    char* GetName() { return m_pName; }
private:
    int m_nAge;
    char* m_pName;
};

void main_2() {
    Student s1("xiaowang");
    Student s2 = s1;
}
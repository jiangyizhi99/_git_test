#include <gtest/gtest.h>
#include <stdlib.h> 
#include <stdio.h>
#include "printf_test_demo.h"

class CGlobalEnvironment :public testing::Environment
{
public:
	virtual void SetUp()
	{
		
	}

	virtual void TearDown()
	{
		
	}
};

int foo(int a, int b) {
	if (0 == a || 0 == b) {
		throw "something wrong";
		return -1;
	}
	else {
		int c = a % b;
		if (!c)
			return b;
		else
			return foo(b, c);
	}
}

TEST(PRINTER, test_demo)
{
	char test[] = "Hello World!\n";
	EXPECT_STREQ(PRINTER::PRINT_test(test), test);
}

TEST(PRINTER, test_snprintf) 
{
	char temp_int[BUFSIZ];
	int i = 1;
	EXPECT_STREQ(PRINTER::PRINT_INT(i, temp_int), "1");
	i = -1;
	EXPECT_STREQ(PRINTER::PRINT_INT(i, temp_int), "-1");
	i = 0;
	EXPECT_STREQ(PRINTER::PRINT_INT(i, temp_int), "0");

	char test[BUFSIZ] = "Hello!\n";
	char temp_char[BUFSIZ];
	EXPECT_STREQ(PRINTER::PRINT_CHAR(test, temp_char), test);

	char temp_float[BUFSIZ];
	float f = 1.0;
	EXPECT_STREQ(PRINTER::PRTNT_FLOAT(f, temp_float), "1.00000") << "float failed 1.0";
	f = 1.1;
	EXPECT_STREQ(PRINTER::PRTNT_FLOAT(f, temp_float), "1.10000") << "float failed 1.1";
	f = -1.0;
	EXPECT_STREQ(PRINTER::PRTNT_FLOAT(f, temp_float), "-1.0000") << "float failed -1.0";
	f = -1.1;
	EXPECT_STREQ(PRINTER::PRTNT_FLOAT(f, temp_float), "-1.1000") << "float failed -1.1";
	f = 0.0;
	EXPECT_STREQ(PRINTER::PRTNT_FLOAT(f, temp_float), "0.00000") << "float failed 0.0";

	char temp_double[BUFSIZ];
	double d = 3.1415926;
	EXPECT_STREQ(PRINTER::PRTNT_FLOAT(d, temp_double), "3.14159") << "double failed 3.14";
	d = -3.1415926;
	EXPECT_STREQ(PRINTER::PRTNT_FLOAT(d, temp_double), "-3.1415") << "double failed -3.14";

	char temp_chinese[BUFSIZ];
	char chinese[] = "ÄãºÃ";
	char* chinese_p = chinese;
	EXPECT_STREQ(PRINTER::PRINT_CHINESE(chinese, temp_chinese), "ÄãºÃ") << "ÄãºÃ failed";
}

TEST(footest, data_test_throw) {
	EXPECT_THROW(foo(0, 0), char*);
	EXPECT_THROW(foo(1, 0), char*);
	EXPECT_THROW(foo(5, 0), char*);
	EXPECT_THROW(foo(10, 0), char*);
	EXPECT_THROW(foo(0, 1), char*);
	EXPECT_THROW(foo(0, 5), char*);
	EXPECT_THROW(foo(0, 10), char*);
	EXPECT_ANY_THROW(foo(0, 0));
	EXPECT_ANY_THROW(foo(1, 0));
	EXPECT_ANY_THROW(foo(5, 0));
	EXPECT_ANY_THROW(foo(10, 0));
	EXPECT_ANY_THROW(foo(0, 1));
	EXPECT_ANY_THROW(foo(0, 5));
	EXPECT_ANY_THROW(foo(0, 10));
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	testing::AddGlobalTestEnvironment(new CGlobalEnvironment());
	testing::FLAGS_gtest_filter = "*";
	RUN_ALL_TESTS();
	system("PAUSE"); 

	return 0;
}

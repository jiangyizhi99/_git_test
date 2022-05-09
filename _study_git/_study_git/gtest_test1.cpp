#include <gtest/gtest.h>
#include <stdlib.h> 
#include <stdio.h>

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

int main_(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	testing::AddGlobalTestEnvironment(new CGlobalEnvironment());
	testing::FLAGS_gtest_filter = "*";
	RUN_ALL_TESTS();
	system("PAUSE"); 

	return 0;
}

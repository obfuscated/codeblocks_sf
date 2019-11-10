#include <UnitTest++.h>
#include <TestReporterStdout.h>

struct RunAll
{
    bool operator()(UnitTest::Test const* /*test*/) const { return true; }
};

int main()
{
//    return UnitTest::RunAllTests();
 	UnitTest::TestReporterStdout reporter;
	UnitTest::TestRunner runner(reporter);
	return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, RunAll(), 1000);
}

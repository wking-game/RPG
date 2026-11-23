#include "GamerGameState.h"

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

// Run with: tools/test_data.ps1 -Filter Gamer.Build

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGamerDemolishRefundTest, "Gamer.Build.DemolishRefund",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FGamerDemolishRefundTest::RunTest(const FString& Parameters)
{
	// Unfinished plans always give back everything, whatever the option says.
	for (const EGamerDemolish Mode : { EGamerDemolish::FullRefund, EGamerDemolish::HalfRefund, EGamerDemolish::PartialRefund, EGamerDemolish::Disabled })
	{
		TestEqual(TEXT("unfinished: everything"), DemolishRefundCount(7, Mode, false), 7);
	}
	// Finished pieces: full, about 80 % (nearest piece), half (rounded down, as before).
	TestEqual(TEXT("full"), DemolishRefundCount(5, EGamerDemolish::FullRefund, true), 5);
	TestEqual(TEXT("half of 5"), DemolishRefundCount(5, EGamerDemolish::HalfRefund, true), 2);
	TestEqual(TEXT("80 % of 1"), DemolishRefundCount(1, EGamerDemolish::PartialRefund, true), 1);
	TestEqual(TEXT("80 % of 2"), DemolishRefundCount(2, EGamerDemolish::PartialRefund, true), 2);
	TestEqual(TEXT("80 % of 3"), DemolishRefundCount(3, EGamerDemolish::PartialRefund, true), 2);
	TestEqual(TEXT("80 % of 5"), DemolishRefundCount(5, EGamerDemolish::PartialRefund, true), 4);
	TestEqual(TEXT("80 % of 10"), DemolishRefundCount(10, EGamerDemolish::PartialRefund, true), 8);
	TestEqual(TEXT("80 % of 0"), DemolishRefundCount(0, EGamerDemolish::PartialRefund, true), 0);
	// Never more than spent, never below half for the partial option.
	for (int32 Spent = 0; Spent <= 40; ++Spent)
	{
		const int32 Given = DemolishRefundCount(Spent, EGamerDemolish::PartialRefund, true);
		TestTrue(FString::Printf(TEXT("%d spent: given %d between half and all"), Spent, Given), Given <= Spent && Given >= Spent / 2);
	}
	// A saved world with the old values keeps its meaning (the new value was appended last).
	TestEqual(TEXT("enum value full"), static_cast<int32>(EGamerDemolish::FullRefund), 0);
	TestEqual(TEXT("enum value half"), static_cast<int32>(EGamerDemolish::HalfRefund), 1);
	TestEqual(TEXT("enum value off"), static_cast<int32>(EGamerDemolish::Disabled), 2);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestPKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting {
    TEST_CLASS(TestPKBSort) {
public:
    TEST_METHOD(sortFollowsStarDataStructure_success) {
        TestPKB pkb = TestPKB();
        pkb.storeRelationshipFollowsStar(1, 5);
        pkb.storeRelationshipFollowsStar(1, 4);
        pkb.storeRelationshipFollowsStar(1, 5);

        // ensure state before sorting
        Assert::IsTrue(pkb.getFollowsStarMapKeyLineBefore()[1] == std::vector<LINE_NUM> {5, 4, 5});
        Assert::IsTrue(pkb.getFollowsStarMapKeyLineAfter()[5] == std::vector<LINE_NUM> {1, 1});

        // sorted result
        pkb.sortAllDataStructure();
        Assert::IsTrue(pkb.getFollowsStarMapKeyLineBefore()[1] == std::vector<LINE_NUM> {4, 5});
        Assert::IsTrue(pkb.getFollowsStarMapKeyLineAfter()[5] == std::vector<LINE_NUM> {1});
    }
    };
}   // namespace UnitTesting

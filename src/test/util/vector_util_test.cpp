#include <gtest/gtest.h>
#include "../../main/util/vector_util.h"

TEST(VectorUtil, filter) {
    auto predicate = [](int num) { return num > 5; };

    std::vector<std::pair<std::vector<int>, std::vector<int>>> values{
            {{1,  2,   15, 38, 4, 10}, {15,  38, 10}},
            {{-5, 280, 13, 4,  55},    {280, 13, 55}},
    };

    for (auto value: values) {
        EXPECT_EQ(VectorUtil::filter(value.first, predicate), value.second);
    }
}

TEST(VectorUtil, map) {
    auto predicate = [](int num) { return num + 2; };

    std::vector<std::pair<std::vector<int>, std::vector<int>>> values{
            {{1, 2, 3, 4}, {3, 4, 5, 6}},
            {{0, 1, 2, 3}, {2, 3, 4, 5}},
    };

    for (auto value: values) {
        auto result = VectorUtil::map<int, int>(value.first, predicate);
        EXPECT_EQ(result, value.second);
    }
}

TEST(VectorUtil, indexOf) {
    std::vector v{1, 2, 3, 4, 28, 6, 2};
    ASSERT_EQ(VectorUtil::indexOf(v, 28), 4);
}

TEST(VectorUtil, move) {
    std::vector v{1, 2, 3, 4, 5};

    VectorUtil::move(v, 0, 3);
    EXPECT_EQ(v[3], 1);

    VectorUtil::move(v, 3, 1);
    EXPECT_EQ(v[1], 1);

    VectorUtil::move(v, 1, 4);
    EXPECT_EQ(v[4], 1);
}

TEST(VectorUtil, moveElem) {
    std::vector vector{1, 2, 3, 4, 5};

    VectorUtil::moveElem(vector, 3, 0);
    std::vector expectedVectorValue{3, 2, 1, 4, 5};

    ASSERT_EQ(vector, expectedVectorValue);
}

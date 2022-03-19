#include <gtest/gtest.h>
#include "../string_util.h"

TEST(StringUtil, takeUntil) {
    EXPECT_EQ("hell", takeUntil("hello", 'o'));
    EXPECT_EQ("heli", takeUntil("helicopter", 'c'));
    EXPECT_EQ("abracadabra", takeUntil("abracadabra boom", ' '));
}

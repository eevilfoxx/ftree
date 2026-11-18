#include <gtest/gtest.h>
#include "core/Random.hpp"

TEST(RandomStateTest, RestoresExactNextSequence) {
    ftree::core::Random original(918273);
    (void)original.randomInt(1, 1000);
    (void)original.randomDouble(0.0, 1.0);
    const auto state = original.serializeState();

    ftree::core::Random restored(1);
    restored.restoreState(state);

    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(original.randomInt(-1000, 1000), restored.randomInt(-1000, 1000));
        EXPECT_DOUBLE_EQ(original.randomDouble(-10.0, 10.0),
                         restored.randomDouble(-10.0, 10.0));
    }
}

TEST(RandomStateTest, RejectsMalformedState) {
    ftree::core::Random random(1);
    EXPECT_THROW(random.restoreState("not an mt19937 state"), std::invalid_argument);
}

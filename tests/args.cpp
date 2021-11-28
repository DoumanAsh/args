#include <catch.hpp>
#include <vector>

#include "args.hpp"

constexpr static const char long_flag[] = "flag";
constexpr static const char short_flag[] = "f";

using Flag = args::Option<long_flag, short_flag, args::Required>;
TEST_CASE("it works") {
    args::Args<Flag> args;

    REQUIRE(args.size() == 1);

    const auto& flag = args.get<Flag>();

    REQUIRE(flag.is_missing());

    std::vector<const char*> input = {
        "--flag", "lolka"
    };
    args.parse(input.cbegin(), input.cend());
}

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "kythe",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    visibility = ["//main:__pkg__"],
    deps = [
        "//ast",
        "//ast/treemap",
        "//core",
        "@rapidjson",
    ],
)

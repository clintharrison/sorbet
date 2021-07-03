load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "kythe",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    deps = [
        "//core",
        "//ast",
        "//ast/treemap",
        "@rapidjson",
    ],
    visibility = ["//main:__pkg__"],
)
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "lsif",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    visibility = ["//main:__subpackages__"],
    deps = [
        "@rapidjson",
        "//core",
        "//ast",
        "//ast/treemap",
        "//core/proto",
    ],
)
cc_library(
    name = "vname",
    hdrs = ["VName.h"],
    deps = [
        "//ast",
        "//core",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "json",
    hdrs = ["json.h"],
    deps = [
        ":vname",
        "@com_google_absl//absl/strings",
        "@rapidjson",
    ],
)

cc_library(
    name = "kythe_injector",
    srcs = [
        "injector.cc",
    ],
    linkstatic = select({
        "@com_stripe_ruby_typer//tools/config:linkshared": 0,
        "//conditions:default": 1,
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":json",
        ":vname",
        "@com_stripe_ruby_typer//ast",
        "@com_stripe_ruby_typer//cfg",
        "@com_stripe_ruby_typer//common",
        "@com_stripe_ruby_typer//main/options",
        "@com_stripe_ruby_typer//main/pipeline/semantic_extension:interface",
        "@cxxopts",
    ],
)

cc_binary(
    name = "sorbet",
    srcs = [
        "main.cc",
    ],
    # When changing this, you should also update the REQUIRED_STACK_SIZE variable in common/os/os.cc.
    linkopts = select({
        "//tools/config:darwindbg": [
            "-Wl,-stack_size",
            "-Wl,0x4000000",
        ],
        "//conditions:default": [],
    }),
    linkstatic = select({
        "//tools/config:linkshared": 0,
        "//conditions:default": 1,
    }),
    malloc = select({
        "//main:jemalloc": "@jemalloc//:jemalloc",
        "//conditions:default": "@bazel_tools//tools/cpp:malloc",
    }),
    tags = ["kythe"],
    visibility = ["//visibility:public"],
    deps = [
        ":kythe_injector",
        "//main:realmain",
        "//payload",
    ],
)

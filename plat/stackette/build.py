from build.ab import export
from build.ack import ackcfile
from mach.proto.ncg.build import build_ncg
from plat.build import build_plat_libs
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="stackette")
build_ncg(name="ncg", arch="stackette")

ackcfile(name="boot", srcs=["./boot.s"], plat="stackette")

# The other platform libraries (libem, libc, libsys) come later, with the
# code generator that is able to compile them.
export(
    name="tools",
    items={
        "$(PLATDEP)/stackette/as$(EXT)": ".+as",
        "$(PLATDEP)/stackette/ncg$(EXT)": ".+ncg",
        "$(PLATIND)/descr/stackette": "./descr",
    },
)

export(
    name="all",
    items={
        "$(PLATIND)/stackette/boot.o": ".+boot",
        "$(PLATIND)/stackette/libend.a": "mach/stackette/libend+lib_stackette",
    },
    deps=[".+tools", "util/ack+all"],
)

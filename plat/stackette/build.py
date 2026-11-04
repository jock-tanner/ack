from build.ab import export
from build.ack import ackcfile, exportheaders
from mach.proto.ncg.build import build_ncg
import importlib

build_as = importlib.import_module("mach.proto.as.build").build_as

build_as(name="as", arch="stackette")
build_ncg(name="ncg", arch="stackette")
# Only the C library: the other languages need floating point.
export(
    name="plat_libs",
    items={
        "$(PLATIND)/stackette/libend.a": "mach/stackette/libend+lib_stackette",
        "$(PLATIND)/stackette/libem.a": "mach/stackette/libem+lib_stackette",
    },
    deps=["lang/cem/libcc.ansi+all_stackette"],
)

ackcfile(name="boot", srcs=["./boot.s"], plat="stackette")

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
        "$(PLATIND)/stackette/libsys.a": "./libsys",
    }
    | exportheaders("./include", prefix="$(PLATIND)/stackette/include"),
    deps=[".+tools", ".+plat_libs", "util/ack+all"],
)

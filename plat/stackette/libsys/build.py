from build.ack import ackclibrary

ackclibrary(
    name="libsys",
    plat="stackette",
    srcs=[
        "./brk.c",
        "./close.c",
        "./creat.c",
        "./errno.s",
        "./errnovar.c",
        "./getpid.c",
        "./_hol0.s",
        "./isatty.c",
        "./kill.c",
        "./lseek.c",
        "./open.c",
        "./rawio.c",
        "./read.c",
        "./signal.c",
        "./time.c",
        "./write.c",
        "./libsys.h",
    ],
    deps=["lang/cem/libcc.ansi/headers", "plat/stackette/include"],
)

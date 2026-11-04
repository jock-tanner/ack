from build import PLATS
from build.ack import ackclibrary

for plat in PLATS:
    ackclibrary(
        name=f"lib_{plat}",
        srcs=["./shift.s","./long.s","./switch.s"],
        plat=plat,
    )

#
# This file is the xilinxk-axidma-test recipe.
#

SUMMARY = "Simple xilinxk-axidma-test application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://axidma_transfer.c \
		file://Makefile \
	   file://axidma_ioctl.h \
	   file://conversion.h \
	   file://libaxidma.c \
	   file://libaxidma.h \
	   file://util.c \
	   file://util.h \
	"

S = "${WORKDIR}"

do_compile() {
	     oe_runmake
}

do_install() {
	     install -d ${D}${bindir}
	     install -m 0755 axidmatest ${D}${bindir}
}

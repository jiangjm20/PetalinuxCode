FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " file://bsp.cfg"
SRC_URI:append = " file://test.patch"
KERNEL_FEATURES:append = " bsp.cfg"

#!/bin/sh

petalinux-config --get-hw-description=~/hardware/multiply_example --silentconfig
petalinux-config -c u-boot --silentconfig
petalinux-config -c kernel --silentconfig
petalinux-config -c rootfs --silentconfig
petalinux-build

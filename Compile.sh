#!/bin/sh

cd /home/mingming/final/components/yocto/workspace/sources/linux-xlnx
git diff > ../../../../../project-spec/meta-user/recipes-kernel/linux/linux-xlnx/test.patch
cd /home/mingming/final
petalinux-build 

#!/usr/bin/env bash

set -e
set -x

script_path=$(readlink -f $0)
base_path=`dirname $script_path`

libcec_dir="${base_path}/3rdparty/libcec"
platform_dir="${base_path}/3rdparty/platform"
output_lib_dir="${base_path}/libs"

pi_sysroot="/mnt/pi"
pi_lib_dir="/opt/vc/lib"
pi_include_dir="/opt/vc/include"

compiler_prefix="/opt/arm-sirspuddarch-linux-gnueabihf/bin/arm-sirspuddarch-linux-gnueabihf-"

# libcec cross compile suggestion
#cmake_cmd="cmake -DCMAKE_TOOLCHAIN_FILE=${libcec_dir}/cmake/CrossCompile.cmake \
#      -DXCOMPILE_BASE_PATH=/opt/arm-sirspuddarch-linux-gnueabihf/bin/arm-sirspuddarch-linux-gnueabihf- \
#      -DXCOMPILE_PREFIX="" \
#      -DXCOMPILE_LIB_PATH= \
#      -DRPI_INCLUDE_DIR=${pi_sysroot}/opt/vc/include \
#      -DRPI_LIB_DIR=${pi_sysroot}/opt/vc/lib"

cmake_cmd="cmake \
                -DCMAKE_TOOLCHAIN_FILE=${base_path}/xcompile.cmake \
                -DCMAKE_SYSROOT_INDIRECTION=${pi_sysroot} \
                -DCMAKE_C_COMPILER=${compiler_prefix}gcc \
                -DCMAKE_CXX_COMPILER=${compiler_prefix}g++ \
                -DCMAKE_FIND_ROOT_PATH=${pi_lib_dir} \
                -DCMAKE_INSTALL_PREFIX:PATH=${output_lib_dir} \
                -DRPI_INCLUDE_DIR=${pi_include_dir} \
                -DRPI_LIB_DIR=${pi_lib_dir} \
                "

# --trace \

# Using sysroot breaks the build with: unable to compile simple test
# Appears to be running ?make? under the sysroot
#-DCMAKE_SYSROOT=${pi_sysroot} \

mkdir -p ${output_lib_dir}

mkdir -p ${platform_dir}/build
cd ${platform_dir}/build
$cmake_cmd ..
make
make install

mkdir -p ${libcec_dir}/build
cd ${libcec_dir}/build
$cmake_cmd ..
make
make install

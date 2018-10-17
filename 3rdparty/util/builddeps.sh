#!/usr/bin/env bash

set -e
#set -x

script_path=$(readlink -f $0)
base_path=`dirname $script_path`
thirdparty_dir="$(cd ${base_path}/.. && pwd)"
toplevel_dir=$(cd ${thirdparty_dir}/.. && pwd)

QT_SDK_DIR=~/raspi/qt5

if [[ -z $piver ]] && [[ -n $LOCAL_PI_VER ]]; then
  piver=$LOCAL_PI_VER
fi

if [[ -z $piver ]]; then
  echo "Cant build without an advertized Pi version"
  exit 1
fi

if [[ -n "$piver" ]]; then
  qmake="${QT_SDK_DIR}/bin/qmake"
  compiler_prefix=$(cat ${QT_SDK_DIR}/mkspecs/qdevice.pri | grep CROSS | awk '{ print $3}')
fi

build() {
  local libcec_dir="${thirdparty_dir}/libcec"
  local platform_dir="${thirdparty_dir}/platform"
  local output_lib_dir="${toplevel_dir}/local"

  local pi_sysroot=$(${qmake} -query QT_SYSROOT)
  echo "Building against the following sysroot: $pi_sysroot"
  # revolting but -sysroot does not seem to impact include paths?!
  # ie they are not relative to the sysroot, which they should be
  local pi_lib_dir="${pi_sysroot}/opt/vc/lib"
  local pi_include_dir="${pi_sysroot}/opt/vc/include"

  # libcec cross compile suggestion
  #cmake_cmd="cmake -DCMAKE_TOOLCHAIN_FILE=${libcec_dir}/cmake/CrossCompile.cmake \
  #      -DXCOMPILE_BASE_PATH=/opt/arm-sirspuddarch-linux-gnueabihf/bin/arm-sirspuddarch-linux-gnueabihf- \
  #      -DXCOMPILE_PREFIX="" \
  #      -DXCOMPILE_LIB_PATH= \
  #      -DRPI_INCLUDE_DIR=${pi_sysroot}/opt/vc/include \
  #      -DRPI_LIB_DIR=${pi_sysroot}/opt/vc/lib"

  # Debugging
  #-DCMAKE_RULE_MESSAGES=OFF -DCMAKE_VERBOSE_MAKEFILE=ON \
  #-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
  local cmake_cmd="cmake \
                  -DCMAKE_TOOLCHAIN_FILE=${base_path}/xcompile.cmake \
                  -DCMAKE_SYSROOT_INDIRECTION=${pi_sysroot} \
                  -DCMAKE_C_COMPILER=${compiler_prefix}gcc \
                  -DCMAKE_CXX_COMPILER=${compiler_prefix}g++ \
                  -DCMAKE_FIND_ROOT_PATH=${pi_lib_dir} \
                  -DCMAKE_INSTALL_PREFIX:PATH=${output_lib_dir} \
                  -DRPI_INCLUDE_DIR=${pi_include_dir} \
                  -DRPI_LIB_DIR=${pi_lib_dir} \
                  -DSKIP_PYTHON_WRAPPER=1 \
                  "

  # --trace \

  # Using sysroot breaks the build with: unable to compile simple test
  # Appears to be running ?make? under the sysroot
  #-DCMAKE_SYSROOT=${pi_sysroot} \

  # patch
  sed -i '/check_library_exists(bcm_host/c\set(HAVE_RPI_LIB 1)' ${libcec_dir}/src/libcec/cmake/CheckPlatformSupport.cmake

  # build
  mkdir -p ${output_lib_dir}

  mkdir -p ${platform_dir}/build
  cd ${platform_dir}/build
  $cmake_cmd ..
  make --no-print-directory
  make install

  mkdir -p ${libcec_dir}/build
  cd ${libcec_dir}/build
  $cmake_cmd ..
  make --no-print-directory
  make install
}

init_repos()
{
  git submodule init
  git submodule update
}

init_repos
build

## 编译、链接那些事儿

### 1. LD_LIBRARY_PATH 和 pkg-config 的区别


### 2. 查看 gcc 默认搜索路径

```shell
cpp -v /dev/null -o /dev/null
```

**以上命令在麒麟V10 SP1系统上的输出结果**
```shell
Using built-in specs.
COLLECT_GCC=cpp
Target: aarch64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 9.3.0-10kylin2' --with-bugurl=file:///usr/share/doc/gcc-9/README.Bugs --enable-languages=c,ada,c++,go,d,fortran,objc,obj-c++,gm2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-9 --program-prefix=aarch64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-libquadmath --disable-libquadmath-support --enable-plugin --enable-default-pie --with-system-zlib --with-target-system-zlib=auto --enable-multiarch --enable-fix-cortex-a53-843419 --disable-werror --enable-checking=release --build=aarch64-linux-gnu --host=aarch64-linux-gnu --target=aarch64-linux-gnu
Thread model: posix
gcc version 9.3.0 (Ubuntu 9.3.0-10kylin2)
COLLECT_GCC_OPTIONS='-E' '-v' '-o' '/dev/null' '-mlittle-endian' '-mabi=lp64'
 /usr/lib/gcc/aarch64-linux-gnu/9/cc1 -E -quiet -v -imultiarch aarch64-linux-gnu /dev/null -o /dev/null -mlittle-endian -mabi=lp64 -fasynchronous-unwind-tables -fstack-protector-strong -Wformat -Wformat-security -fstack-clash-protection
ignoring nonexistent directory "/usr/local/include/aarch64-linux-gnu"
ignoring nonexistent directory "/usr/lib/gcc/aarch64-linux-gnu/9/include-fixed"
ignoring nonexistent directory "/usr/lib/gcc/aarch64-linux-gnu/9/../../../../aarch64-linux-gnu/include"
#include "..." search starts here:
#include <...> search starts here:
 /usr/lib/gcc/aarch64-linux-gnu/9/include
 /usr/local/include
 /usr/include/aarch64-linux-gnu
 /usr/include
End of search list.
COMPILER_PATH=/usr/lib/gcc/aarch64-linux-gnu/9/:/usr/lib/gcc/aarch64-linux-gnu/9/:/usr/lib/gcc/aarch64-linux-gnu/:/usr/lib/gcc/aarch64-linux-gnu/9/:/usr/lib/gcc/aarch64-linux-gnu/
LIBRARY_PATH=/usr/lib/gcc/aarch64-linux-gnu/9/:/usr/lib/gcc/aarch64-linux-gnu/9/../../../aarch64-linux-gnu/:/usr/lib/gcc/aarch64-linux-gnu/9/../../../../lib/:/lib/aarch64-linux-gnu/:/lib/../lib/:/usr/lib/aarch64-linux-gnu/:/usr/lib/../lib/:/usr/lib/gcc/aarch64-linux-gnu/9/../../../:/lib/:/usr/lib/
COLLECT_GCC_OPTIONS='-E' '-v' '-o' '/dev/null' '-mlittle-endian' '-mabi=lp64'
```
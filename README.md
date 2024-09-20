# NutShell

## Goal

实现Difftest 与 RTL 解耦版本, 暂不考虑相关优化设计，避免过度设计。

## Run programs by simulation

You can either use our ready-to-run image for simulation or build image yourself.

To use ready-to run image (recommended) :

* Run `make emu` to launch simulation. Default image is linux kernel.
* Run `make IMAGE=yourimage.bin emu` to specify your image file. We have provided some in ./ready-to-run.

To build image yourself:

* Set a new environment variable `NEMU_HOME` to the **absolute path** of the [NEMU project](https://github.com/OpenXiangShan/NEMU).
* Set a new environment variable `NUTSHELL_HOME` to the **absolute path** of the NutShell project.
* Clone the [AM project](https://github.com/OSCPU/nexus-am.git).
* Set a new environment variable `AM_HOME` to the **absolute path** of the AM project.
* Run the application in the AM project by `make ARCH=riscv64-nutshell run`.
```
cd nexus-am/apps/microbench
make ARCH=riscv64-nutshell mainargs=test run
```
# CuberOS

一项开发 OS 的编程挑战。

# 环境

本项目在 Ubuntu 中开发。

请确保你的系统安装了如下软件包：

- make
- qemu-system-x86
- qemu-system-arm
- clang
- lld

或者你可以通过如下指令统一安装：

```bash
sudo apt install make qemu-system-x86 qemu-system-arm ovmf clang lld
```

# 快速开始

在项目根目录运行下面的指令可以编译并在 qemu 中运行 CuberOS:

```bash
make
```

---

This project is licensed under the GPL 3.0 License. See the [LICENSE](/LICENSE) file for details.

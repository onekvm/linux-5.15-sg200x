# SG2002/CV181x 的 Linux 5.15 支持

[English](README.md) | 简体中文

本仓库为 SG2002/CV181x SoC 系列提供 Linux 5.15 平台支持。内核基于上游
`v5.15.216`，从厂商 5.10 BSP 移植所需的架构支持与驱动。

NanoKVM Cube/PCIe 是本次移植的验证平台，但不是唯一的目标硬件。

## 为什么选择 Linux 5.15

Linux 5.15 是从厂商 5.10 BSP 迁移时较为稳妥的 LTS 目标。继续使用 5.x 系列，
可以在获得更新稳定基线的同时，使内核接口与厂商驱动的差异小于更新的 6.x
内核。

## 平台支持

本树包含 SG2002/CV181x 平台所需的架构改动和驱动：

- T-Head C906 页表属性、TLB 处理、缓存维护和 DMA 重映射
- CV181x 时钟与复位控制器、pinctrl MMIO 映射和 SiFive PLIC
- 支持 64-bit ADMA 和 DT 源时钟配置的 SDHCI
- CVITEK DWMAC glue 和 RMII PHY
- CVITEK DMA、ION、efuse、thermal、watchdog、reboot 和 SPACC crypto
- CV181x DWC2/OTG glue 和 `/proc/cviusb/otg_role`
- CVITEK ASoC 源码及其 Kconfig/Makefile 集成
- 验证平台固件所需的 DW APB GPIO 集成

## 参考配置

本仓库不提供适用于所有 SG2002/CV181x 设备的通用 defconfig。板级集成者需要
根据实际硬件选择驱动和平台选项。

NanoKVM Cube/PCIe 验证使用以下配置 fragment：

```text
arch/riscv/configs/sg2002_nanokvm.config
```

它是叠加在厂商 `sg2002_licheervnano_sd` 配置上的 overlay，并非独立 defconfig。
配套的 `arch/riscv/configs/sg2002_nanokvm_kconfig.allow` 记录了与厂商 5.10
配置之间有意保留的差异。

NanoKVM 验证配置中的重要选项包括：

- `CONFIG_LOCALVERSION="-onekvm"`
- `CONFIG_FLATMEM_MANUAL=y`
- `# CONFIG_VMAP_STACK is not set`
- `CONFIG_NO_SFENCE_VMA=y`
- `CONFIG_SCHED_CVITEK=y`
- `CONFIG_RESET_CVITEK=y`
- `CONFIG_SIFIVE_PLIC=y`

使用 SG2002/CV181x 厂商驱动时必须关闭 `CONFIG_VMAP_STACK`。这些驱动使用
DMA，其中部分路径会对内核对象（包括放在内核栈上的对象）调用
`virt_to_phys()`。vmapped stack 没有直接的线性物理映射；开启 VMAP_STACK
可能使驱动向硬件写入无效 DMA 地址。已知受影响的功能包括 SPACC/CryptoDMA
以及 WebRTC SRTP 等工作负载。

## 验证状态

以下功能已在 NanoKVM Cube/PCIe 硬件上验证：

| 类别 | 状态 |
| --- | --- |
| 启动 | Linux `5.15.216-onekvm`、ttyS0 |
| 存储 | MMC 64-bit ADMA |
| 网络 | CVITEK DWMAC 和 RMII PHY |
| 总线 | I2C0/1/3/4/5 |
| 视频 | ION 和匹配的树外 MMF 模块，HDMI 1080p60 |
| USB | DWC2 device mode 和 HID gadget |
| 平台功能 | Thermal、CryptoDMA、OLED、ATX 和 reboot |

## 集成说明

- 树外模块必须针对本内核重新构建。为厂商 5.10 内核构建的模块与 Linux 5.15
  不兼容。
- 树内 efuse 驱动已经提供 `cvi_efuse_read_buf`；外部模块不得重复提供 stub
  或导出。
- NanoKVM 验证配置关闭 HDMI 音频和 AUD pinmux 修改，但其他板卡仍可使用
  CVITEK ASoC 源码。
- 厂商 framebuffer 和 CIF 驱动因缺少源码而未包含。
- 本树不包含 U-Boot、固件镜像、用户态或树外多媒体模块。

## 基线版本

| 项目 | 值 |
| --- | --- |
| 上游标签 | `v5.15.216` |
| 上游提交 | `0eb903b5b519d037c723d47a58c5d4f20eed7342` |
| 内核版本 | `5.15.216` |
| NanoKVM 验证版本 | `5.15.216-onekvm` |

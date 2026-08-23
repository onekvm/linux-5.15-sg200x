# Linux 5.15 support for SG2002/CV181x

English | [简体中文](README-CN.md)

This repository provides Linux 5.15 platform support for the SG2002/CV181x
SoC family. It is based on upstream Linux `v5.15.216` and ports the required
architecture support and drivers from the vendor 5.10 BSP.

NanoKVM Cube/PCIe hardware is used as the validation platform for this port; it
is not the only intended target.

## Why Linux 5.15

Linux 5.15 is a conservative LTS migration target from the vendor 5.10 BSP.
Staying within the 5.x series keeps the kernel interfaces closer to the vendor
drivers than newer 6.x kernels while providing a newer stable base.

## Platform support

The tree includes the architecture changes and drivers required by the
SG2002/CV181x platform:

- T-Head C906 page-table attributes, TLB handling, cache maintenance and DMA
  remapping
- CV181x clock and reset controllers, pinctrl MMIO mapping and SiFive PLIC
- SDHCI with 64-bit ADMA and DT-provided source clock handling
- CVITEK DWMAC glue and RMII PHY support
- CVITEK DMA, ION, efuse, thermal, watchdog, reboot and SPACC crypto support
- CV181x DWC2/OTG glue and `/proc/cviusb/otg_role`
- CVITEK ASoC sources and their Kconfig/Makefile integration
- DW APB GPIO integration used by the tested platform firmware

## Reference configuration

This repository does not provide a universal SG2002/CV181x defconfig. Board
integrators must select the drivers and platform options required by their
hardware.

The configuration fragment used for NanoKVM Cube/PCIe validation is:

```text
arch/riscv/configs/sg2002_nanokvm.config
```

It is an overlay for the vendor `sg2002_licheervnano_sd` configuration, not a
standalone defconfig. The accompanying
`arch/riscv/configs/sg2002_nanokvm_kconfig.allow` file documents intentional
differences from the vendor 5.10 configuration.

Important settings in the NanoKVM validation fragment include:

- `CONFIG_LOCALVERSION="-onekvm"`
- `CONFIG_FLATMEM_MANUAL=y`
- `# CONFIG_VMAP_STACK is not set`
- `CONFIG_NO_SFENCE_VMA=y`
- `CONFIG_SCHED_CVITEK=y`
- `CONFIG_RESET_CVITEK=y`
- `CONFIG_SIFIVE_PLIC=y`

`CONFIG_VMAP_STACK` must be disabled when using the SG2002/CV181x vendor
drivers. These drivers use DMA and some paths call `virt_to_phys()` on kernel
objects, including objects placed on kernel stacks. A vmapped stack has no
direct linear physical mapping, so enabling VMAP_STACK can program invalid DMA
addresses. This is known to break SPACC/CryptoDMA and workloads such as WebRTC
SRTP.

## Validation status

The following functionality has been verified on NanoKVM Cube/PCIe hardware:

| Area | Status |
| --- | --- |
| Boot | Linux `5.15.216-onekvm`, ttyS0 |
| Storage | MMC with 64-bit ADMA |
| Network | CVITEK DWMAC and RMII PHY |
| Buses | I2C0/1/3/4/5 |
| Video | ION and matching out-of-tree MMF modules at HDMI 1080p60 |
| USB | DWC2 device mode and HID gadget |
| Platform | Thermal, CryptoDMA, OLED, ATX and reboot |

## Integration notes

- Out-of-tree modules must be rebuilt against this kernel. Modules built for a
  vendor 5.10 kernel are not compatible with Linux 5.15.
- The in-tree efuse driver provides `cvi_efuse_read_buf`; external modules must
  not provide a duplicate stub or export.
- The NanoKVM validation configuration disables HDMI audio and AUD pinmux
  changes, but the CVITEK ASoC sources remain available for other boards.
- Vendor framebuffer and CIF drivers are not included because their sources
  are unavailable.
- The tree does not include U-Boot, firmware images, userspace or out-of-tree
  multimedia modules.

## Base version

| Item | Value |
| --- | --- |
| Upstream tag | `v5.15.216` |
| Upstream commit | `0eb903b5b519d037c723d47a58c5d4f20eed7342` |
| Kernel version | `5.15.216` |
| NanoKVM validation release | `5.15.216-onekvm` |

# Borzx-Docker Kernel for Infinix Note 10 Pro (`X695D`)

Custom Linux `4.14.186` kernel with full Docker/container support and `AnyKernel3` packaging for the **Infinix Note 10 Pro (`X695D` / `MT6785`)**, brought up cleanly on top of `MiCode/begonia-r-oss` without OEM kernel source from Transsion.

---

## Device Info

- **Device**: Infinix Note 10 Pro (`Infinix-X695D` / `X695D-OP`, board `CY-X695-H854-D`)
- **SoC**: MediaTek Helio G95 (`mt6785`)
- **Kernel**: Linux `4.14.186-Borzx-Docker`
- **Defconfig**: `arch/arm64/configs/x695d_docker_defconfig`
- **Partition Layout**: A/B (`IS_SLOT_DEVICE=1`, `boot_a` / `boot_b`)

---

## Hardware & Feature Status

| Component | Status | Notes |
|---|---|---|
| Display (`NT36672C` C-PHY 90Hz) | Working | Awake + sleep/wake (`FBIOBLANK` non-blocking async notifier) |
| Touch (`NVT-ts` / `NT36672C` SPI) | Working | Loads `/vendor/firmware/novatek_ts_fw.bin` (`PID=6244`) |
| Battery / Fuel Gauge | Working | `FG_DAEMON_CMD_FROM_USER_NUMBER=88` aligned with stock `fuelgauged` |
| Wi-Fi (2.4 GHz & 5 GHz) | Working | Stock `/vendor/lib/modules/` (`wmt_drv.ko`, `wlan_drv_gen4m.ko`) |
| Bluetooth | Working | Stock `/vendor/lib/modules/bt_drv.ko` |
| Internal Speakers (`tran_audio`) | Working | Clean-room `extamp_gpio` / `dual_speaker_gpio` 1-wire PA control |
| Sensors & Vibrator | Working | 19 hardware sensors (`lsm6dsm`, `akm09918`, `stk33562`, etc.) |
| LTE Modem / RIL | Ready | Baseband `MOLY.LR13.R1.MP.V93.P44` (`ready`, RF signal verified) |
| Docker / Containers | Working | Namespaces (`user`, `pid`, `ipc`, `uts`, `net`, `cgroup`), cgroups, OverlayFS, Bridge/NAT, IPVS, VXLAN, MACVLAN |
| Cameras & Torch | Not working | Missing X695D `imgsensor` driver sources (`s5kgw1sd`, `gc8034main`, `hi1631q`, `s5k3p9sp`, `ov02b1b`) |
| Fingerprint (`silead`) | Not working | `vendor.silead.hardware.fingerprintext@1.0-service` fails to open HAL |

---

## Docker / Container Kernel Features

Enabled in `arch/arm64/configs/x695d_docker_defconfig`:
- **Namespaces & IPC**: `CONFIG_NAMESPACES`, `CONFIG_USER_NS`, `CONFIG_PID_NS`, `CONFIG_IPC_NS`, `CONFIG_UTS_NS`, `CONFIG_NET_NS`, `CONFIG_POSIX_MQUEUE`, `CONFIG_SYSVIPC`
- **Cgroups**: `CONFIG_CGROUPS`, `CONFIG_CGROUP_CPUACCT`, `CONFIG_CGROUP_DEVICE`, `CONFIG_CGROUP_FREEZER`, `CONFIG_CGROUP_PIDS`, `CONFIG_CGROUP_SCHED`, `CONFIG_CFS_BANDWIDTH`, `CONFIG_MEMCG`, `CONFIG_MEMCG_SWAP`, `CONFIG_BLK_CGROUP`, `CONFIG_CPUSETS`
- **Networking & Storage**: `CONFIG_OVERLAY_FS`, `CONFIG_VETH`, `CONFIG_BRIDGE`, `CONFIG_STP`, `CONFIG_BRIDGE_NETFILTER`, `CONFIG_IP_VS` (`IPV6`, `TCP`, `UDP`, `RR`, `WRR`, `LC`), `CONFIG_IP6_NF_NAT`, `CONFIG_IP6_NF_TARGET_MASQUERADE`, `CONFIG_NETFILTER_XT_MATCH_ADDRTYPE`, `CONFIG_NETFILTER_XT_MATCH_IPVS`, `CONFIG_VXLAN`, `CONFIG_MACVLAN`

### Running Docker in Termux (Root / Magisk required)
```sh
pkg update && pkg install -y root-repo && pkg install -y docker tsu
sudo dockerd
```
In another Termux session:
```sh
sudo docker run --rm hello-world
```

---

## Building from Source

Requires AOSP `clang-r383902` and `aarch64-linux-android-4.9` binutils:

```bash
export PATH="/path/to/clang-r383902/bin:/path/to/aarch64-linux-android-4.9/bin:$PATH"
make O=out ARCH=arm64 x695d_docker_defconfig
make -j$(nproc) O=out ARCH=arm64 CC=clang CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=aarch64-linux-android- LD=ld.lld Image.gz
```

To package an `AnyKernel3` zip:
```bash
cp out/arch/arm64/boot/Image.gz AnyKernel3/Image.gz
cd AnyKernel3 && zip -r9 ../Borzx-Docker-X695D-AnyKernel3.zip . -x "*.git*"
```

---

## Flashing

- **AnyKernel3 Zip** (in TWRP / Kernel Flasher over an existing/Magisk-patched boot image):
  Flash `Borzx-Docker-X695D-AnyKernel3.zip` (`split_boot` / `flash_boot` preserves your existing ramdisk & Magisk).
- **Boot Image (`fastboot`)**:
  ```sh
  fastboot flash boot_a boot.img
  fastboot reboot
  ```
  *(To keep root, patch `boot.img` in the Magisk app first via **Install -> Select and Patch a File**.)*

---

## Credits

- **Surakiat Sittisomboon / Borzx** ([@lollipop4545](https://github.com/lollipop4545)) — Project owner, testing, and `Borzx-Docker` series
- **Antigravity AI** — Clean-room `X695D` kernel bring-up, debugging, Docker config integration, and `AnyKernel3` adaptation
- **Xiaomi / MiCode** — [`MiCode/begonia-r-oss`](https://github.com/MiCode/Xiaomi_Kernel_OpenSource/tree/begonia-r-oss) base `MT6785` `4.14.186` kernel tree
- **osm0sis** — [`AnyKernel3`](https://github.com/osm0sis/AnyKernel3)

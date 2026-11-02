# X695D Power-Minimal Diagnostic Architecture

## 1. Executive Summary

This document establishes the power-minimal hardware and driver architecture for the **Infinix Note 10 Pro (X695D)**.

The design decouples safety-critical voltage and thermal telemetry from active charging control loops. Because an end-to-end software clamp to 5V/500mA cannot be validated from local kernel source alone without hardware bus verification, **all external charging power input is strictly prohibited for any future physical test**. The system must operate exclusively on battery reserve.

---

## 2. Source-Level Subsystem Dependency Map

```
                     [ Transsion BL-49FX Battery ]
                                   |
           +-----------------------+-----------------------+
           |                                               |
           v                                               v
[ MediaTek MT6359 PMIC ]                        [ MediaTek MT6360 Sub-PMIC ]
- CPU / Memory Rails (VCORE, VPROC, VIO18)       - Auxiliary LDOs (vfp, vtp, vmch)
- System Watchdog & Power-Hold                   - ADC (VBAT, TSBAT telemetry)
- Hardware UVLO / OVP Protections                - TCPC (USB Type-C CC line detection)
- STATUS: ACTIVE / ESSENTIAL                     - Charger Control: QUالان ESCENT / DISABLED
                                                           |
                                                           v
                                            [ Richtek RT9759 Dual Pump ]
                                            - 33W Direct Fast Charging
                                            - STATUS: UNMANAGED / DISABLED
```

### 2.1 Detailed Subsystem Roles
1. **MediaTek MT6359 Core**:
   - Manages fundamental system power rails.
   - Built-in drivers: `drivers/regulator/mt6359-regulator.c`, `drivers/mfd/mt6359-core.c`.
   - Hardware UVLO (Under-Voltage Lockout) protects the battery cell from deep over-discharge.
2. **MediaTek MT6360 Sub-PMIC Core & LDOs**:
   - Provides LDO voltages for touchscreen (`ldo2`), sensors (`ldo5`), and SIM.
   - Built-in drivers: `drivers/misc/mediatek/pmic/mt6360/pmu/mt6360_pmu.c`.
3. **MediaTek MT6360 Switching Charger**:
   - Begonia charging state machine excised via `CONFIG_MTK_CHARGER_BEGONIA=n`.
   - Operates in unmanaged power-on reset (POR) baseline state.
4. **Richtek RT9759 Dual Charge Pump**:
   - Nodes `rt9759_master@66` and `rt9759_slave@65` on `i2c6` remain unbound.
   - Hardware remains in high-impedance / quiescent state without clocking or phase gating.

---

## 3. Telemetry & Protection Allocations

### 3.1 What Must Remain Available
- **Battery Voltage Monitoring (VBAT ADC)**: Real-time cell voltage reporting to detect brownouts.
- **Battery Temperature Monitoring (TSBAT ADC)**: Thermal sensing via MT6359/MT6360 ADC.
- **Hardware Emergency Shutdown**: Autonomous silicon-level shutdown if battery voltage drops below UVLO threshold (typically 3.4V) or exceeds thermal ceiling (typically 60°C).
- **USB Cable Plug / Orientation Detection**: MT6360 TCPC CC line detection required for USB gadget data enumeration.
- **USB Gadget Data Path**: Peripheral USB communication for diagnostic logging over ADB/Serial.

### 3.2 What Must Remain Unavailable
- **33W Charge Pump Activation**: No phase switching, no divider clocking, no dual-phase handshake.
- **Power Delivery (PD) High-Voltage Negotiation**: No request for 9V, 11V, or 12V contracts over USB Type-C.
- **Vendor Fast-Charging Algorithms**: All background charging threads and thermal override scripts disabled.
- **Unverified Current Limits**: No software alteration of input current limit registers without bus verification.

---

## 4. Policy on External Power Input

> [!CAUTION]
> **PROHIBITION OF EXTERNAL CHARGING POWER**
> In the absence of bit-for-bit verified Transsion charging control drivers, claiming a safe 5V/500mA software clamp is unproven and unsafe.
>
> Therefore, **all external charging power input is strictly prohibited during any future physical testing**.
> If physical diagnostic boot is ever authorized, the phone must be booted strictly on existing internal battery power. Connecting a wall charger or high-power USB source is prohibited.

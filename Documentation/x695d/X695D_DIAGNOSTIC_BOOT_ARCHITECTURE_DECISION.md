# X695D Diagnostic Boot Architecture Decision: Minimal Initramfs Selection

## 1. Executive Summary & Production Context

The physical **Infinix Note 10 Pro (X695D)** is an in-service production device hosting active user services. Any accidental modification, filesystem corruption, or recovery-triggered wiping of the userdata partition is completely unacceptable.

This decision document formally evaluates two competing diagnostic architectures:
- **Approach A**: Booting with the Stock Android Vendor Ramdisk.
- **Approach B**: Booting with a Dedicated Minimal Diagnostic Initramfs.

**Formal Decision**: **`APPROACH B IS SELECTED AS THE MANDATORY FUTURE DIAGNOSTIC ARCHITECTURE`**.

---

## 2. Comparative Evaluation Matrix

| Architectural Criterion | Approach A: Stock Android Ramdisk | Approach B: Minimal Diagnostic Initramfs | Winner / Justification |
| :--- | :--- | :--- | :---: |
| **Userdata Mount Requirement** | **MANDATORY READ-WRITE**. Android `init` executes `mount_all --late` on `fstab.mt6785`, attempting read-write mount with quota and format flags. | **ZERO WRITES**. `/data` is NEVER mounted or touched. Entire userland runs in RAM (`tmpfs`). | **APPROACH B** (Zero risk to production data) |
| **Recovery Mount Failure Behavior**| **DESTRUCTIVE HAZARD**. If mount fails or encryption keys fail to unwrap, Android redirects to recovery with `prompt_and_wipe_data`. | **NONE**. No recovery partitions or wiping scripts exist in the diagnostic initramfs. | **APPROACH B** (Failsafe isolation) |
| **Health & Charger Dependencies** | **HIGH CHURN**. Android launches `healthd` and charging daemons that panic or restart continuously when RT9759 is unmanaged. | **ZERO**. No battery daemons launched; system runs statically on battery reserve. | **APPROACH B** (No daemon loop panics) |
| **Vendor Kernel Module Dependencies**| **FATAL ABI CONFLICT**. Ramdisk attempts to load 10 vendor `.ko` modules which fail 100% due to `module_layout` CRC mismatch. | **ZERO**. No loadable modules required; all diagnostic drivers built-in. | **APPROACH B** (Immune to ABI CRC gap) |
| **Display Subsystem Dependency** | **HIGH**. SurfaceFlinger and BootAnimation crash continuously on missing C-PHY panel driver, blinding the operator. | **NONE**. Output routed immediately over USB serial (`/dev/ttyGS0`) and UART earlycon. | **APPROACH B** (Full visibility without display) |
| **Logging & Diagnostic Visibility** | **DELAYED / FRAGILE**. Stock ADB only starts after late-stage init; unavailable if boot halts early. | **IMMEDIATE**. Shell and logging available at time 1.0s directly from RAM. | **APPROACH B** (Early, robust diagnostics) |
| **Image Size & Build Footprint** | Large (Stock ramdisk ~15-20 MB). | Minimal (< 3 MB compressed). | **APPROACH B** |

---

## 3. Engineering Justification

Under the Production Device Protection Policy:
> "Prioritize zero userdata writes over reaching the Android UI."

Attempting to reach the full Android UI using the stock ramdisk creates an unacceptable chain of failure modes:
1. Display failure causes a black screen.
2. Vendor module loader failure breaks Wi-Fi and audio.
3. Health daemon failure triggers watchdog restarts.
4. Any flaw in userdata mounting risks triggering Android Recovery's data wipe routine.

In contrast, **Approach B (Minimal Diagnostic Initramfs)** guarantees:
- The `/data` block device is never mounted or altered.
- All hardware inspection (UFS health, memory validation, battery voltage ADC, kernel dmesg) occurs safely in RAM.
- Diagnostic logs are streamed immediately over standard USB-C serial.

---

## 4. Phase 2D.1 Scope Boundary

In accordance with Phase 2D.1 instructions, **the diagnostic initramfs will NOT be created in this phase**. This decision formally establishes the architectural requirement for future implementation phases.

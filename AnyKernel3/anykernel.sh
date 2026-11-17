### AnyKernel3 Ramdisk Mod Script
## osm0sis @ xda-developers
## Modified for Borzx-Docker Kernel (Infinix Note 10 Pro / X695D)

### AnyKernel setup
# global properties
properties() { '
kernel.string=Borzx-Docker for Infinix Note 10 Pro (X695D)
do.devicecheck=1
do.modules=0
do.systemless=1
do.cleanup=1
do.cleanuponabort=0
device.name1=Infinix-X695D
device.name2=X695D
device.name3=x695d
device.name4=X695D-OP
device.name5=
supported.versions=11.0-14.0
supported.patchlevels=
supported.vendorpatchlevels=
'; } # end properties


### AnyKernel install
## boot files attributes
boot_attributes() {
set_perm_recursive 0 0 755 644 $RAMDISK/*;
set_perm_recursive 0 0 750 750 $RAMDISK/init* $RAMDISK/sbin;
} # end attributes

# boot shell variables
BLOCK=/dev/block/by-name/boot;
IS_SLOT_DEVICE=1;
RAMDISK_COMPRESSION=auto;
PATCH_VBMETA_FLAG=auto;

# import functions/variables and setup patching - see for reference (DO NOT REMOVE)
. tools/ak3-core.sh;

ui_print "-----------------------------------------"
ui_print " Borzx-Docker Kernel for Infinix X695D"
ui_print " Device    : Infinix Note 10 Pro (mt6785)"
ui_print " GitHub    : github.com/lollipop4545"
ui_print "-----------------------------------------"
ui_print " "

# boot install
ui_print "-> Unpacking boot image..."
split_boot;

ui_print "-> Flashing Borzx-Docker kernel..."
flash_boot;
## end boot install

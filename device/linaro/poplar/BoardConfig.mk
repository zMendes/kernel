


BOARD_FLASH_BLOCK_SIZE := 512

BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1610612224
BOARD_SYSTEMIMAGE_FILE_SYSTEM_TYPE := ext4

BOARD_VENDORIMAGE_PARTITION_SIZE := 268434944
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4

BOARD_USERDATAIMAGE_PARTITION_SIZE := 5456789504
BOARD_USERDATAIMAGE_FILE_SYSTEM_TYPE := ext4

BOARD_CACHEIMAGE_PARTITION_SIZE := 268434944
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4

TARGET_USERIMAGES_USE_EXT4 := true
TARGET_NO_BOOTLOADER := false
TARGET_BOOTLOADER_IS_2ND = true
TARGET_NO_KERNEL := false
TARGET_NO_RECOVERY := true
TARGET_NO_RADIOIMAGE := true

TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := cortex-a9
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi


TARGET_USES_64_BIT_BINDER := true
TARGET_BOARD_PLATFORM := poplar
TARGET_BOOTLOADER_BOARD_NAME := poplar 

BOARD_KERNEL_CMDLINE := androidboot.hardware=poplar androidboot.selinux=permissive firmware_class.path=/vendor/firmware mmz=ddr,0,0,60M
BOARD_KERNEL_CMDLINE += printk.devkmsg=on
BOARD_KERNEL_CMDLINE += blkdevparts=mmcblk1:4193792@512(loader),40m(boot),1610612224(system),5993660928,268434944@1580m(vendor),268434944@1836m(cache),5364m@2092m(userdata)


#sepolicy
BOARD_SEPOLICY_DIRS := device/linaro/poplar/sepolicy

# HAL's build config
# build config of feature graphics
TARGET_USES_HWC2 := true

# wifi
BOARD_WLAN_DEVICE := rtl
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)

TARGET_COPY_OUT_VENDOR := vendor

# Enable Treble
PRODUCT_FULL_TREBLE_OVERRIDE := true
BOARD_VNDK_VERSION := current
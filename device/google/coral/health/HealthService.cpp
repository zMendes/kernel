/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "android.hardware.health@2.0-service.coral"
#include <android-base/logging.h>

#include <android-base/file.h>
#include <android-base/parseint.h>
#include <android-base/strings.h>
#include <health2/Health.h>
#include <health2/service.h>
#include <healthd/healthd.h>
#include <hidl/HidlTransportSupport.h>
#include <pixelhealth/BatteryMetricsLogger.h>
#include "pixelhealth/BatteryThermalControl.h"
#include <pixelhealth/CycleCountBackupRestore.h>
#include <pixelhealth/DeviceHealth.h>
#include <pixelhealth/LowBatteryShutdownMetrics.h>

#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

namespace {

using android::hardware::health::V2_0::DiskStats;
using android::hardware::health::V2_0::StorageAttribute;
using android::hardware::health::V2_0::StorageInfo;
using hardware::google::pixel::health::BatteryMetricsLogger;
using hardware::google::pixel::health::BatteryThermalControl;
using hardware::google::pixel::health::CycleCountBackupRestore;
using hardware::google::pixel::health::DeviceHealth;
using hardware::google::pixel::health::LowBatteryShutdownMetrics;

#define FG_DIR "/sys/class/power_supply/battery"
constexpr char kBatteryResistance[] {FG_DIR "/resistance"};
constexpr char kBatteryAvgResistance[] {FG_DIR "/resistance_avg"};
constexpr char kBatteryOCV[] {FG_DIR "/voltage_ocv"};
constexpr char kVoltageAvg[] {FG_DIR "/voltage_avg"};
constexpr char kCycleCountsBins[] {FG_DIR "/cycle_counts"};
constexpr char kGaugeSerial[] {FG_DIR "/serial_number"};

static BatteryThermalControl battThermalControl(
    "sys/devices/virtual/thermal/tz-by-name/soc/mode");
static BatteryMetricsLogger battMetricsLogger(
    kBatteryResistance, kBatteryOCV, kBatteryAvgResistance);
static LowBatteryShutdownMetrics shutdownMetrics(kVoltageAvg);
static CycleCountBackupRestore ccBackupRestore(
    10, kCycleCountsBins, "/persist/battery/cycle_counts", kGaugeSerial);
static DeviceHealth deviceHealth;

#define UFS_DIR "/sys/devices/platform/soc/1d84000.ufshc"
constexpr char kUfsHealthEol[]{UFS_DIR "/health/eol"};
constexpr char kUfsHealthLifetimeA[]{UFS_DIR "/health/lifetimeA"};
constexpr char kUfsHealthLifetimeB[]{UFS_DIR "/health/lifetimeB"};
constexpr char kUfsVersion[]{UFS_DIR "/version"};
constexpr char kDiskStatsFile[]{"/sys/block/sda/stat"};
constexpr char kUFSName[]{"UFS0"};

constexpr char kTCPMPSYName[]{"tcpm-source-psy-usbpd0"};

std::ifstream assert_open(const std::string &path) {
  std::ifstream stream(path);
  if (!stream.is_open()) {
    LOG(FATAL) << "Cannot read " << path;
  }
  return stream;
}

template <typename T>
void read_value_from_file(const std::string &path, T *field) {
  auto stream = assert_open(path);
  stream.unsetf(std::ios_base::basefield);
  stream >> *field;
}

void read_ufs_version(StorageInfo *info) {
  uint64_t value;
  read_value_from_file(kUfsVersion, &value);
  std::stringstream ss;
  ss << "ufs " << std::hex << value;
  info->version = ss.str();
}

void fill_ufs_storage_attribute(StorageAttribute *attr) {
  attr->isInternal = true;
  attr->isBootDevice = true;
  attr->name = kUFSName;
}

}  // anonymous namespace

void healthd_board_init(struct healthd_config *hc) {
  hc->ignorePowerSupplyNames.push_back(android::String8(kTCPMPSYName));
  ccBackupRestore.Restore();
}

int healthd_board_battery_update(struct android::BatteryProperties *props) {
  deviceHealth.update(props);
  battThermalControl.updateThermalState(props);
  battMetricsLogger.logBatteryProperties(props);
  shutdownMetrics.logShutdownVoltage(props);
  ccBackupRestore.Backup(props->batteryLevel);
  return 0;
}

void get_storage_info(std::vector<StorageInfo> &vec_storage_info) {
  vec_storage_info.resize(1);
  StorageInfo *storage_info = &vec_storage_info[0];
  fill_ufs_storage_attribute(&storage_info->attr);

  read_ufs_version(storage_info);
  read_value_from_file(kUfsHealthEol, &storage_info->eol);
  read_value_from_file(kUfsHealthLifetimeA, &storage_info->lifetimeA);
  read_value_from_file(kUfsHealthLifetimeB, &storage_info->lifetimeB);
  return;
}

void get_disk_stats(std::vector<DiskStats> &vec_stats) {
  vec_stats.resize(1);
  DiskStats *stats = &vec_stats[0];
  fill_ufs_storage_attribute(&stats->attr);

  auto stream = assert_open(kDiskStatsFile);
  // Regular diskstats entries
  stream >> stats->reads >> stats->readMerges >> stats->readSectors >>
      stats->readTicks >> stats->writes >> stats->writeMerges >>
      stats->writeSectors >> stats->writeTicks >> stats->ioInFlight >>
      stats->ioTicks >> stats->ioInQueue;
  return;
}

int main(void) { return health_service_main(); }

/*
 * Copyright (C) 2019 The Android Open Source Project
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

#define ATRACE_TAG (ATRACE_TAG_VIBRATOR | ATRACE_TAG_HAL)
#define LOG_TAG "android.hardware.vibrator@1.3-service.coral"

#include "Hardware.h"

#include <log/log.h>
#include <utils/Trace.h>

#include <iostream>

namespace android {
namespace hardware {
namespace vibrator {
namespace V1_3 {
namespace implementation {

template <typename T>
static void fileFromEnv(const char *env, T *outStream, std::string *outName = nullptr) {
    auto file = std::getenv(env);
    auto mode = std::is_base_of_v<std::ostream, T> ? std::ios_base::out : std::ios_base::in;

    if (file == nullptr) {
        ALOGE("Failed get env %s", env);
        return;
    }

    if (outName != nullptr) {
        *outName = std::string(file);
    }

    // Force 'in' mode to prevent file creation
    outStream->open(file, mode | std::ios_base::in);
    if (!*outStream) {
        ALOGE("Failed to open %s:%s (%d): %s", env, file, errno, strerror(errno));
    }
}

static auto pathsFromEnv(const char *env) {
    std::map<std::string, std::ifstream> ret;
    auto value = std::getenv(env);

    if (value == nullptr) {
        return ret;
    }

    std::istringstream paths{value};
    std::string path;

    while (paths >> path) {
        ret[path].open(path);
    }

    return ret;
}

static std::string trim(const std::string &str, const std::string &whitespace = " \t") {
    const auto str_begin = str.find_first_not_of(whitespace);
    if (str_begin == std::string::npos) {
        return "";
    }

    const auto str_end = str.find_last_not_of(whitespace);
    const auto str_range = str_end - str_begin + 1;

    return str.substr(str_begin, str_range);
}

template <typename T>
static Enable_If_Iterable<T, true> unpack(std::istream &stream, T *value) {
    for (auto &entry : *value) {
        stream >> entry;
    }
}

template <typename T>
static Enable_If_Iterable<T, false> unpack(std::istream &stream, T *value) {
    stream >> *value;
}

#define RECORD(args...) record(__FUNCTION__, ##args)

HwApi::HwApi() {
    // ostreams below are required
    fileFromEnv("F0_FILEPATH", &mF0, &mNames[&mF0]);
    fileFromEnv("REDC_FILEPATH", &mRedc, &mNames[&mRedc]);
    fileFromEnv("Q_FILEPATH", &mQ, &mNames[&mQ]);
    fileFromEnv("ACTIVATE_PATH", &mActivate, &mNames[&mActivate]);
    fileFromEnv("DURATION_PATH", &mDuration, &mNames[&mDuration]);
    fileFromEnv("STATE_PATH", &mState, &mNames[&mState]);
    fileFromEnv("EFFECT_DURATION_PATH", &mEffectDuration, &mNames[&mEffectDuration]);
    fileFromEnv("EFFECT_INDEX_PATH", &mEffectIndex, &mNames[&mEffectIndex]);
    fileFromEnv("EFFECT_QUEUE_PATH", &mEffectQueue, &mNames[&mEffectQueue]);
    fileFromEnv("EFFECT_SCALE_PATH", &mEffectScale, &mNames[&mEffectScale]);
    fileFromEnv("GLOBAL_SCALE_PATH", &mGlobalScale, &mNames[&mGlobalScale]);
    fileFromEnv("ASP_ENABLE_PATH", &mAspEnable, &mNames[&mAspEnable]);
    fileFromEnv("GPIO_FALL_INDEX", &mGpioFallIndex, &mNames[&mGpioFallIndex]);
    fileFromEnv("GPIO_FALL_SCALE", &mGpioFallScale, &mNames[&mGpioFallScale]);
    fileFromEnv("GPIO_RISE_INDEX", &mGpioRiseIndex, &mNames[&mGpioRiseIndex]);
    fileFromEnv("GPIO_RISE_SCALE", &mGpioRiseScale, &mNames[&mGpioRiseScale]);
}

template <typename T>
bool HwApi::has(T &stream) {
    return !!stream;
}

template <typename T, typename U>
bool HwApi::get(T *value, U &stream) {
    ATRACE_NAME("HwApi::get");
    bool ret;
    stream.seekg(0);
    stream >> *value;
    if (!(ret = !!stream)) {
        ALOGE("Failed to read %s (%d): %s", mNames[&stream].c_str(), errno, strerror(errno));
    }
    stream.clear();
    RECORD(*value, &stream);
    return ret;
}

template <typename T, typename U>
bool HwApi::set(const T &value, U &stream) {
    ATRACE_NAME("HwApi::set");
    bool ret;
    stream << value << std::endl;
    if (!(ret = !!stream)) {
        ALOGE("Failed to write %s (%d): %s", mNames[&stream].c_str(), errno, strerror(errno));
        stream.clear();
    }
    RECORD(value, &stream);
    return ret;
}

template <typename T>
void HwApi::record(const char *func, const T &value, void *stream) {
    mRecords.emplace_back(std::make_unique<Record<T>>(func, value, stream));
    mRecords.erase(mRecords.begin());
}

void HwApi::debug(int fd) {
    dprintf(fd, "Kernel:\n");

    for (auto &entry : pathsFromEnv("HWAPI_DEBUG_PATHS")) {
        auto &path = entry.first;
        auto &stream = entry.second;
        std::string line;

        dprintf(fd, "  %s:\n", path.c_str());
        while (std::getline(stream, line)) {
            dprintf(fd, "    %s\n", line.c_str());
        }
    }

    dprintf(fd, "  Records:\n");
    for (auto &r : mRecords) {
        if (r == nullptr) {
            continue;
        }
        dprintf(fd, "    %s\n", r->toString(mNames).c_str());
    }
}

template <typename T>
std::string HwApi::Record<T>::toString(const NamesMap &names) {
    std::stringstream ret;

    ret << mFunc << " '" << names.at(mStream) << "' = '" << mValue << "'";

    return ret.str();
}

HwCal::HwCal() {
    std::ifstream calfile;

    fileFromEnv("CALIBRATION_FILEPATH", &calfile);

    for (std::string line; std::getline(calfile, line);) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::istringstream is_line(line);
        std::string key, value;
        if (std::getline(is_line, key, ':') && std::getline(is_line, value)) {
            mCalData[trim(key)] = trim(value);
        }
    }
}

template <typename T>
bool HwCal::get(const char *key, T *value) {
    ATRACE_NAME("HwCal::get");
    auto it = mCalData.find(key);
    if (it == mCalData.end()) {
        ALOGE("Missing %s config!", key);
        return false;
    }
    std::stringstream stream{it->second};
    unpack(stream, value);
    if (!stream || !stream.eof()) {
        ALOGE("Invalid %s config!", key);
        return false;
    }
    return true;
}

void HwCal::debug(int fd) {
    std::ifstream stream;
    std::string path;
    std::string line;

    dprintf(fd, "Persist:\n");

    fileFromEnv("CALIBRATION_FILEPATH", &stream, &path);

    dprintf(fd, "  %s:\n", path.c_str());
    while (std::getline(stream, line)) {
        dprintf(fd, "    %s\n", line.c_str());
    }
}

}  // namespace implementation
}  // namespace V1_3
}  // namespace vibrator
}  // namespace hardware
}  // namespace android

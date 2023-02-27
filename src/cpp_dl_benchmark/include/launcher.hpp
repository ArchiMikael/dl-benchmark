// Copyright (C) 2023 KNS Group LLC (YADRO)
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "tensor_buffer.hpp"
#include "utils.hpp"

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

using HighresClock = std::chrono::high_resolution_clock;

struct TensorDescr {
    std::string name;
    std::vector<int> shape;
    std::vector<int> data_shape;
    std::string layout;
    utils::DataPrecision data_precision;

    bool is_image() const;
    bool is_image_info() const;
    bool is_dynamic() const;
    bool has_batch() const;
    bool is_dynamic_batch() const;
    int get_dimension_by_layout(char ch) const;
    int channels() const;
    int width() const;
    int height() const;
    void set_batch(int batch_size);
};

using IOTensorsInfo = std::pair<std::vector<TensorDescr>, std::vector<TensorDescr>>;

class Launcher {
protected:
    int nthreads;

    std::vector<std::vector<TensorBuffer>> tensor_buffers;

    // time stamps for total time measurments;
    HighresClock::time_point total_start_time;
    HighresClock::time_point total_end_time;

    // time stamps for individual inference
    HighresClock::time_point infer_start_time;
    std::vector<double> latencies;

public:
    Launcher(int nthreads) : nthreads(nthreads) {};
    virtual ~Launcher() {}

    virtual void configure_framework(const std::vector<std::string> &args) = 0;
    virtual void log_framework_version() const = 0;

    virtual void read(const std::string &model) = 0;
    virtual void load() = 0;

    virtual void fill_inputs_outputs_info() = 0;
    virtual IOTensorsInfo get_io_tensors_info() const = 0;

    // virtual void set_batch_size(int batch_size) = 0;
    // int get_batch_size() const;
    virtual void prepare_input_tensors(std::vector<std::vector<TensorBuffer>> tensor_buffers) = 0;

    virtual void warmup_inference() = 0;
    virtual int evaluate(int iterations_num, uint64_t time_limit_ns) = 0;

    std::vector<double> get_latencies() const;
    double get_total_time_ms() const;

    void reset_timers();
};

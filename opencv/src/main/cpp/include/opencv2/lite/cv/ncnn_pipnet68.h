//
// Created by DefTruth on 2022/3/20.
//

#ifndef LITE_AI_TOOLKIT_NCNN_CV_NCNN_PIPNET68_H
#define LITE_AI_TOOLKIT_NCNN_CV_NCNN_PIPNET68_H

#include "../ncnn_core.h"

namespace ncnncv
{
  class LITE_EXPORTS NCNNPIPNet68 : public BasicNCNNHandler
  {
  public:
    explicit NCNNPIPNet68(const std::string &_param_path,
                          const std::string &_bin_path,
                          unsigned int _num_threads = 1);

    ~NCNNPIPNet68() override = default;

  private:
    // hardcode input size
    static constexpr const unsigned int input_height = 256;
    static constexpr const unsigned int input_width = 256;
    const float mean_vals[3] = {0.485f * 255.f, 0.456f * 255.f, 0.406f * 255.f};
    const float norm_vals[3] = {(1.0f / 0.229f) * (1.0 / 255.f),
                                (1.0f / 0.224f) * (1.0 / 255.f),
                                (1.0f / 0.225f) * (1.0 / 255.f)};
    static constexpr const unsigned int num_nb = 10;
    static constexpr const unsigned int num_lms = 68;
    static constexpr const unsigned int max_len = 22;
    static constexpr const unsigned int net_stride = 32;
    // hardcode grid size
    static constexpr const unsigned int grid_h = 8;
    static constexpr const unsigned int grid_w = 8;
    static constexpr const unsigned int grid_length = 8 * 8; // 64

  private:
    void transform(const cv::Mat &mat, ncnn::Mat &in) override;

    void generate_landmarks(types::Landmarks &landmarks,
                            ncnn::Extractor &extractor,
                            float img_height, float img_width);

  public:
    void detect(const cv::Mat &mat, types::Landmarks &landmarks);

  private:
    const unsigned int reverse_index1[68 * 22] = {
        1, 2, 17, 18, 36, 1, 2, 17, 18, 36, 1, 2, 17, 18, 36, 1, 2, 17, 18, 36, 1, 2, 0, 2, 3, 17, 0, 2, 3, 17, 0, 2, 3, 17, 0, 2, 3, 17, 0,
        2, 3, 17, 0, 2, 0, 1, 3, 4, 0, 1, 3, 4, 0, 1, 3, 4, 0, 1, 3, 4, 0, 1, 3, 4, 0, 1, 1, 2, 4, 5, 1, 2, 4, 5, 1, 2, 4, 5, 1, 2, 4, 5, 1,
        2, 4, 5, 1, 2, 2, 3, 5, 6, 2, 3, 5, 6, 2, 3, 5, 6, 2, 3, 5, 6, 2, 3, 5, 6, 2, 3, 3, 4, 6, 7, 3, 4, 6, 7, 3, 4, 6, 7, 3, 4, 6, 7, 3,
        4, 6, 7, 3, 4, 3, 4, 5, 7, 8, 3, 4, 5, 7, 8, 3, 4, 5, 7, 8, 3, 4, 5, 7, 8, 3, 4, 5, 6, 8, 9, 5, 6, 8, 9, 5, 6, 8, 9, 5, 6, 8, 9, 5,
        6, 8, 9, 5, 6, 6, 7, 9, 10, 6, 7, 9, 10, 6, 7, 9, 10, 6, 7, 9, 10, 6, 7, 9, 10, 6, 7, 7, 8, 10, 11, 7, 8, 10, 11, 7, 8, 10, 11, 7,
        8, 10, 11, 7, 8, 10, 11, 7, 8, 8, 9, 11, 12, 13, 8, 9, 11, 12, 13, 8, 9, 11, 12, 13, 8, 9, 11, 12, 13, 8, 9, 9, 10, 12, 13, 9, 10,
        12, 13, 9, 10, 12, 13, 9, 10, 12, 13, 9, 10, 12, 13, 9, 10, 10, 11, 13, 14, 10, 11, 13, 14, 10, 11, 13, 14, 10, 11, 13, 14, 10, 11,
        13, 14, 10, 11, 11, 12, 14, 15, 11, 12, 14, 15, 11, 12, 14, 15, 11, 12, 14, 15, 11, 12, 14, 15, 11, 12, 12, 13, 15, 16, 12, 13, 15,
        16, 12, 13, 15, 16, 12, 13, 15, 16, 12, 13, 15, 16, 12, 13, 13, 14, 16, 26, 13, 14, 16, 26, 13, 14, 16, 26, 13, 14, 16, 26, 13, 14,
        16, 26, 13, 14, 14, 15, 25, 26, 45, 14, 15, 25, 26, 45, 14, 15, 25, 26, 45, 14, 15, 25, 26, 45, 14, 15, 0, 1, 2, 18, 19, 36, 37, 41,
        0, 1, 2, 18, 19, 36, 37, 41, 0, 1, 2, 18, 19, 36, 0, 1, 17, 19, 20, 36, 37, 38, 41, 0, 1, 17, 19, 20, 36, 37, 38, 41, 0, 1, 17, 19,
        0, 17, 18, 20, 21, 36, 37, 38, 40, 41, 0, 17, 18, 20, 21, 36, 37, 38, 40, 41, 0, 17, 17, 18, 19, 21, 36, 37, 38, 39, 40, 41, 17, 18,
        19, 21, 36, 37, 38, 39, 40, 41, 17, 18, 18, 19, 20, 22, 27, 28, 37, 38, 39, 40, 41, 18, 19, 20, 22, 27, 28, 37, 38, 39, 40, 41, 21,
        23, 24, 25, 27, 28, 42, 43, 44, 46, 47, 21, 23, 24, 25, 27, 28, 42, 43, 44, 46, 47, 22, 24, 25, 26, 42, 43, 44, 45, 46, 47, 22, 24,
        25, 26, 42, 43, 44, 45, 46, 47, 22, 24, 16, 22, 23, 25, 26, 43, 44, 45, 46, 47, 16, 22, 23, 25, 26, 43, 44, 45, 46, 47, 16, 22, 15,
        16, 23, 24, 26, 43, 44, 45, 46, 15, 16, 23, 24, 26, 43, 44, 45, 46, 15, 16, 23, 24, 14, 15, 16, 24, 25, 44, 45, 46, 14, 15, 16, 24,
        25, 44, 45, 46, 14, 15, 16, 24, 25, 44, 20, 21, 22, 23, 28, 29, 38, 39, 40, 42, 43, 47, 20, 21, 22, 23, 28, 29, 38, 39, 40, 42, 21,
        22, 27, 29, 30, 39, 40, 42, 47, 21, 22, 27, 29, 30, 39, 40, 42, 47, 21, 22, 27, 29, 27, 28, 30, 31, 35, 39, 42, 27, 28, 30, 31, 35,
        39, 42, 27, 28, 30, 31, 35, 39, 42, 27, 28, 29, 31, 32, 33, 34, 35, 28, 29, 31, 32, 33, 34, 35, 28, 29, 31, 32, 33, 34, 35, 28, 2,
        3, 29, 30, 32, 33, 48, 49, 2, 3, 29, 30, 32, 33, 48, 49, 2, 3, 29, 30, 32, 33, 29, 30, 31, 33, 34, 35, 49, 50, 29, 30, 31, 33, 34,
        35, 49, 50, 29, 30, 31, 33, 34, 35, 29, 30, 31, 32, 34, 35, 50, 51, 52, 29, 30, 31, 32, 34, 35, 50, 51, 52, 29, 30, 31, 32, 29, 30,
        31, 32, 33, 35, 52, 53, 29, 30, 31, 32, 33, 35, 52, 53, 29, 30, 31, 32, 33, 35, 13, 14, 29, 30, 32, 33, 34, 53, 54, 13, 14, 29, 30,
        32, 33, 34, 53, 54, 13, 14, 29, 30, 0, 1, 2, 17, 18, 19, 20, 37, 38, 39, 40, 41, 0, 1, 2, 17, 18, 19, 20, 37, 38, 39, 0, 1, 17, 18,
        19, 20, 21, 36, 38, 39, 40, 41, 0, 1, 17, 18, 19, 20, 21, 36, 38, 39, 0, 1, 17, 18, 19, 20, 21, 27, 28, 36, 37, 39, 40, 41, 0, 1,
        17, 18, 19, 20, 21, 27, 19, 20, 21, 27, 28, 29, 36, 37, 38, 40, 41, 19, 20, 21, 27, 28, 29, 36, 37, 38, 40, 41, 0, 1, 17, 18, 19,
        20, 21, 27, 28, 36, 37, 38, 39, 41, 0, 1, 17, 18, 19, 20, 21, 27, 0, 1, 2, 17, 18, 19, 20, 21, 36, 37, 38, 39, 40, 0, 1, 2, 17, 18,
        19, 20, 21, 36, 22, 23, 24, 27, 28, 29, 43, 44, 45, 46, 47, 22, 23, 24, 27, 28, 29, 43, 44, 45, 46, 47, 15, 16, 22, 23, 24, 25, 26,
        27, 42, 44, 45, 46, 47, 15, 16, 22, 23, 24, 25, 26, 27, 42, 15, 16, 22, 23, 24, 25, 26, 42, 43, 45, 46, 47, 15, 16, 22, 23, 24, 25,
        26, 42, 43, 45, 14, 15, 16, 23, 24, 25, 26, 42, 43, 44, 46, 47, 14, 15, 16, 23, 24, 25, 26, 42, 43, 44, 14, 15, 16, 22, 23, 24, 25,
        26, 42, 43, 44, 45, 47, 14, 15, 16, 22, 23, 24, 25, 26, 42, 15, 16, 22, 23, 24, 25, 26, 27, 28, 42, 43, 44, 45, 46, 15, 16, 22, 23,
        24, 25, 26, 27, 2, 3, 4, 5, 6, 49, 59, 60, 2, 3, 4, 5, 6, 49, 59, 60, 2, 3, 4, 5, 6, 49, 3, 4, 5, 31, 32, 48, 50, 51, 59, 60, 61,
        67, 3, 4, 5, 31, 32, 48, 50, 51, 59, 60, 30, 31, 32, 33, 34, 48, 49, 51, 52, 58, 59, 60, 61, 62, 66, 67, 30, 31, 32, 33, 34, 48, 30,
        31, 32, 33, 34, 35, 48, 49, 50, 52, 53, 54, 56, 58, 60, 61, 62, 63, 64, 65, 66, 67, 30, 32, 33, 34, 35, 50, 51, 53, 54, 55, 56, 62,
        63, 64, 65, 30, 32, 33, 34, 35, 50, 51, 11, 12, 13, 34, 35, 52, 54, 55, 63, 64, 65, 11, 12, 13, 34, 35, 52, 54, 55, 63, 64, 65, 10,
        11, 12, 13, 14, 53, 55, 64, 10, 11, 12, 13, 14, 53, 55, 64, 10, 11, 12, 13, 14, 53, 8, 9, 10, 11, 12, 13, 53, 54, 56, 57, 63, 64,
        65, 8, 9, 10, 11, 12, 13, 53, 54, 56, 7, 8, 9, 10, 11, 12, 54, 55, 57, 58, 63, 64, 65, 66, 7, 8, 9, 10, 11, 12, 54, 55, 6, 7, 8, 9,
        10, 55, 56, 58, 59, 62, 65, 66, 67, 6, 7, 8, 9, 10, 55, 56, 58, 59, 4, 5, 6, 7, 8, 9, 48, 56, 57, 59, 60, 61, 62, 66, 67, 4, 5, 6,
        7, 8, 9, 48, 3, 4, 5, 6, 7, 8, 48, 49, 57, 58, 60, 61, 67, 3, 4, 5, 6, 7, 8, 48, 49, 57, 2, 3, 4, 5, 6, 31, 48, 49, 59, 2, 3, 4, 5,
        6, 31, 48, 49, 59, 2, 3, 4, 5, 31, 32, 33, 48, 49, 50, 51, 52, 57, 58, 59, 60, 62, 63, 66, 67, 31, 32, 33, 48, 49, 50, 33, 34, 48,
        49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 63, 64, 65, 66, 67, 33, 34, 35, 50, 51, 52, 53, 54, 55, 56, 57, 61, 62, 64, 65,
        66, 34, 35, 50, 51, 52, 53, 54, 10, 11, 12, 13, 14, 35, 53, 54, 55, 10, 11, 12, 13, 14, 35, 53, 54, 55, 10, 11, 12, 13, 9, 10, 11,
        12, 51, 52, 53, 54, 55, 56, 57, 58, 61, 62, 63, 64, 66, 67, 9, 10, 11, 12, 7, 8, 9, 50, 51, 52, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        64, 65, 67, 7, 8, 9, 50, 4, 5, 6, 7, 48, 49, 50, 51, 56, 57, 58, 59, 60, 61, 62, 63, 65, 66, 4, 5, 6, 7
    };
    const unsigned int reverse_index2[68 * 22] = {
        0, 3, 1, 7, 8, 0, 3, 1, 7, 8, 0, 3, 1, 7, 8, 0, 3, 1, 7, 8, 0, 3, 1, 1, 4, 9, 1, 1, 4, 9, 1, 1, 4, 9, 1, 1, 4, 9, 1, 1, 4, 9, 1, 1,
        6, 1, 1, 5, 6, 1, 1, 5, 6, 1, 1, 5, 6, 1, 1, 5, 6, 1, 1, 5, 6, 1, 5, 0, 0, 6, 5, 0, 0, 6, 5, 0, 0, 6, 5, 0, 0, 6, 5, 0, 0, 6, 5, 0,
        2, 0, 1, 7, 2, 0, 1, 7, 2, 0, 1, 7, 2, 0, 1, 7, 2, 0, 1, 7, 2, 0, 2, 1, 1, 6, 2, 1, 1, 6, 2, 1, 1, 6, 2, 1, 1, 6, 2, 1, 1, 6, 2, 1,
        9, 4, 0, 1, 4, 9, 4, 0, 1, 4, 9, 4, 0, 1, 4, 9, 4, 0, 1, 4, 9, 4, 5, 0, 1, 3, 5, 0, 1, 3, 5, 0, 1, 3, 5, 0, 1, 3, 5, 0, 1, 3, 5, 0,
        4, 0, 0, 4, 4, 0, 0, 4, 4, 0, 0, 4, 4, 0, 0, 4, 4, 0, 0, 4, 4, 0, 3, 0, 0, 5, 3, 0, 0, 5, 3, 0, 0, 5, 3, 0, 0, 5, 3, 0, 0, 5, 3, 0,
        3, 1, 0, 4, 9, 3, 1, 0, 4, 9, 3, 1, 0, 4, 9, 3, 1, 0, 4, 9, 3, 1, 6, 1, 0, 2, 6, 1, 0, 2, 6, 1, 0, 2, 6, 1, 0, 2, 6, 1, 0, 2, 6, 1,
        7, 1, 0, 2, 7, 1, 0, 2, 7, 1, 0, 2, 7, 1, 0, 2, 7, 1, 0, 2, 7, 1, 6, 1, 1, 4, 6, 1, 1, 4, 6, 1, 1, 4, 6, 1, 1, 4, 6, 1, 1, 4, 6, 1,
        5, 1, 0, 6, 5, 1, 0, 6, 5, 1, 0, 6, 5, 1, 0, 6, 5, 1, 0, 6, 5, 1, 3, 0, 0, 9, 3, 0, 0, 9, 3, 0, 0, 9, 3, 0, 0, 9, 3, 0, 0, 9, 3, 0,
        3, 1, 7, 2, 8, 3, 1, 7, 2, 8, 3, 1, 7, 2, 8, 3, 1, 7, 2, 8, 3, 1, 0, 3, 9, 0, 4, 4, 8, 6, 0, 3, 9, 0, 4, 4, 8, 6, 0, 3, 9, 0, 4, 4,
        3, 8, 0, 0, 6, 5, 7, 9, 7, 3, 8, 0, 0, 6, 5, 7, 9, 7, 3, 8, 0, 0, 7, 4, 1, 1, 6, 6, 5, 7, 9, 5, 7, 4, 1, 1, 6, 6, 5, 7, 9, 5, 7, 4,
        8, 4, 1, 0, 9, 6, 4, 7, 6, 8, 8, 4, 1, 0, 9, 6, 4, 7, 6, 8, 8, 4, 9, 6, 0, 4, 2, 7, 9, 6, 5, 5, 9, 9, 6, 0, 4, 2, 7, 9, 6, 5, 5, 9,
        4, 1, 6, 9, 3, 8, 5, 6, 9, 9, 6, 4, 1, 6, 9, 3, 8, 5, 6, 9, 9, 6, 0, 1, 4, 8, 7, 5, 7, 9, 8, 5, 0, 1, 4, 8, 7, 5, 7, 9, 8, 5, 0, 1,
        7, 6, 0, 1, 4, 7, 5, 6, 6, 9, 7, 6, 0, 1, 4, 7, 5, 6, 6, 9, 7, 6, 8, 3, 5, 0, 0, 9, 6, 5, 7, 8, 3, 5, 0, 0, 9, 6, 5, 7, 8, 3, 5, 0,
        8, 3, 1, 4, 0, 8, 4, 5, 8, 3, 1, 4, 0, 8, 4, 5, 8, 3, 1, 4, 0, 8, 9, 1, 1, 9, 1, 2, 8, 4, 7, 2, 8, 7, 9, 1, 1, 9, 1, 2, 8, 4, 7, 2,
        8, 8, 0, 0, 6, 6, 8, 6, 8, 8, 8, 0, 0, 6, 6, 8, 6, 8, 8, 8, 0, 0, 5, 0, 0, 9, 9, 9, 9, 5, 0, 0, 9, 9, 9, 9, 5, 0, 0, 9, 9, 9, 9, 5,
        4, 1, 2, 2, 2, 2, 2, 4, 1, 2, 2, 2, 2, 2, 4, 1, 2, 2, 2, 2, 2, 4, 8, 8, 6, 5, 0, 7, 7, 9, 8, 8, 6, 5, 0, 7, 7, 9, 8, 8, 6, 5, 0, 7,
        4, 3, 0, 0, 4, 5, 8, 7, 4, 3, 0, 0, 4, 5, 8, 7, 4, 3, 0, 0, 4, 5, 7, 2, 1, 1, 1, 1, 5, 8, 5, 7, 2, 1, 1, 1, 1, 5, 8, 5, 7, 2, 1, 1,
        3, 1, 5, 4, 1, 0, 6, 9, 3, 1, 5, 4, 1, 0, 6, 9, 3, 1, 5, 4, 1, 0, 8, 9, 5, 4, 9, 6, 0, 8, 7, 8, 9, 5, 4, 9, 6, 0, 8, 7, 8, 9, 5, 4,
        2, 2, 4, 2, 3, 5, 8, 1, 5, 8, 4, 1, 2, 2, 4, 2, 3, 5, 8, 1, 5, 8, 5, 6, 3, 2, 2, 3, 7, 1, 1, 3, 3, 0, 5, 6, 3, 2, 2, 3, 7, 1, 1, 3,
        9, 9, 6, 6, 3, 2, 2, 7, 9, 3, 2, 1, 0, 3, 9, 9, 6, 6, 3, 2, 2, 7, 9, 4, 3, 4, 3, 9, 7, 4, 2, 1, 4, 9, 4, 3, 4, 3, 9, 7, 4, 2, 1, 4,
        8, 7, 7, 8, 8, 5, 5, 8, 5, 2, 3, 0, 0, 2, 8, 7, 7, 8, 8, 5, 5, 8, 4, 4, 5, 5, 5, 7, 7, 9, 0, 0, 3, 2, 2, 4, 4, 5, 5, 5, 7, 7, 9, 0,
        3, 4, 9, 1, 2, 8, 2, 4, 7, 4, 2, 3, 4, 9, 1, 2, 8, 2, 4, 7, 4, 2, 9, 9, 2, 2, 3, 6, 6, 6, 1, 2, 3, 3, 0, 9, 9, 2, 2, 3, 6, 6, 6, 1,
        6, 5, 7, 3, 2, 2, 3, 4, 1, 1, 1, 3, 6, 5, 7, 3, 2, 2, 3, 4, 1, 1, 4, 2, 2, 8, 5, 3, 1, 8, 4, 1, 0, 4, 4, 2, 2, 8, 5, 3, 1, 8, 4, 1,
        5, 5, 4, 9, 7, 7, 5, 5, 3, 3, 0, 0, 1, 5, 5, 4, 9, 7, 7, 5, 5, 3, 7, 8, 5, 6, 8, 8, 7, 9, 6, 0, 0, 3, 2, 2, 7, 8, 5, 6, 8, 8, 7, 9,
        6, 3, 2, 2, 5, 3, 3, 0, 6, 3, 2, 2, 5, 3, 3, 0, 6, 3, 2, 2, 5, 3, 6, 7, 8, 4, 6, 1, 3, 9, 4, 1, 5, 8, 6, 7, 8, 4, 6, 1, 3, 9, 4, 1,
        7, 3, 3, 4, 8, 5, 1, 1, 7, 9, 8, 5, 1, 6, 9, 5, 7, 3, 3, 4, 8, 5, 9, 6, 5, 3, 5, 6, 9, 6, 1, 1, 6, 9, 8, 8, 8, 3, 0, 3, 8, 6, 6, 6,
        8, 8, 5, 3, 3, 8, 2, 1, 5, 8, 9, 7, 1, 5, 4, 8, 8, 5, 3, 3, 8, 2, 8, 7, 6, 6, 4, 3, 1, 3, 5, 1, 8, 8, 7, 6, 6, 4, 3, 1, 3, 5, 1, 8,
        5, 2, 2, 4, 6, 2, 4, 0, 5, 2, 2, 4, 6, 2, 4, 0, 5, 2, 2, 4, 6, 2, 7, 5, 2, 3, 6, 7, 5, 2, 2, 9, 8, 2, 5, 7, 5, 2, 3, 6, 7, 5, 2, 2,
        7, 5, 2, 3, 7, 8, 6, 0, 1, 5, 7, 6, 3, 8, 7, 5, 2, 3, 7, 8, 6, 0, 8, 4, 2, 4, 8, 7, 0, 0, 7, 8, 7, 4, 7, 8, 4, 2, 4, 8, 7, 0, 0, 7,
        9, 7, 3, 2, 6, 7, 6, 5, 0, 0, 6, 7, 9, 7, 3, 9, 7, 3, 2, 6, 7, 6, 7, 6, 3, 2, 5, 8, 2, 5, 8, 2, 2, 8, 4, 7, 6, 3, 2, 5, 8, 2, 5, 8,
        7, 5, 3, 4, 6, 8, 0, 0, 1, 7, 5, 3, 4, 6, 8, 0, 0, 1, 7, 5, 3, 4, 7, 7, 9, 3, 2, 0, 3, 9, 6, 4, 5, 3, 2, 6, 3, 0, 7, 7, 9, 3, 2, 0,
        8, 9, 8, 7, 2, 0, 2, 7, 8, 9, 6, 5, 6, 9, 7, 2, 2, 7, 2, 0, 2, 8, 7, 7, 9, 4, 0, 3, 3, 5, 4, 7, 6, 3, 3, 0, 5, 7, 7, 9, 4, 0, 3, 3,
        6, 4, 3, 5, 7, 8, 0, 0, 1, 6, 4, 3, 5, 7, 8, 0, 0, 1, 6, 4, 3, 5, 8, 9, 9, 9, 7, 4, 4, 4, 2, 1, 4, 7, 9, 5, 0, 4, 2, 9, 8, 9, 9, 9,
        9, 9, 9, 6, 5, 8, 6, 3, 2, 3, 6, 9, 4, 1, 4, 9, 1, 1, 9, 9, 9, 6, 8, 9, 9, 8, 4, 4, 4, 6, 7, 3, 1, 2, 4, 0, 4, 9, 9, 1, 8, 9, 9, 8
    };

  };
}

#endif //LITE_AI_TOOLKIT_NCNN_CV_NCNN_PIPNET68_H

//
// Created by DefTruth on 2021/11/14.
//

#ifndef LITE_AI_TOOLKIT_NCNN_CV_NCNN_CAVA_COMBINED_FACE_H
#define LITE_AI_TOOLKIT_NCNN_CV_NCNN_CAVA_COMBINED_FACE_H

#include "../ncnn_core.h"

namespace ncnncv
{
  class LITE_EXPORTS NCNNCavaCombinedFace : public BasicNCNNHandler
  {
  public:
    explicit NCNNCavaCombinedFace(const std::string &_param_path,
                                  const std::string &_bin_path,
                                  unsigned int _num_threads = 1) :
        BasicNCNNHandler(_param_path, _bin_path, _num_threads)
    {};

    ~NCNNCavaCombinedFace() override = default;

  private:
    const float mean_vals[3] = {127.5f, 127.5f, 127.5f}; // RGB
    const float norm_vals[3] = {1.f / 128.0f, 1.f / 128.0f, 1.f / 128.0f};
    static constexpr const int input_width = 112;
    static constexpr const int input_height = 112;

  private:
    void transform(const cv::Mat &mat, ncnn::Mat &in) override;

  public:
    void detect(const cv::Mat &mat, types::FaceContent &face_content);
  };
}

#endif //LITE_AI_TOOLKIT_NCNN_CV_NCNN_CAVA_COMBINED_FACE_H

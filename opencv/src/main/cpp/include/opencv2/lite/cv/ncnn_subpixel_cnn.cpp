//
// Created by DefTruth on 2021/11/29.
//

#include "ncnn_subpixel_cnn.h"

using ncnncv::NCNNSubPixelCNN;

NCNNSubPixelCNN::NCNNSubPixelCNN(
    const std::string &_param_path,
    const std::string &_bin_path,
    unsigned int _num_threads) :
    BasicNCNNHandler(_param_path, _bin_path, _num_threads)
{
}

void NCNNSubPixelCNN::transform(const cv::Mat &mat, ncnn::Mat &in)
{
  cv::Mat mat_y; // assume that input mat is Y of YCrCb
  mat.convertTo(mat_y, CV_32FC1, 1.0f / 255.0f, 0.f); // (224,224,1) range (0.,1.0)

  in = ncnn::Mat(input_width, input_height, mat_y.data);
}

void NCNNSubPixelCNN::detect(const cv::Mat &mat, types::SuperResolutionContent &super_resolution_content)
{
  if (mat.empty()) return;
  cv::Mat mat_copy = mat.clone();
  cv::resize(mat_copy, mat_copy, cv::Size(input_width, input_height)); // (224,224,3)
  cv::Mat mat_ycrcb, mat_y, mat_cr, mat_cb;
  cv::cvtColor(mat_copy, mat_ycrcb, cv::COLOR_BGR2YCrCb);

  // 0. split
  std::vector<cv::Mat> split_mats;
  cv::split(mat_ycrcb, split_mats);
  mat_y = split_mats.at(0); // (224,224,1) uchar CV_8UC1
  mat_cr = split_mats.at(1);
  mat_cb = split_mats.at(2);

  // 1. make input tensor
  ncnn::Mat input;
  this->transform(mat_y, input); // (1,1,224,224)
  // 2. inference & extract
  auto extractor = net->create_extractor();
  extractor.set_light_mode(false);  // default
  extractor.set_num_threads(num_threads);
  extractor.input("input", input);
  // 3. fetch.
  ncnn::Mat pred;
  extractor.extract("output", pred); // (1,1,672,672)
#ifdef LITENCNN_DEBUG
  BasicNCNNHandler::print_shape(pred, "output");
#endif

  const unsigned int rows = pred.h; // H
  const unsigned int cols = pred.w; // W

  float *pred_ptr = (float *) pred.data;

  mat_y = cv::Mat(rows, cols, CV_32FC1, pred_ptr); // release & create

  mat_y *= 255.0f;

  mat_y.convertTo(mat_y, CV_8UC1);

  cv::resize(mat_cr, mat_cr, cv::Size(cols, rows));
  cv::resize(mat_cb, mat_cb, cv::Size(cols, rows));

  std::vector<cv::Mat> out_mats;
  out_mats.push_back(mat_y);
  out_mats.push_back(mat_cr);
  out_mats.push_back(mat_cb);

  // 3. merge
  cv::merge(out_mats, super_resolution_content.mat);
  if (super_resolution_content.mat.empty())
  {
    super_resolution_content.flag = false;
    return;
  }
  cv::cvtColor(super_resolution_content.mat, super_resolution_content.mat, cv::COLOR_YCrCb2BGR);
  super_resolution_content.flag = true;
}
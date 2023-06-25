//
// Created by DefTruth on 2021/8/8.
//

#ifndef LITE_AI_MODELS_H
#define LITE_AI_MODELS_H

#include "backend.h"



// ENABLE_NCNN
#ifdef ENABLE_NCNN
#include "ncnn_core.h"
#include "ncnn_utils.h"
#include "cv/ncnn_nanodet.h"
#include "cv/ncnn_nanodet_efficientnet_lite.h"
#include "cv/ncnn_nanodet_depreciated.h"
#include "cv/ncnn_nanodet_efficientdet_lite_depreciated.h"
#include "cv/ncnn_rvm.h"
#include "cv/ncnn_yolox.h"
#include "cv/ncnn_yolop.h"
#include "cv/ncnn_yolov5.h"
#include "cv/ncnn_yolox_v0.1.1.h"
#include "cv/ncnn_yolor.h"
#include "cv/ncnn_yolor_ssss.h"
#include "cv/ncnn_yolov5_v6.0.h"
#include "cv/ncnn_yolov5_v6.0_p6.h"
#include "cv/ncnn_glint_arcface.h"
#include "cv/ncnn_glint_cosface.h"
#include "cv/ncnn_glint_partial_fc.h"
#include "cv/ncnn_facenet.h"
#include "cv/ncnn_focal_arcface.h"
#include "cv/ncnn_focal_asia_arcface.h"
#include "cv/ncnn_tencent_curricular_face.h"
#include "cv/ncnn_tencent_cifp_face.h"
#include "cv/ncnn_center_loss_face.h"
#include "cv/ncnn_sphere_face.h"
#include "cv/ncnn_mobile_facenet.h"
#include "cv/ncnn_cava_ghost_arcface.h"
#include "cv/ncnn_cava_combined_face.h"
#include "cv/ncnn_mobilese_focal_face.h"
#include "cv/ncnn_ultraface.h"
#include "cv/ncnn_retinaface.h"
#include "cv/ncnn_faceboxes.h"
#include "cv/ncnn_face_landmarks_1000.h"
#include "cv/ncnn_pfld.h"
#include "cv/ncnn_pfld68.h"
#include "cv/ncnn_pfld98.h"
#include "cv/ncnn_mobilenetv2_68.h"
#include "cv/ncnn_mobilenetv2_se_68.h"
#include "cv/ncnn_age_googlenet.h"
#include "cv/ncnn_gender_googlenet.h"
#include "cv/ncnn_emotion_ferplus.h"
#include "cv/ncnn_efficient_emotion7.h"
#include "cv/ncnn_efficient_emotion8.h"
#include "cv/ncnn_mobile_emotion7.h"
#include "cv/ncnn_efficientnet_lite4.h"
#include "cv/ncnn_shufflenetv2.h"
#include "cv/ncnn_densenet.h"
#include "cv/ncnn_ghostnet.h"
#include "cv/ncnn_hdrdnet.h"
#include "cv/ncnn_ibnnet.h"
#include "cv/ncnn_mobilenetv2.h"
#include "cv/ncnn_resnet.h"
#include "cv/ncnn_resnext.h"
#include "cv/ncnn_deeplabv3_resnet101.h"
#include "cv/ncnn_fcn_resnet101.h"
#include "cv/ncnn_colorizer.h"
#include "cv/ncnn_fast_style_transfer.h"
#include "cv/ncnn_subpixel_cnn.h"
#include "cv/ncnn_nanodet_plus.h"
#include "cv/ncnn_scrfd.h"
#include "cv/ncnn_yolo5face.h"
#include "cv/ncnn_faceboxesv2.h"
#include "cv/ncnn_pipnet98.h"
#include "cv/ncnn_pipnet68.h"
#include "cv/ncnn_pipnet29.h"
#include "cv/ncnn_pipnet19.h"
#include "cv/ncnn_insectid.h"
#include "cv/ncnn_plantid.h"
#include "cv/ncnn_modnet.h"
#include "cv/ncnn_female_photo2cartoon.h"

#endif

// NCNN version
namespace lite
{
#ifdef ENABLE_NCNN
  namespace ncnn
  {
    // mediapipe
    namespace mediapipe
    {
    }

    namespace cv
    {
      // classification
      namespace classification
      {
        typedef ncnncv::NCNNEfficientNetLite4 EfficientNetLite4;
        typedef ncnncv::NCNNShuffleNetV2 ShuffleNetV2;
        typedef ncnncv::NCNNDenseNet DenseNet;
        typedef ncnncv::NCNNGhostNet GhostNet;
        typedef ncnncv::NCNNHdrDNet HdrDNet;
        typedef ncnncv::NCNNIBNNet IBNNet;
        typedef ncnncv::NCNNMobileNetV2 MobileNetV2;
        typedef ncnncv::NCNNResNet ResNet;
        typedef ncnncv::NCNNResNeXt ResNeXt;
        typedef ncnncv::NCNNInsectID InsectID;
        typedef ncnncv::NCNNPlantID PlantID;
      }
      // object detection
      namespace detection
      {
        typedef ncnncv::NCNNNanoDet NanoDet;
        typedef ncnncv::NCNNNanoDetEfficientNetLite NanoDetEfficientNetLite;
        typedef ncnncv::NCNNNanoDetDepreciated NanoDetDepreciated;
        typedef ncnncv::NCNNNanoDetEfficientNetLiteDepreciated NanoDetEfficientNetLiteDepreciated;
        typedef ncnncv::NCNNYoloX YoloX;
        typedef ncnncv::NCNNYOLOP YOLOP;
        typedef ncnncv::NCNNYoloV5 YoloV5;
        typedef ncnncv::NCNNYoloX_V_0_1_1 YoloX_V_0_1_1;
        typedef ncnncv::NCNNYoloR YoloR;
        typedef ncnncv::NCNNYoloRssss YoloRssss;
        typedef ncnncv::NCNNYoloV5_V_6_0 YoloV5_V_6_0;
        typedef ncnncv::NCNNYoloV5_V_6_0_P6 YoloV5_V_6_0_P6;
        typedef ncnncv::NCNNNanoDetPlus NanoDetPlus;
      }
      // face etc.
      namespace face
      {
        namespace detect
        {
          typedef ncnncv::NCNNUltraFace UltraFace;
          typedef ncnncv::NCNNRetinaFace RetinaFace;
          typedef ncnncv::NCNNFaceBoxes FaceBoxes;
          typedef ncnncv::NCNNSCRFD SCRFD;
          typedef ncnncv::NCNNYOLO5Face YOLO5Face;
          typedef ncnncv::NCNNFaceBoxesV2 FaceBoxesV2;
        }
        namespace align
        {
          typedef ncnncv::NCNNFaceLandmark1000 FaceLandmark1000;
          typedef ncnncv::NCNNPFLD PFLD;
          typedef ncnncv::NCNNPFLD68 PFLD68;
          typedef ncnncv::NCNNPFLD98 PFLD98;
          typedef ncnncv::NCNNMobileNetV268 MobileNetV268;
          typedef ncnncv::NCNNMobileNetV2SE68 MobileNetV2SE68;
          typedef ncnncv::NCNNPIPNet98 PIPNet98;
          typedef ncnncv::NCNNPIPNet68 PIPNet68;
          typedef ncnncv::NCNNPIPNet29 PIPNet29;
          typedef ncnncv::NCNNPIPNet19 PIPNet19;
        }

        namespace align3d
        {
        }

        namespace pose
        {
        }
        namespace attr
        {
          typedef ncnncv::NCNNAgeGoogleNet AgeGoogleNet;
          typedef ncnncv::NCNNGenderGoogleNet GenderGoogleNet;
          typedef ncnncv::NCNNEmotionFerPlus EmotionFerPlus;
          typedef ncnncv::NCNNEfficientEmotion7 EfficientEmotion7;
          typedef ncnncv::NCNNEfficientEmotion8 EfficientEmotion8;
          typedef ncnncv::NCNNMobileEmotion7 MobileEmotion7;
        }
      }
      // face recognition
      namespace faceid
      {
        typedef ncnncv::NCNNGlintArcFace GlintArcFace;
        typedef ncnncv::NCNNGlintCosFace GlintCosFace;
        typedef ncnncv::NCNNGlintPartialFC GlintPartialFC;
        typedef ncnncv::NCNNFaceNet FaceNet;
        typedef ncnncv::NCNNFocalArcFace FocalArcFace;
        typedef ncnncv::NCNNFocalAsiaArcFace FocalAsiaArcFace;
        typedef ncnncv::NCNNTencentCurricularFace TencentCurricularFace;
        typedef ncnncv::NCNNTencentCifpFace TencentCifpFace;
        typedef ncnncv::NCNNCenterLossFace CenterLossFace;
        typedef ncnncv::NCNNSphereFace SphereFace;
        typedef ncnncv::NCNNMobileFaceNet MobileFaceNet;
        typedef ncnncv::NCNNCavaGhostArcFace CavaGhostArcFace;
        typedef ncnncv::NCNNCavaCombinedFace CavaCombinedFace;
        typedef ncnncv::NCNNMobileSEFocalFace MobileSEFocalFace;
      }
      // segmentation
      namespace segmentation
      {
        typedef ncnncv::NCNNDeepLabV3ResNet101 DeepLabV3ResNet101;
        typedef ncnncv::NCNNFCNResNet101 FCNResNet101;
      }
      // reid
      namespace reid
      {
      }
      // ocr
      namespace ocr
      {
      }
      // matting
      namespace matting
      {
        typedef ncnncv::NCNNRobustVideoMatting RobustVideoMatting;
        typedef ncnncv::NCNNMODNet MODNet;
      }
      // style transfer
      namespace style
      {
        typedef ncnncv::NCNNFastStyleTransfer FastStyleTransfer;
        typedef ncnncv::NCNNFemalePhoto2Cartoon FemalePhoto2Cartoon;
      }

      // colorization
      namespace colorization
      {
        typedef ncnncv::NCNNColorizer Colorizer;
      }
      // super resolution
      namespace resolution
      {
        typedef ncnncv::NCNNSubPixelCNN SubPixelCNN;
      }

    } // namespace cv

  }
#endif
}

#endif //LITE_AI_MODELS_H

# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
if(DEFINED ANDROID_NDK_MAJOR AND ${ANDROID_NDK_MAJOR} GREATER 20)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-openmp")
endif()
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# 搜索当前目录下的所有.cpp文件
aux_source_directory(./ SRC_LIST)
aux_source_directory(./ocr OCR_SRC_LIST)
add_library(
        native-lib
        SHARED
        HightLightAndDarkLight.cpp
        include/opencv2/hsl/HSL.cpp
        include/opencv2/hsl/ColorSpace.cpp
        include/opencv2/hsl/ColorSpace.hpp
        include/opencv2/beauty/FaceBlush.cpp
        FaceLandmark.cpp
        FaceOption.cpp
        ${SRC_LIST}
        ${OCR_SRC_LIST}
        include/opencv2/lut/lut3d.cpp
        include/opencv2/lut/Lut.cpp
        SoftSkin.cpp
        include/opencv2/skin/CommenMixLayer.cpp
        include/opencv2/skin/SkinDetect.cpp
        include/opencv2/skin/SkinPdf.cpp
        include/opencv2/colorSpaceChange/TRGB2HSV.cpp
        include/opencv2/colorSpaceChange/TRGB2YCbCr.cpp
        include/opencv2/filter/GaussFilter.cpp
        lips/LipsColorChange.cpp
        include/opencv2/beauty/MakeupBase.cpp
        include/opencv2/beauty/AffineTransfrom.cpp
        nose/SteroNose.cpp
        include/opencv2/skin/SkinColor.cpp
        include/opencv2/beauty/BeautyIris.cpp
        include/opencv2/colorSpaceChange/TRGB2YUV.cpp
        include/opencv2/beauty/MeanFilter.cpp
        include/opencv2/beauty/EyeLash.cpp
        include/opencv2/beauty/EyeLid.cpp
        include/opencv2/beauty/EyeShadow.cpp
        include/opencv2/beauty/EyeBrow.cpp
        include/opencv2/beauty/EyeLine.cpp
        Mask.cpp
        Superres.cpp
        HsbNew.cpp
        MixImageLayer.cpp
        Test.cpp
        effects.cpp
        include/opencv2/lite/cv/ncnn_face_landmarks_1000.cpp
        include/opencv2/lite/cv/ncnn_female_photo2cartoon.cpp
        include/opencv2/lite/cv/ncnn_colorizer.cpp
        include/opencv2/lite/cv/ncnn_faceboxesv2.cpp
        include/opencv2/lite/cv/ncnn_yolo5face.cpp
        include/opencv2/lite/ncnn_handler.cpp
        include/opencv2/lite/ncnn_utils.cpp
        include/opencv2/lite/utils.cpp
)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library(log-lib log)
find_library(android-lib android)
find_library(vulkan-lib vulkan)
find_library(jnigraphics-lib jnigraphics)



set(ncnn_DIR ${CMAKE_SOURCE_DIR}/ncnn/${ANDROID_ABI}/lib/cmake/ncnn)
find_package(ncnn REQUIRED)

#[[include_directories(
        ncnn/${ANDROID_ABI}/include/ncnn
        ocr
)]]

include_directories(
        ${CMAKE_SOURCE_DIR}/include
)
add_library(libopencv_java4 STATIC IMPORTED)
set_target_properties(
        libopencv_java4
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libopencv_java4.so
)


add_library(libopencv_dnn STATIC IMPORTED)
set_target_properties(
        libopencv_dnn
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libopencv_dnn_superres.a
)

target_link_libraries( # Specifies the target library.
        native-lib
        libopencv_dnn
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${vulkan-lib}
        ${android-lib}
        ${jnigraphics-lib}
        libopencv_java4
        ncnn)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${vulkan-lib}
        ${android-lib}
        ${jnigraphics-lib}
        ncnn)

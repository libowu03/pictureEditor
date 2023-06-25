//
// Created by DefTruth on 2021/10/7.
//

#ifndef LITE_AI_TOOLKIT_NCNN_CORE_NCNN_DEFS_H
#define LITE_AI_TOOLKIT_NCNN_CORE_NCNN_DEFS_H

#include "config.h"
#include "lite.ai.defs.h"

#ifdef ENABLE_DEBUG_STRING
# define LITENCNN_DEBUG 1
#else
# define LITENCNN_DEBUG 0
#endif


#ifdef LITE_WIN32
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#endif


#endif //LITE_AI_TOOLKIT_NCNN_CORE_NCNN_DEFS_H

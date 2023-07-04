cmake_minimum_required(VERSION 3.16)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

project(deepstream_plugins)

find_package(CUDA REQUIRED)
if(CUDA_FOUND)
  find_cuda_helper_libs(nvinfer)
  message("-- CUDA_nvinfer_LIBRARY: " ${CUDA_nvinfer_LIBRARY})
  set(NVINFER_LIBRARY ${CUDA_nvinfer_LIBRARY} CACHE FILEPATH "location of the nvinfer library")

  find_cuda_helper_libs(nvonnxparser)
  set(NVONNXPARSER_LIBRARY ${CUDA_nvonnxparser_LIBRARY} CACHE FILEPATH "location of the nvonnxparser library")
  message("-- CUDA_nvonnxparser_LIBRARY: " ${NVONNXPARSER_LIBRARY})
  
  unset(CUDA_nvinfer_LIBRARY CACHE)
endif()


if(NVINFER_LIBRARY)
  find_path(NVINFER_INCLUDE_DIR
    NvInfer.h
    PATHS ${CUDA_TOOLKIT_INCLUDE}
    DOC "location of NvInfer.h"
    NO_DEFAULT_PATH
  )

  if(NOT NVINFER_INCLUDE_DIR)
    find_path(NVINFER_INCLUDE_DIR
        NvInfer.h
        DOC "location of NvInfer.h"
    )
  endif()

  message("-- Found NVINFER: " ${NVINFER_LIBRARY})
  message("-- Found NVINFER include: " ${NVINFER_INCLUDE_DIR})
endif()

# Set Deepstream lib location
set(DeepStream_DIR "/opt/nvidia/deepstream/deepstream-6.2")
if(DEFINED DeepStream_DIR)
  include_directories("${DeepStream_DIR}/sources/includes")
  message("-- Found DeepStream_DIR ")
endif(DEFINED DeepStream_DIR)   



include_directories(${CUDA_INCLUDE_DIRS} ${NVINFER_INCLUDE_DIR})

cuda_add_library(yolov8_bbox_parser SHARED
libbboxparser/parse_custom_bbox.cpp)

target_link_libraries(yolov8_bbox_parser ${CUDA_LIBRARIES} nvinfer nvinfer_plugin nvonnxparser)



#include "nvdsinfer_custom_impl.h"
#include "nvdsinfer_context.h"
#include <stdio.h>
#include<iostream>
#include<algorithm>



#define NETWORK_INPUT_WIDTH 640
#define NETWORK_INPUT_HEIGHT 640

#define VIDEO_INPUT_WIDTH 1280
#define VIDEO_INPUT_HEIGHT 1280


extern "C" bool NvDsParseCustomBoundingBox(
    std::vector<NvDsInferLayerInfo> const& outputLayerInfo,
    NvDsInferNetworkInfo const& networkInfo,
    NvDsInferParseDetectionParams const& detectionParams,
    std::vector<NvDsInferParseObjectInfo>& objectList);


extern "C" bool NvDsParseCustomBoundingBox(
    std::vector<NvDsInferLayerInfo> const& outputLayerInfo,
    NvDsInferNetworkInfo const& networkInfo,
    NvDsInferParseDetectionParams const& detectionParams,
    std::vector<NvDsInferParseObjectInfo>& objectList)
    {
        auto numOutputLayers = outputLayerInfo.size();

        float netWidth = networkInfo.width;
        float netHeight = networkInfo.height;

        float x_scale_factor = float(VIDEO_INPUT_WIDTH / NETWORK_INPUT_WIDTH); // use your scale factor
        float y_scale_factor = float(VIDEO_INPUT_HEIGHT / NETWORK_INPUT_HEIGHT); // use your scale factor

        NvDsInferLayerInfo layerInfo = outputLayerInfo[0];
        NvDsInferDims inferDims = layerInfo.inferDims;
        int numConfiguredClasses = detectionParams.numClassesConfigured;
        int dimPreds, dimCoords;
        int dimBatch = 1;
        if (inferDims.numDims == 2){
            dimPreds = inferDims.d[0];
            dimCoords = inferDims.d[1];
        }
        else{
            dimBatch = inferDims.d[0];
            dimPreds = inferDims.d[1];
            dimCoords = inferDims.d[2];
        }
        std::vector<NvDsInferParseObjectInfo> parsedDetections;
        
        float *data = (float*) outputLayerInfo[0].buffer;
        std::vector<NvDsInferParseObjectInfo> objects;
        NvDsInferParseObjectInfo obj;
        for(int i=0; i< dimCoords; i++){
            float x = *(data +  i);
            float y = *(data + (dimCoords * 1) + i);
            float w = *(data + (dimCoords * 2) + i);
            float h = *(data + (dimCoords * 3) + i);

            float left = (x - (0.5 * w)) * x_scale_factor;
            float top = (y - (0.5 * y)) * y_scale_factor;
            float width = w * x_scale_factor;
            float height = h * y_scale_factor;


            std::vector<float> cls_confidences;
            for (int j=0; j < numConfiguredClasses; j++){
                cls_confidences.push_back(*(data + (dimCoords * (4 + j)) + i));
            }

            unsigned int maxClsIdx = std::max_element(cls_confidences.begin(), cls_confidences.end()) - cls_confidences.begin();
            float maxClsConfidence = cls_confidences[maxClsIdx];

            if (maxClsConfidence < 0.2){
                continue;
            }

            obj = {maxClsIdx, left, top, width, height, maxClsConfidence};
            objects.push_back(obj);
        }
        objectList = objects;
        
        return true;
    }

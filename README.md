# yolov8_deepstream

Run the yolov8 object detector with NVIDIA deepstream


## Build the shared lib

```bash
mkdir build  && cd build
cmake ..
make
```

## Run  a deepstream pipeline

```bash
gst-launch-1.0 uridecodebin uri='file:///<absolute file path>' !  nvvideoconvert ! 'video/x-raw(memory:NVMM),format=(string)NV12,width=(int)1280,height=(int)720' ! smux.sink_0 nvstreammux name=smux width=640 height=640 batch-size=1 ! nvinfer config-file-path='deepstream_config.txt' ! nvvideoconvert ! nvdsosd ! nveglglessink

```
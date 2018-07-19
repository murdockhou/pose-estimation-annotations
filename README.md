# pose-estimation annotation program

## About
This is about pose-estimation annotation program for single people in single image.

## Installation

### Compiling
```bash
$ cmake CMakeLists.txt
$ make
```

### Running

```bash
$ ./pose_annotation_tool images_list.txt annotations_out.json
```

## How to use


### Using the tool
The image is loaded and displayes on screen one by one.

Press ENTER or any key (not SPACE or ESC) to annoate current image

Press SPACE to skip current image


#### Once the image has been selected for annotation

Left Click ------  Visible joint

Right Click ------- Occluded joint

TAB --------- Current joint is not applicable for current image

Any key -------- register the annotation and move to next joint (only accept the last time you "left click point".)

#### After all the joints are annotated the joint coordinates is written to .json file you input and next image is loaded. (The format of .JSON file is the same as the ai-challenger data format.)


## Code Reference
- [ https://github.com/motlabs/dont-be-turtle-pose-annotation-tool]( https://github.com/motlabs/dont-be-turtle-pose-annotation-tool ) (almost based this)

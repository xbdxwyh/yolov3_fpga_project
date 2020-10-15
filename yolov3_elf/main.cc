/*
 * Empty C++ Application
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "yolo3.h"


#include <chrono>

extern inline long long getTimeStampMill();


int main( int argc, char *argv[])
{
	unsigned int WEIGHT_BASE = 0x10000000;
    unsigned int BETA_BASE = 0x1EC38000;
    unsigned int MEM_BASE  = 0x1EC53000;

	printf("YOLOv3 TEST Begin\n");
	printf("timestamp is %ld\n",getTimeStampMill());
    char **names = get_labels((char*)"coco.names");
	int x;
    image **alphabet = load_alphabet();
    network *net = load_network((char*)"yolov3.cfg", (char*)"yolov3.weights", 0);
    set_batch_network(net, 1);

	char buff[256];
    char *input_imgfn = buff;
    if(argc==1)
    {
    	strncpy(input_imgfn, (char*)"dog.jpg", 256);
    }
    else
    {
    	strncpy(input_imgfn, argv[1], 256);
    }
	image im = load_image_stb(input_imgfn, 3);
	image sized = letterbox_image(im, 416,416);
	save_image_png(sized, "sized");
	double time;
	layer l = net->layers[net->n-1];
    float *X = sized.data;
    time = what_time_is_it_now();
	net = yolov2_hls_ps(net, X,WEIGHT_BASE,BETA_BASE,MEM_BASE);
	printf("Predicted in %f seconds.\n",what_time_is_it_now()-time);

    int nboxes = 0;
	int total = 0;
    float nms=0.45;
	float thresh = .5;
	float hier_thresh = .5;

    detection *dets = get_network_boxes(net, im.w, im.h, thresh, hier_thresh, 0, 1, &nboxes);

    if (nms) do_nms_sort(dets, nboxes, l.classes, nms);

    draw_detections(im, dets, nboxes, thresh, names, alphabet, l.classes);
    free_detections(dets, nboxes);

	save_image_png(im, "predictions");// output

	free_image(im);
    free_image(sized);

	printf("YOLOv3 TEST End\n");

    return 0;
}

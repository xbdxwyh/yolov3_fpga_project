#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "yolov3.h"

int main( int argc, char *argv[])
{
	//freopen("result.txt","w",stdout);

	printf("YOLOv3 TEST Begin\n");
    char **names = get_labels("coco.names");
	int x;
	for(x=0;x<80;x++)//80 classe labels
	{
		printf("[%d]%s\n",x,names[x]);
	}
    image **alphabet = load_alphabet();

    network *net = load_network("yolov3.cfg", "yolov3.weights", 0);
	set_batch_network(net, 1);

////////////////////load img resize img begin
	char buff[256];
    char *input_imgfn = buff;
	strncpy(input_imgfn, "dog.jpg", 256);
	printf("Input img:%s\n",input_imgfn);
	image im = load_image_stb(input_imgfn, 3);//3 channel img
	printf("img w=%d,h=%d,c=%d\n",im.w,im.h,im.c);
	//image sized = letterbox_image(im, 416, 416);
	image sized = letterbox_image(im, 416, 416);

	save_image_png(sized, "sized");// convert to yolov3 net input size 416x416x3
////////////////////load img resize img end

	time_t first, second;       
	
	layer l = net->layers[net->n-1];
    float *X = sized.data;

	//char line[256];
	//FILE *fp3;
	//char filename[256];
	//sprintf(filename, "yolo_layer_input_%d.txt", 123123);
	//printf("YOLO_layer:intputs=%d,%s\n",416*416*3,filename);
 //   if( (fp3 = fopen(filename, "w")) == NULL)fprintf(stderr,"CANNOT OPEN\n");
 //   for( x = 0; x < l.outputs; x++)
	//{
	//	sprintf(line, "%f\n", X[x]);
	//	if(fputs(line,fp3)<0)fprintf(stderr,"write FILE failed\n");
 //   }
 //   fclose(fp3);

	first=time(NULL); 
	yolov2_hls_ps(net, X);
	printf("yolov2_hls_ps END!\n");
	second=time(NULL); 
	printf("%s: Predicted in %f seconds.\n", input_imgfn, difftime(second,first));

    int nboxes = 0;
    float nms=.45;
	float thresh = .5;
	float hier_thresh = .5;
    detection *dets = get_network_boxes(net, im.w, im.h, thresh, hier_thresh, 0, 1, &nboxes);
    printf("%d\n", nboxes);
	printf("get_network_boxes END!\n");
	/*
	for(x=0;x<nboxes;x++)
	{
		printf("[%3d]:h=%f,w=%f,x=%f,y=%f,objectness=%f\n",x,dets[x].bbox.h,dets[x].bbox.w,dets[x].bbox.x,dets[x].bbox.y,dets[x].objectness);
	}*/
    if (nms) do_nms_sort(dets, nboxes, l.classes, nms);
    draw_detections(im, dets, nboxes, thresh, names, alphabet, l.classes);

    free_detections(dets, nboxes);
	
///////////////////write predictions img
	save_image_png(im, "predictions");// output

	free_image(im);
    free_image(sized);

	printf("YOLOv3 TEST End\n");

    return 0;
}


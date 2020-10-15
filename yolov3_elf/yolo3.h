
#ifndef SRC_YOLOV3_H_
#define SRC_YOLOV3_H_



#include <stdio.h>
#include <stdlib.h>
//#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "xconv_hw.h"
//#include "hw_drivers.h"

#define HALF_INPUT_SIZE 208
#define MINIST_NET_SIZE 13

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define FLT_MAX         3.402823466e+38F        /* max value */
#include <chrono>

inline long long getTimeStampMill(){
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}


//#define TEST

double what_time_is_it_now()
{
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}


typedef enum{
    LOGISTIC, RELU, RELIE, LINEAR, RAMP, TANH, PLSE, LEAKY, ELU, LOGGY, STAIR, HARDTAN, LHTAN
} ACTIVATION;

typedef enum {
    CONVOLUTIONAL,
    DECONVOLUTIONAL,
    CONNECTED,
    MAXPOOL,
    SOFTMAX,
    DETECTION,
    DROPOUT,
    CROP,
    ROUTE,
    COST,
    NORMALIZATION,
    AVGPOOL,
    LOCAL,
    SHORTCUT,
    ACTIVE,
    RNN,
    GRU,
    LSTM,
    CRNN,
    BATCHNORM,
    NETWORK,
    XNOR,
    REGION,
    YOLO,
    REORG,
    UPSAMPLE,
    LOGXENT,
    L2NORM,
    BLANK
} LAYER_TYPE;

struct network;
typedef struct network network;

struct layer;
typedef struct layer layer;

struct layer{
    LAYER_TYPE type;
    ACTIVATION activation;
    void (*forward)   (struct layer, struct network);
    int batch_normalize;
    int shortcut;
    int batch;
    int forced;
    int flipped;
    int inputs;
    int outputs;
    int nweights;
    int nbiases;
    int extra;
    int truths;
    int h,w,c;
    int out_h, out_w, out_c;
    int n;
    int max_boxes;
    int groups;
    int size;
    int side;
    int stride;
    int reverse;
    int flatten;
    int spatial;
    int pad;
    int sqrt;
    int flip;
    int index;
    int binary;
    int xnor;
    int steps;
    int hidden;
    int truth;
    float smooth;
    float dot;
    float angle;
    float jitter;
    float saturation;
    float exposure;
    float shift;
    float ratio;
    float learning_rate_scale;
    float clip;
    int softmax;
    int classes;
    int coords;
    int background;
    int rescore;
    int objectness;
    int joint;
    int noadjust;
    int reorg;
    int log;
    int tanh;
    int *mask;
    int total;

    float alpha;
    float beta;
    float kappa;

    float coord_scale;
    float object_scale;
    float noobject_scale;
    float mask_scale;
    float class_scale;
    int bias_match;
    int random;
    float ignore_thresh;
    float truth_thresh;
    float thresh;
    float focus;
    int classfix;
    int absolute;

    int onlyforward;
    int stopbackward;
   // int dontload;
    int dontsave;
  //  int dontloadscales;

    float temperature;
    float probability;
    float scale;

    char  * cweights;
    int   * indexes;
    int   * input_layers;
    int   * input_sizes;
    int   * map;
    float * rand;
    float * cost;
    float * state;
    float * prev_state;
    float * forgot_state;
    float * forgot_delta;
    float * state_delta;
    float * combine_cpu;
    float * combine_delta_cpu;

    float * concat;
    float * concat_delta;

    float * binary_weights;

    float * biases;
    float * bias_updates;

    float * scales;
    float * scale_updates;

    float * weights;
    float * weight_updates;

    float * delta;
    float * output;
    float * loss;
    float * squared;
    float * norms;

    float * spatial_mean;
    float * mean;
    float * variance;

    float * mean_delta;
    float * variance_delta;

    float * rolling_mean;
    float * rolling_variance;

    float * x;
    float * x_norm;

    float * m;
    float * v;

    float * bias_m;
    float * bias_v;
    float * scale_m;
    float * scale_v;


    float *z_cpu;
    float *r_cpu;
    float *h_cpu;
    float * prev_state_cpu;

    float *temp_cpu;
    float *temp2_cpu;
    float *temp3_cpu;

    float *dh_cpu;
    float *hh_cpu;
    float *prev_cell_cpu;
    float *cell_cpu;
    float *f_cpu;
    float *i_cpu;
    float *g_cpu;
    float *o_cpu;
    float *c_cpu;
    float *dc_cpu;

    float * binary_input;

    struct layer *input_layer;
    struct layer *self_layer;
    struct layer *output_layer;

    struct layer *reset_layer;
    struct layer *update_layer;
    struct layer *state_layer;

    struct layer *input_gate_layer;
    struct layer *state_gate_layer;
    struct layer *input_save_layer;
    struct layer *state_save_layer;
    struct layer *input_state_layer;
    struct layer *state_state_layer;

    struct layer *input_z_layer;
    struct layer *state_z_layer;

    struct layer *input_r_layer;
    struct layer *state_r_layer;

    struct layer *input_h_layer;
    struct layer *state_h_layer;

    struct layer *wz;
    struct layer *uz;
    struct layer *wr;
    struct layer *ur;
    struct layer *wh;
    struct layer *uh;
    struct layer *uo;
    struct layer *wo;
    struct layer *uf;
    struct layer *wf;
    struct layer *ui;
    struct layer *wi;
    struct layer *ug;
    struct layer *wg;

    //tree *softmax_tree;
    size_t workspace_size;
};

void free_layer(layer l)
{
    if(l.cweights)           free(l.cweights);
    if(l.indexes)            free(l.indexes);
    if(l.input_layers)       free(l.input_layers);
    if(l.input_sizes)        free(l.input_sizes);
    if(l.map)                free(l.map);
    if(l.rand)               free(l.rand);
    if(l.cost)               free(l.cost);
    if(l.state)              free(l.state);
    if(l.prev_state)         free(l.prev_state);
    if(l.forgot_state)       free(l.forgot_state);
    if(l.forgot_delta)       free(l.forgot_delta);
    if(l.state_delta)        free(l.state_delta);
    if(l.concat)             free(l.concat);
    if(l.concat_delta)       free(l.concat_delta);
    if(l.binary_weights)     free(l.binary_weights);
    if(l.biases)             free(l.biases);
    if(l.bias_updates)       free(l.bias_updates);
    if(l.scales)             free(l.scales);
    if(l.scale_updates)      free(l.scale_updates);
    if(l.weights)            free(l.weights);
    if(l.weight_updates)     free(l.weight_updates);
    if(l.delta)              free(l.delta);
    if(l.output)             free(l.output);
    if(l.squared)            free(l.squared);
    if(l.norms)              free(l.norms);
    if(l.spatial_mean)       free(l.spatial_mean);
    if(l.mean)               free(l.mean);
    if(l.variance)           free(l.variance);
    if(l.mean_delta)         free(l.mean_delta);
    if(l.variance_delta)     free(l.variance_delta);
    if(l.rolling_mean)       free(l.rolling_mean);
    if(l.rolling_variance)   free(l.rolling_variance);
    if(l.x)                  free(l.x);
    if(l.x_norm)             free(l.x_norm);
    if(l.m)                  free(l.m);
    if(l.v)                  free(l.v);
    if(l.z_cpu)              free(l.z_cpu);
    if(l.r_cpu)              free(l.r_cpu);
    if(l.h_cpu)              free(l.h_cpu);
    if(l.binary_input)       free(l.binary_input);
}

//void free_layer(layer);

typedef enum {
    CONSTANT, STEP, EXP, POLY, STEPS, SIG, RANDOM
} learning_rate_policy;

typedef struct network{
    int n;
    int batch;
    size_t *seen;
    int *t;
    float epoch;
    int subdivisions;
    layer *layers;
    float *output;
    learning_rate_policy policy;

    float learning_rate;
    float momentum;
    float decay;
    float gamma;
    float scale;
    float power;
    int time_steps;
    int step;
    int max_batches;
    float *scales;
    int   *steps;
    int num_steps;
    int burn_in;

    int adam;
    float B1;
    float B2;
    float eps;

    int inputs;
    int outputs;
    int truths;
    int notruth;
    int h, w, c;
    int max_crop;
    int min_crop;
    float max_ratio;
    float min_ratio;
    int center;
    float angle;
    float aspect;
    float exposure;
    float saturation;
    float hue;
    int random;

    int gpu_index;
//    tree *hierarchy;

    float *input;
    float *truth;
    float *delta;
    float *workspace;
    int train;
    int index;
    float *cost;
    float clip;
} network;

network *make_network(int n);
layer get_network_output_layer(network *net);

typedef struct {
    int w;
    int h;
    float scale;
    float rad;
    float dx;
    float dy;
    float aspect;
} augment_args;

typedef struct {
    int w;
    int h;
    int c;
    float *data;
} image;

typedef struct{
    float x, y, w, h;
} box;

typedef struct detection{
    box bbox;
    int classes;
    float *prob;
    float *mask;
    float objectness;
    int sort_class;
} detection;

typedef struct matrix{
    int rows, cols;
    float **vals;
} matrix;


typedef struct{
    int w, h;
    matrix X;
    matrix y;
    int shallow;
    int *num_boxes;
    box **boxes;
} data;

typedef enum {
    CLASSIFICATION_DATA, DETECTION_DATA, CAPTCHA_DATA, REGION_DATA, IMAGE_DATA, COMPARE_DATA, WRITING_DATA, SWAG_DATA, TAG_DATA, OLD_CLASSIFICATION_DATA, STUDY_DATA, DET_DATA, SUPER_DATA, LETTERBOX_DATA, REGRESSION_DATA, SEGMENTATION_DATA, INSTANCE_DATA
} data_type;

typedef struct load_args{
    int threads;
    char **paths;
    char *path;
    int n;
    int m;
    char **labels;
    int h;
    int w;
    int out_w;
    int out_h;
    int nh;
    int nw;
    int num_boxes;
    int min, max, size;
    int classes;
    int background;
    int scale;
    int center;
    int coords;
    float jitter;
    float angle;
    float aspect;
    float saturation;
    float exposure;
    float hue;
    data *d;
    image *im;
    image *resized;
    data_type type;
//    tree *hierarchy;
} load_args;

typedef struct{
    int id;
    float x,y,w,h;
    float left, right, top, bottom;
} box_label;

//network *load_network(char *cfg, char *weights, int clear);
//load_args get_base_args(network *net);
//void free_data(data d);

typedef struct{
    char *key;
    char *val;
    int used;
} kvp;

typedef struct node{
    void *val;
    struct node *next;
    struct node *prev;
} node;

typedef struct list{
    int size;
    node *front;
    node *back;
} list;

void error(const char *s)
{
    perror(s);
    assert(0);
    exit(-1);
}

void malloc_error()
{
    fprintf(stderr, "Malloc error\n");
    exit(-1);
}

void file_error(char *s)
{
    fprintf(stderr, "Couldn't open file: %s\n", s);
    exit(0);
}

/////////////////list begin
list *make_list()
{
	list *l = (list *)malloc(sizeof(list));
	l->size = 0;
	l->front = 0;
	l->back = 0;
	return l;
}


void *list_pop(list *l){
    if(!l->back) return 0;
    node *b = l->back;
    void *val = b->val;
    l->back = b->prev;
    if(l->back) l->back->next = 0;
    free(b);
    --l->size;

    return val;
}

void list_insert(list *l, void *val)
{
	node *new_node = (node *)malloc(sizeof(node));
	new_node->val = val;
	new_node->next = 0;

	if(!l->back){
		l->front = new_node;
		new_node->prev = 0;
	}else{
		l->back->next = new_node;
		new_node->prev = l->back;
	}
	l->back = new_node;
	++l->size;
}

void free_node(node *n)
{
	node *next;
	while(n) {
		next = n->next;
		free(n);
		n = next;
	}
}

void free_list(list *l)
{
	free_node(l->front);
	free(l);
}

void free_list_contents(list *l)
{
	node *n = l->front;
	while(n){
		free(n->val);
		n = n->next;
	}
}

void **list_to_array(list *l)
{
    void **a = (void **)calloc(l->size, sizeof(void*));
    int count = 0;
    node *n = l->front;
    while(n){
        a[count++] = n->val;
        n = n->next;
    }
    return a;
}
/////////////////list end

/////////////////////utils begin
void del_arg(int argc, char **argv, int index)
{
    int i;
    for(i = index; i < argc-1; ++i) argv[i] = argv[i+1];
    argv[i] = 0;
}

int find_arg(int argc, char* argv[], char *arg)
{
    int i;
    for(i = 0; i < argc; ++i) {
        if(!argv[i]) continue;
        if(0==strcmp(argv[i], arg)) {
            del_arg(argc, argv, i);
            return 1;
        }
    }
    return 0;
}

int find_int_arg(int argc, char **argv, char *arg, int def)
{
    int i;
    for(i = 0; i < argc-1; ++i){
        if(!argv[i]) continue;
        if(0==strcmp(argv[i], arg)){
            def = atoi(argv[i+1]);
            del_arg(argc, argv, i);
            del_arg(argc, argv, i);
            break;
        }
    }
    return def;
}

float find_float_arg(int argc, char **argv, char *arg, float def)
{
    int i;
    for(i = 0; i < argc-1; ++i){
        if(!argv[i]) continue;
        if(0==strcmp(argv[i], arg)){
            def = atof(argv[i+1]);
            del_arg(argc, argv, i);
            del_arg(argc, argv, i);
            break;
        }
    }
    return def;
}

char *find_char_arg(int argc, char **argv, char *arg, char *def)
{
    int i;
    for(i = 0; i < argc-1; ++i){
        if(!argv[i]) continue;
        if(0==strcmp(argv[i], arg)){
            def = argv[i+1];
            del_arg(argc, argv, i);
            del_arg(argc, argv, i);
            break;
        }
    }
    return def;
}

unsigned char *read_file(char *filename)
{
    FILE *fp = fopen(filename, "rb");
    size_t size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char *text = (unsigned char *)calloc(size+1, sizeof(unsigned char));
    fread(text, 1, size, fp);
    fclose(fp);
    return text;
}

list *split_str(char *s, char delim)
{
    size_t i;
    size_t len = strlen(s);
    list *l = make_list();
    list_insert(l, s);
    for(i = 0; i < len; ++i){
        if(s[i] == delim){
            s[i] = '\0';
            list_insert(l, &(s[i+1]));
        }
    }
    return l;
}

void strip(char *s)
{
    size_t i;
    size_t len = strlen(s);
    size_t offset = 0;
    for(i = 0; i < len; ++i){
        char c = s[i];
        if(c==' '||c=='\t'||c=='\n') ++offset;
        else s[i-offset] = c;
    }
    s[len-offset] = '\0';
}

void strip_char(char *s, char bad)
{
    size_t i;
    size_t len = strlen(s);
    size_t offset = 0;
    for(i = 0; i < len; ++i){
        char c = s[i];
        if(c==bad) ++offset;
        else s[i-offset] = c;
    }
    s[len-offset] = '\0';
}

void free_ptrs(void **ptrs, int n)
{
    int i;
    for(i = 0; i < n; ++i) free(ptrs[i]);
    free(ptrs);
}

char *fgetl(FILE *fp)
{
    if(feof(fp)) return 0;
    size_t size = 512;
    char *line = (char *)malloc(size*sizeof(char));
    if(!fgets(line, size, fp)){
        free(line);
        return 0;
    }

    size_t curr = strlen(line);

    while((line[curr-1] != '\n') && !feof(fp)){
        if(curr == size-1){
            size *= 2;
            line = (char *)realloc(line, size*sizeof(char));
            if(!line) {
                printf("%ld\n", size);
                malloc_error();
            }
        }
        size_t readsize = size-curr;
        if(readsize > INT_MAX) readsize = INT_MAX-1;
        fgets(&line[curr], readsize, fp);
        curr = strlen(line);
    }
    if(line[curr-1] == '\n') line[curr-1] = '\0';

    return line;
}
/////////////////////utils end

////////////////////option_list begin

void option_insert(list *l, char *key, char *val)
{
    kvp *p = (kvp *)malloc(sizeof(kvp));
    p->key = key;
    p->val = val;
    p->used = 0;
    list_insert(l, p);
}

int read_option(char *s, list *options)
{
    size_t i;
    size_t len = strlen(s);
    char *val = 0;
    for(i = 0; i < len; ++i){
        if(s[i] == '='){
            s[i] = '\0';
            val = s+i+1;
            break;
        }
    }
    if(i == len-1) return 0;
    char *key = s;
    option_insert(options, key, val);
    return 1;
}

void option_unused(list *l)
{
    node *n = l->front;
    while(n){
        kvp *p = (kvp *)n->val;
        if(!p->used){
            fprintf(stderr, "Unused field: '%s = %s'\n", p->key, p->val);
        }
        n = n->next;
    }
}

char *option_find(list *l, char *key)
{
    node *n = l->front;
    while(n){
        kvp *p = (kvp *)n->val;
        if(strcmp(p->key, key) == 0){
            p->used = 1;
            return p->val;
        }
        n = n->next;
    }
    return 0;
}
char *option_find_str(list *l, char *key, char *def)
{
    char *v = option_find(l, key);
    if(v) return v;
    if(def) fprintf(stderr, "%s: Using default '%s'\n", key, def);
    return def;
}

int option_find_int(list *l, char *key, int def)
{
    char *v = option_find(l, key);
    if(v) return atoi(v);
    fprintf(stderr, "%s: Using default '%d'\n", key, def);
    return def;
}

int option_find_int_quiet(list *l, char *key, int def)
{
    char *v = option_find(l, key);
    if(v) return atoi(v);
    return def;
}

float option_find_float_quiet(list *l, char *key, float def)
{
    char *v = option_find(l, key);
    if(v) return atof(v);
    return def;
}

float option_find_float(list *l, char *key, float def)
{
    char *v = option_find(l, key);
    if(v) return atof(v);
    fprintf(stderr, "%s: Using default '%lf'\n", key, def);
    return def;
}

list *read_data_cfg(char *filename)
{
    FILE *file = fopen(filename, "r");
    if(file == 0) file_error(filename);
    char *line;
    int nu = 0;
    list *options = make_list();
    while((line=fgetl(file)) != 0){
        ++ nu;
        strip(line);
        switch(line[0]){
            case '\0':
            case '#':
            case ';':
                free(line);
                break;
            default:
                if(!read_option(line, options)){
                    fprintf(stderr, "Config file error line %d, could parse: %s\n", nu, line);
                    free(line);
                }
                break;
        }
    }
    fclose(file);
    return options;
}
///////////////////option_list end
image make_empty_image(int w, int h, int c)
{
    image out;
    out.data = 0;
    out.h = h;
    out.w = w;
    out.c = c;
    return out;
}

list *get_paths(char *filename)
{
    char *path;
    FILE *file = fopen(filename, "r");
    if(!file) file_error(filename);
    list *lines = make_list();
    while((path=fgetl(file))){
        list_insert(lines, path);
    }
    fclose(file);
    return lines;
}

char **get_labels(char *filename)
{
    list *plist = get_paths(filename);
    char **labels = (char **)list_to_array(plist);
    free_list(plist);
    return labels;
}


image make_image(int w, int h, int c)
{
    image out = make_empty_image(w,h,c);
    out.data = (float *)calloc(h*w*c, sizeof(float));
    return out;
}

static float get_pixel(image m, int x, int y, int c)
{
    assert(x < m.w && y < m.h && c < m.c);
    return m.data[c*m.h*m.w + y*m.w + x];
}

static void set_pixel(image m, int x, int y, int c, float val)
{
    if (x < 0 || y < 0 || c < 0 || x >= m.w || y >= m.h || c >= m.c) return;
    assert(x < m.w && y < m.h && c < m.c);
    m.data[c*m.h*m.w + y*m.w + x] = val;
}

static void add_pixel(image m, int x, int y, int c, float val)
{
    assert(x < m.w && y < m.h && c < m.c);
    m.data[c*m.h*m.w + y*m.w + x] += val;
}

void free_image(image m)
{
    if(m.data){
        free(m.data);
    }
}

image resize_image(image im, int w, int h)
{
    image resized = make_image(w, h, im.c);
    image part = make_image(w, im.h, im.c);
    int r, c, k;
    float w_scale = (float)(im.w - 1) / (w - 1);
    float h_scale = (float)(im.h - 1) / (h - 1);
    for(k = 0; k < im.c; ++k){
        for(r = 0; r < im.h; ++r){
            for(c = 0; c < w; ++c){
                float val = 0;
                if(c == w-1 || im.w == 1){
                    val = get_pixel(im, im.w-1, r, k);
                } else {
                    float sx = c*w_scale;
                    int ix = (int) sx;
                    float dx = sx - ix;
                    val = (1 - dx) * get_pixel(im, ix, r, k) + dx * get_pixel(im, ix+1, r, k);
                }
                set_pixel(part, c, r, k, val);
            }
        }
    }

    for(k = 0; k < im.c; ++k){
        for(r = 0; r < h; ++r){
            float sy = r*h_scale;
            int iy = (int) sy;
            float dy = sy - iy;
            for(c = 0; c < w; ++c){
                float val = (1-dy) * get_pixel(part, c, iy, k);
                set_pixel(resized, c, r, k, val);
            }
            if(r == h-1 || im.h == 1) continue;
            for(c = 0; c < w; ++c){
                float val = dy * get_pixel(part, c, iy+1, k);
                add_pixel(resized, c, r, k, val);
            }
        }
    }

    free_image(part);
    return resized;
}

void fill_image(image m, float s)
{
    int i;
    for(i = 0; i < m.h*m.w*m.c; ++i) m.data[i] = s;
}

void embed_image(image source, image dest, int dx, int dy)
{
    int x,y,k;
    for(k = 0; k < source.c; ++k){
        for(y = 0; y < source.h; ++y){
            for(x = 0; x < source.w; ++x){
                float val = get_pixel(source, x,y,k);
                set_pixel(dest, dx+x, dy+y, k, val);
            }
        }
    }
}

image letterbox_image(image im, int w, int h)
{
    int new_w = im.w;
    int new_h = im.h;
    if (((float)w/im.w) < ((float)h/im.h)) {
        new_w = w;
        new_h = (im.h * w)/im.w;
    } else {
        new_h = h;
        new_w = (im.w * h)/im.h;
    }
    image resized = resize_image(im, new_w, new_h);
    image boxed = make_image(w, h, im.c);
    fill_image(boxed, .5);
    //int i;
    //for(i = 0; i < boxed.w*boxed.h*boxed.c; ++i) boxed.data[i] = 0;
    embed_image(resized, boxed, (w-new_w)/2, (h-new_h)/2);
    free_image(resized);
    return boxed;
}

image load_image_stb(char *filename, int channels)
{
    int w, h, c;
    unsigned char *data = stbi_load(filename, &w, &h, &c, channels);
    if (!data) {
        fprintf(stderr, "Cannot load image \"%s\"\nSTB Reason: %s\n", filename, stbi_failure_reason());
        exit(0);
    }
    if(channels) c = channels;
    int i,j,k;
    image im = make_image(w, h, c);
    for(k = 0; k < c; ++k){
        for(j = 0; j < h; ++j){
            for(i = 0; i < w; ++i){
                int dst_index = i + w*j + w*h*k;
                int src_index = k + c*i + c*w*j;
                im.data[dst_index] = (float)data[src_index]/255.;
            }
        }
    }
    free(data);
    return im;
}

void save_image_png(image im, const char *name)
{
    char buff[256];
    //sprintf(buff, "%s (%d)", name, windows);
    sprintf(buff, "%s.png", name);
	unsigned char *data = (unsigned char *)calloc(im.w*im.h*im.c, sizeof(char));
    int i,k;
    for(k = 0; k < im.c; ++k){
        for(i = 0; i < im.w*im.h; ++i){
            data[i*im.c+k] = (unsigned char) (255*im.data[i + k*im.w*im.h]);
        }
    }
    int success = stbi_write_png(buff, im.w, im.h, im.c, data, im.w*im.c);
    free(data);
    if(!success) fprintf(stderr, "Failed to write image %s\n", buff);
}

image **load_alphabet()
{
    int i, j;
    const int nsize = 8;
    image **alphabets = (image **)calloc(nsize, sizeof(image));
    for(j = 0; j < nsize; ++j){
        alphabets[j] = (image *)calloc(128, sizeof(image));
        for(i = 32; i < 127; ++i){
            char buff[256];
            sprintf(buff, "labels/%d_%d.png", i, j);
            //alphabets[j][i] = load_image_color(buff, 0, 0);
			alphabets[j][i] = load_image_stb(buff, 3);
        }
    }
    return alphabets;
}
///////////////////activation begin

static inline float stair_activate(float x)
{
    int n = floor(x);
    if (n%2 == 0) return floor(x/2.);
    else return (x - n) + floor(x/2.);
}
static inline float hardtan_activate(float x)
{
    if (x < -1) return -1;
    if (x > 1) return 1;
    return x;
}
static inline float linear_activate(float x){return x;}
static inline float logistic_activate(float x){return 1./(1. + exp(-x));}
static inline float loggy_activate(float x){return 2./(1. + exp(-x)) - 1;}
static inline float relu_activate(float x){return x*(x>0);}
static inline float elu_activate(float x){return (x >= 0)*x + (x < 0)*(exp(x)-1);}
static inline float relie_activate(float x){return (x>0) ? x : .01*x;}
static inline float ramp_activate(float x){return x*(x>0)+.1*x;}
static inline float leaky_activate(float x){return (x>0) ? x : .1*x;}
static inline float tanh_activate(float x){return (exp(2*x)-1)/(exp(2*x)+1);}
static inline float plse_activate(float x)
{
    if(x < -4) return .01 * (x + 4);
    if(x > 4)  return .01 * (x - 4) + 1;
    return .125*x + .5;
}

static inline float lhtan_activate(float x)
{
    if(x < 0) return .001*x;
    if(x > 1) return .001*(x-1) + 1;
    return x;
}
static inline float lhtan_gradient(float x)
{
    if(x > 0 && x < 1) return 1;
    return .001;
}

static inline float hardtan_gradient(float x)
{
    if (x > -1 && x < 1) return 1;
    return 0;
}
static inline float linear_gradient(float x){return 1;}
static inline float logistic_gradient(float x){return (1-x)*x;}
static inline float loggy_gradient(float x)
{
    float y = (x+1.)/2.;
    return 2*(1-y)*y;
}
static inline float stair_gradient(float x)
{
    if (floor(x) == x) return 0;
    return 1;
}
static inline float relu_gradient(float x){return (x>0);}
static inline float elu_gradient(float x){return (x >= 0) + (x < 0)*(x + 1);}
static inline float relie_gradient(float x){return (x>0) ? 1 : .01;}
static inline float ramp_gradient(float x){return (x>0)+.1;}
static inline float leaky_gradient(float x){return (x>0) ? 1 : .1;}
static inline float tanh_gradient(float x){return 1-x*x;}
static inline float plse_gradient(float x){return (x < 0 || x > 1) ? .01 : .125;}

char *get_activation_string(ACTIVATION a)
{
    switch(a){
        case LOGISTIC:
            return (char*)"logistic";
        case LOGGY:
            return (char*)"loggy";
        case RELU:
            return (char*)"relu";
        case ELU:
            return (char*)"elu";
        case RELIE:
            return (char*)"relie";
        case RAMP:
            return (char*)"ramp";
        case LINEAR:
            return (char*)"linear";
        case TANH:
            return (char*)"tanh";
        case PLSE:
            return (char*)"plse";
        case LEAKY:
            return (char*)"leaky";
        case STAIR:
            return (char*)"stair";
        case HARDTAN:
            return (char*)"hardtan";
        case LHTAN:
            return (char*)"lhtan";
        default:
            break;
    }
    return (char*)"relu";
}

ACTIVATION get_activation(char *s)
{
    if (strcmp(s, "logistic")==0) return LOGISTIC;
    if (strcmp(s, "loggy")==0) return LOGGY;
    if (strcmp(s, "relu")==0) return RELU;
    if (strcmp(s, "elu")==0) return ELU;
    if (strcmp(s, "relie")==0) return RELIE;
    if (strcmp(s, "plse")==0) return PLSE;
    if (strcmp(s, "hardtan")==0) return HARDTAN;
    if (strcmp(s, "lhtan")==0) return LHTAN;
    if (strcmp(s, "linear")==0) return LINEAR;
    if (strcmp(s, "ramp")==0) return RAMP;
    if (strcmp(s, "leaky")==0) return LEAKY;
    if (strcmp(s, "tanh")==0) return TANH;
    if (strcmp(s, "stair")==0) return STAIR;
    fprintf(stderr, "Couldn't find activation function %s, going with ReLU\n", s);
    return RELU;
}

float activate(float x, ACTIVATION a)
{
    switch(a){
        case LINEAR:
            return linear_activate(x);
        case LOGISTIC:
            return logistic_activate(x);
        case LOGGY:
            return loggy_activate(x);
        case RELU:
            return relu_activate(x);
        case ELU:
            return elu_activate(x);
        case RELIE:
            return relie_activate(x);
        case RAMP:
            return ramp_activate(x);
        case LEAKY:
            return leaky_activate(x);
        case TANH:
            return tanh_activate(x);
        case PLSE:
            return plse_activate(x);
        case STAIR:
            return stair_activate(x);
        case HARDTAN:
            return hardtan_activate(x);
        case LHTAN:
            return lhtan_activate(x);
    }
    return 0;
}

void activate_array(float *x, const int n, const ACTIVATION a)
{
    int i;
    for(i = 0; i < n; ++i){
        x[i] = activate(x[i], a);
    }
}

float gradient(float x, ACTIVATION a)
{
    switch(a){
        case LINEAR:
            return linear_gradient(x);
        case LOGISTIC:
            return logistic_gradient(x);
        case LOGGY:
            return loggy_gradient(x);
        case RELU:
            return relu_gradient(x);
        case ELU:
            return elu_gradient(x);
        case RELIE:
            return relie_gradient(x);
        case RAMP:
            return ramp_gradient(x);
        case LEAKY:
            return leaky_gradient(x);
        case TANH:
            return tanh_gradient(x);
        case PLSE:
            return plse_gradient(x);
        case STAIR:
            return stair_gradient(x);
        case HARDTAN:
            return hardtan_gradient(x);
        case LHTAN:
            return lhtan_gradient(x);
    }
    return 0;
}

///////////////////activation end

void copy_cpu(int N, float *X, int INCX, float *Y, int INCY)
{
    int i;
    for(i = 0; i < N; ++i) Y[i*INCY] = X[i*INCX];
}

void fill_cpu(int N, float ALPHA, float *X, int INCX)
{
    int i;
    for(i = 0; i < N; ++i) X[i*INCX] = ALPHA;
}

void shortcut_cpu(int batch, int w1, int h1, int c1, float *add, int w2, int h2, int c2, float s1, float s2, float *out)
{
    int stride = w1/w2;
    int sample = w2/w1;
    assert(stride == h1/h2);
    assert(sample == h2/h1);
	//printf("shorcut_layer batch=%d,stride=%d,sample=%d\n",batch,stride,sample);
	if(stride < 1) stride = 1;
    if(sample < 1) sample = 1;
    int minw = (w1 < w2) ? w1 : w2;
    int minh = (h1 < h2) ? h1 : h2;
    int minc = (c1 < c2) ? c1 : c2;

    int i,j,k,b;
    for(b = 0; b < batch; ++b){
        for(k = 0; k < minc; ++k){
            for(j = 0; j < minh; ++j){
                for(i = 0; i < minw; ++i){
                    int out_index = i*sample + w2*(j*sample + h2*(k + c2*b));
                    int add_index = i*stride + w1*(j*stride + h1*(k + c1*b));
                    out[out_index] = s1*out[out_index] + s2*add[add_index];
                }
            }
        }
    }
}

void forward_shortcut_layer(const layer l, network net)
{
    //copy_cpu(l.outputs*l.batch, net.input, 1, l.output, 1);
    //shortcut_cpu(l.batch, l.w, l.h, l.c, net.layers[l.index].output, l.out_w, l.out_h, l.out_c, l.alpha, l.beta, l.output);
    //activate_array(l.output, l.outputs*l.batch, l.activation);

    int w = l.w;
    int h = l.h;
    int c = l.c;
	float *add = net.layers[l.index].output;
	float *out = l.output;
	float *in = net.input;
	printf("forward_shortcut_layer begin!\n");
	//printf("out[0] = %f",out[0]);
	printf("in[0] = %f",in[0]);
	printf("add[0] = %f",add[0]);

    int i,j,k;
    for(k = 0; k < c; ++k){
        for(j = 0; j < h; ++j){
            for(i = 0; i < w; ++i){
                int index = i + w*(j + h*k );
                out[index] = in[index] + add[index];
            }
        }
    }

}

layer make_shortcut_layer(int batch, int index, int w, int h, int c, int w2, int h2, int c2)
{
    fprintf(stderr, "res  %3d                %4d x%4d x%4d   ->  %4d x%4d x%4d\n",index, w2,h2,c2, w,h,c);
    layer l;
	memset(&l,0,sizeof(layer));

    l.type = SHORTCUT;
    l.batch = batch;
    l.w = w2;
    l.h = h2;
    l.c = c2;
    l.out_w = w;
    l.out_h = h;
    l.out_c = c;
    l.outputs = w*h*c;
    l.inputs = l.outputs;

    l.index = index;

    l.output = (float *)calloc(l.outputs*batch, sizeof(float));;

    l.forward = forward_shortcut_layer;

    return l;
}

int convolutional_out_height(layer l)
{
    return (l.h + 2*l.pad - l.size) / l.stride + 1;
}

int convolutional_out_width(layer l)
{
    return (l.w + 2*l.pad - l.size) / l.stride + 1;
}

static size_t get_workspace_size(layer l){

    return (size_t)l.out_h*l.out_w*l.size*l.size*l.c/l.groups*sizeof(float);
}

void add_bias(float *output, float *biases, int batch, int n, int size)
{
    int i,j,b;
    for(b = 0; b < batch; ++b){
        for(i = 0; i < n; ++i){
            for(j = 0; j < size; ++j){
                output[(b*n + i)*size + j] += biases[i];
            }
        }
    }
}

void scale_bias(float *output, float *scales, int batch, int n, int size)
{
    int i,j,b;
    for(b = 0; b < batch; ++b){
        for(i = 0; i < n; ++i){
            for(j = 0; j < size; ++j){
                output[(b*n + i)*size + j] *= scales[i];
            }
        }
    }
}

float im2col_get_pixel(float *im, int height, int width, int channels,
                        int row, int col, int channel, int pad)
{
    row -= pad;
    col -= pad;

    if (row < 0 || col < 0 ||
        row >= height || col >= width) return 0;
    return im[col + width*(row + height*channel)];
}

//From Berkeley Vision's Caffe!
//https://github.com/BVLC/caffe/blob/master/LICENSE
void im2col_cpu(float* data_im,
     int channels,  int height,  int width,
     int ksize,  int stride, int pad, float* data_col)
{
    int c,h,w;
    int height_col = (height + 2*pad - ksize) / stride + 1;
    int width_col = (width + 2*pad - ksize) / stride + 1;

    int channels_col = channels * ksize * ksize;
    for (c = 0; c < channels_col; ++c) {
        int w_offset = c % ksize;
        int h_offset = (c / ksize) % ksize;
        int c_im = c / ksize / ksize;
        for (h = 0; h < height_col; ++h) {
            for (w = 0; w < width_col; ++w) {
                int im_row = h_offset + h * stride;
                int im_col = w_offset + w * stride;
                int col_index = (c * height_col + h) * width_col + w;
                data_col[col_index] = im2col_get_pixel(data_im, height, width, channels,
                        im_row, im_col, c_im, pad);
            }
        }
    }
}

void gemm_nn(int M, int N, int K, float ALPHA,
        float *A, int lda,
        float *B, int ldb,
        float *C, int ldc)
{
    int i,j,k;
    #pragma omp parallel for
    for(i = 0; i < M; ++i){
        for(k = 0; k < K; ++k){
            register float A_PART = ALPHA*A[i*lda+k];
            for(j = 0; j < N; ++j){
                C[i*ldc+j] += A_PART*B[k*ldb+j];
            }
        }
    }
}

void gemm_cpu(int TA, int TB, int M, int N, int K, float ALPHA,
        float *A, int lda,
        float *B, int ldb,
        float BETA,
        float *C, int ldc)
{
    //printf("cpu: %d %d %d %d %d %f %d %d %f %d\n",TA, TB, M, N, K, ALPHA, lda, ldb, BETA, ldc);
    int i, j;
    for(i = 0; i < M; ++i){
        for(j = 0; j < N; ++j){
            C[i*ldc + j] *= BETA;
        }
    }
    if(!TA && !TB)
        gemm_nn(M, N, K, ALPHA,A,lda, B, ldb,C,ldc);
    //else if(TA && !TB)
    //    gemm_tn(M, N, K, ALPHA,A,lda, B, ldb,C,ldc);
    //else if(!TA && TB)
    //    gemm_nt(M, N, K, ALPHA,A,lda, B, ldb,C,ldc);
    //else
    //    gemm_tt(M, N, K, ALPHA,A,lda, B, ldb,C,ldc);
}

void gemm(int TA, int TB, int M, int N, int K, float ALPHA,
        float *A, int lda,
        float *B, int ldb,
        float BETA,
        float *C, int ldc)
{
    gemm_cpu( TA,  TB,  M, N, K, ALPHA,A,lda, B, ldb,BETA,C,ldc);
}

void normalize_cpu(float *x, float *mean, float *variance, int batch, int filters, int spatial)
{
    int b, f, i;
    for(b = 0; b < batch; ++b){
        for(f = 0; f < filters; ++f){
            for(i = 0; i < spatial; ++i){
                int index = b*filters*spatial + f*spatial + i;
                x[index] = (x[index] - mean[f])/(sqrt(variance[f]) + .000001f);
            }
        }
    }
}

void forward_batchnorm_layer(layer l, network net)//for conv
{
    normalize_cpu(l.output, l.rolling_mean, l.rolling_variance, l.batch, l.out_c, l.out_h*l.out_w);
    scale_bias(l.output, l.scales, l.batch, l.out_c, l.out_h*l.out_w);
    add_bias(l.output, l.biases, l.batch, l.out_c, l.out_h*l.out_w);
}

void CONV_Padding_Relu(float *Input,float *Output,float *Weight,const int InFM_num,const int OutFM_num,const int Kernel_size,const int Kernel_stride,const int Input_w,const int Input_h,const int Padding)
{
	// (output_w - 1)*Kernel_stride + Kernel_size = Input_w
	const int output_w = (Input_w - Kernel_size + 2*Padding)/Kernel_stride + 1 ;
	const int output_h = (Input_h - Kernel_size + 2*Padding)/Kernel_stride + 1 ;
	int x, y, of, inf;
	int m,n;
    for( of = 0; of < OutFM_num; of++){
    	for( y = 0; y < output_h; y++) {
    		for( x = 0; x < output_w; x++){
				float tmp = 0.0;
				for(inf = 0;inf < InFM_num; inf++)
				{
					int intput_offset = inf*Input_w*Input_h + (y*Kernel_stride - Padding)*Input_w + x*Kernel_stride - Padding;
					for(m = 0;m < Kernel_size; m++)
					{
    					for(n = 0;n < Kernel_size; n++)
						{
							int kernel_offset = of*InFM_num*Kernel_size*Kernel_size + inf*Kernel_size*Kernel_size;
							bool inFM_width  = ((x*Kernel_stride + n - Padding) >= 0)&&((x*Kernel_stride + n - Padding) < Input_w);
							bool inFM_height = ((y*Kernel_stride + m - Padding) >= 0)&&((y*Kernel_stride + m - Padding) < Input_h);
							if(inFM_width&&inFM_height)
								tmp += Weight[kernel_offset + m*Kernel_size + n]*Input[intput_offset + m*Input_w + n];
    					}
    				}
				}
    			Output[of*output_w*output_h + y*output_w + x] = tmp;
    		}
    	}
    }
}

void forward_convolutional_layer(layer l, network net)
{
    int i, j;

    fill_cpu(l.outputs*l.batch, 0, l.output, 1);

	//printf("c=%d,n=%d,size=%d,stride=%d,w=%d,h=%d,pad=%d\n",l.c,l.n,l.size,l.stride,l.w,l.h,l.pad);

    //int m = l.n/l.groups;
    //int k = l.size*l.size*l.c/l.groups;
    //int n = l.out_w*l.out_h;
    //for(i = 0; i < l.batch; ++i){
    //    for(j = 0; j < l.groups; ++j){
    //        float *a = l.weights + j*l.nweights/l.groups;
    //        float *b = net.workspace;
    //        float *c = l.output + (i*l.groups + j)*n*m;

    //        im2col_cpu(net.input + (i*l.groups + j)*l.c/l.groups*l.h*l.w,
    //            l.c/l.groups, l.h, l.w, l.size, l.stride, l.pad, b);
    //        gemm(0,0,m,n,k,1,a,k,b,n,1,c,n);
    //    }
    //}

    int m = l.n;
    int k = l.size*l.size*l.c;
    int n = l.out_w*l.out_h;
    float *a = l.weights;
    float *b = net.workspace;
    float *c = l.output;

    im2col_cpu(net.input,l.c, l.h, l.w, l.size, l.stride, l.pad, b);
    gemm(0,0,m,n,k,1,a,k,b,n,1,c,n);

    //CONV_Padding_Relu(net.input,l.output,l.weights,l.c,l.n,l.size,l.stride,l.w,l.h,l.pad);

    if(l.batch_normalize){
        forward_batchnorm_layer(l, net);
    } else {
        add_bias(l.output, l.biases, l.batch, l.n, l.out_h*l.out_w);
    }

    activate_array(l.output, l.outputs*l.batch, l.activation);
}

layer make_convolutional_layer(int batch, int h, int w, int c, int n, int groups, int size, int stride, int padding, ACTIVATION activation, int batch_normalize, int binary, int xnor, int adam)
{
    int i;
	layer l;
    memset(&l,0,sizeof(layer));
	l.type = CONVOLUTIONAL;

    l.groups = groups;
    l.h = h;
    l.w = w;
    l.c = c;
    l.n = n;
    l.binary = binary;
    l.xnor = xnor;
    l.batch = batch;
    l.stride = stride;
    l.size = size;
    l.pad = padding;
    l.batch_normalize = batch_normalize;

 //   l.weights = (float *)calloc(c/groups*n*size*size, sizeof(float));
  //  l.biases = (float *)calloc(n, sizeof(float));

    l.nweights = c/groups*n*size*size;
    l.nbiases = n;

	int out_w = convolutional_out_width(l);
    int out_h = convolutional_out_height(l);
    l.out_h = out_h;
    l.out_w = out_w;
    l.out_c = n;
    l.outputs = l.out_h * l.out_w * l.out_c;
    l.inputs = l.w * l.h * l.c;

   // l.output = (float *)calloc(l.batch*l.outputs, sizeof(float));
    l.forward = forward_convolutional_layer;

    if(batch_normalize){
    //    l.scales = (float *)calloc(n, sizeof(float));
    //   l.rolling_mean = (float *)calloc(n, sizeof(float));
//l.rolling_variance = (float *)calloc(n, sizeof(float));
    }

    l.workspace_size = get_workspace_size(l);
    l.activation = activation;

    fprintf(stderr, "conv  %5d %2d x%2d /%2d  %4d x%4d x%4d   ->  %4d x%4d x%4d  %5.3f BFLOPs\n", n, size, size, stride, w, h, c, l.out_w, l.out_h, l.out_c, (2.0 * l.n * l.size*l.size*l.c/l.groups * l.out_h*l.out_w)/1000000000.);

    return l;
}

void upsample_cpu(float *in, int w, int h, int c, int batch, int stride, int forward, float scale, float *out)
{
    int i, j, k, b;
    for(b = 0; b < batch; ++b){
        for(k = 0; k < c; ++k){
            for(j = 0; j < h*stride; ++j){
                for(i = 0; i < w*stride; ++i){
                    int in_index = b*w*h*c + k*w*h + (j/stride)*w + i/stride;
                    int out_index = b*w*h*c*stride*stride + k*w*h*stride*stride + j*w*stride + i;
                    if(forward) out[out_index] = scale*in[in_index];
                    else in[in_index] += scale*out[out_index];
                }
            }
        }
    }
}

void forward_upsample_layer(const layer l, network net)
{
    //fill_cpu(l.outputs*l.batch, 0, l.output, 1);
    //upsample_cpu(net.input, l.w, l.h, l.c, l.batch, l.stride, 1, l.scale, l.output);

	int c = l.c;
	int h = l.h;
	int w = l.w;
	int stride = l.stride;
	float *in = net.input;
	float *out = l.output;

    int i, j, k;

    for(k = 0; k < c; ++k){
        for(j = 0; j < h*stride; ++j){
            for(i = 0; i < w*stride; ++i){
                int in_index = k*w*h + (j/stride)*w + i/stride;
                int out_index = k*w*h*stride*stride + j*w*stride + i;
                out[out_index] = in[in_index];

            }
        }
    }

}

layer make_upsample_layer(int batch, int w, int h, int c, int stride)
{
    layer l;
	memset(&l,0,sizeof(layer));

    l.type = UPSAMPLE;
    l.batch = batch;
    l.w = w;
    l.h = h;
    l.c = c;
    l.out_w = w*stride;
    l.out_h = h*stride;
    l.out_c = c;
    if(stride < 0){
        stride = -stride;
        l.reverse=1;
        l.out_w = w/stride;
        l.out_h = h/stride;
    }
    l.stride = stride;
    l.outputs = l.out_w*l.out_h*l.out_c;
    l.inputs = l.w*l.h*l.c;
    l.output = (float *)calloc(l.outputs*batch, sizeof(float));;

    l.forward = forward_upsample_layer;

    if(l.reverse) fprintf(stderr, "downsample         %2dx  %4d x%4d x%4d   ->  %4d x%4d x%4d\n", stride, w, h, c, l.out_w, l.out_h, l.out_c);
    else fprintf(stderr, "upsample           %2dx  %4d x%4d x%4d   ->  %4d x%4d x%4d\n", stride, w, h, c, l.out_w, l.out_h, l.out_c);
    return l;
}

void forward_route_layer(const layer l, network net)
{
    int i, j;
    int offset = 0;
    for(i = 0; i < l.n; ++i){
        int index = l.input_layers[i];
        float *input = net.layers[index].output;
        int input_size = l.input_sizes[i];
		copy_cpu(input_size, input, 1, l.output + offset, 1);
        offset += input_size;
    }
}

layer make_route_layer(int batch, int n, int *input_layers, int *input_sizes)
{
    fprintf(stderr,"route ");
    layer l;
	memset(&l,0,sizeof(layer));

    l.type = ROUTE;
    l.batch = batch;
    l.n = n;
    l.input_layers = input_layers;
    l.input_sizes = input_sizes;
    int i;
    int outputs = 0;
    for(i = 0; i < n; ++i){
        fprintf(stderr," %d", input_layers[i]);
        outputs += input_sizes[i];
    }
    fprintf(stderr, "\n");
    l.outputs = outputs;
    l.inputs = outputs;

  //  l.output = (float *)calloc(outputs*batch, sizeof(float));;
    l.forward = forward_route_layer;

    return l;
}

static int entry_index(layer l, int batch, int location, int entry)
{
    int n =   location / (l.w*l.h);
    int loc = location % (l.w*l.h);
    return batch*l.outputs + n*l.w*l.h*(4+l.classes+1) + entry*l.w*l.h + loc;
}

void forward_yolo_layer(const layer l, network net)
{
    int i,j,b,t,n;
    memcpy(l.output, net.input, l.outputs*l.batch*sizeof(float));

    for (b = 0; b < l.batch; ++b){
        for(n = 0; n < l.n; ++n){
            int index = entry_index(l, b, n*l.w*l.h, 0);
            activate_array(l.output + index, 2*l.w*l.h, LOGISTIC);
            index = entry_index(l, b, n*l.w*l.h, 4);
            activate_array(l.output + index, (1+l.classes)*l.w*l.h, LOGISTIC);
        }
    }

	return ;

}

float* forward_yolo_array(const layer l, float* input){
	int i,j,b,t,n;
	
	memcpy(l.output, input, l.outputs*l.batch*sizeof(float));

    for (b = 0; b < l.batch; ++b){
        for(n = 0; n < l.n; ++n){
            int index = entry_index(l, b, n*l.w*l.h, 0);
            activate_array(l.output + index, 2*l.w*l.h, LOGISTIC);
            index = entry_index(l, b, n*l.w*l.h, 4);
            activate_array(l.output + index, (1+l.classes)*l.w*l.h, LOGISTIC);
        }
    }

	return l.output;
}


layer make_yolo_layer(int batch, int w, int h, int n, int total, int *mask, int classes)
{
    int i;
    layer l;
	memset(&l,0,sizeof(layer));

    l.type = YOLO;

    l.n = n;
    l.total = total;
    l.batch = batch;
    l.h = h;
    l.w = w;
    l.c = n*(classes + 4 + 1);
    l.out_w = l.w;
    l.out_h = l.h;
    l.out_c = l.c;
    l.classes = classes;
    //l.cost = (float *)calloc(1, sizeof(float));
    l.biases = (float *)calloc(total*2, sizeof(float));
    if(mask) l.mask = mask;
    else{
        l.mask = (int *)calloc(n, sizeof(int));
        for(i = 0; i < n; ++i){
            l.mask[i] = i;
        }
    }
    //l.bias_updates = (float *)calloc(n*2, sizeof(float));
    l.outputs = h*w*n*(classes + 4 + 1);
    l.inputs = l.outputs;
    //l.truths = 90*(4 + 1);
    //l.delta = (float *)calloc(batch*l.outputs, sizeof(float));
	
    l.output = (float *)calloc(batch*l.outputs, sizeof(float));
    for(i = 0; i < total*2; ++i){
        l.biases[i] = .5;
    }

    l.forward = forward_yolo_layer;

    fprintf(stderr, "detection\n");
    srand(0);

    return l;
}

/////////////////praser begin
typedef struct{
    char *type;
    list *options;
}section;

list *read_cfg(char *filename);

LAYER_TYPE string_to_layer_type(char * type)
{

    if (strcmp(type, "[shortcut]")==0) return SHORTCUT;
    if (strcmp(type, "[crop]")==0) return CROP;
    if (strcmp(type, "[cost]")==0) return COST;
    if (strcmp(type, "[detection]")==0) return DETECTION;
    if (strcmp(type, "[region]")==0) return REGION;
    if (strcmp(type, "[yolo]")==0) return YOLO;
    if (strcmp(type, "[local]")==0) return LOCAL;
    if (strcmp(type, "[conv]")==0
            || strcmp(type, "[convolutional]")==0) return CONVOLUTIONAL;
    if (strcmp(type, "[deconv]")==0
            || strcmp(type, "[deconvolutional]")==0) return DECONVOLUTIONAL;
    if (strcmp(type, "[activation]")==0) return ACTIVE;
    if (strcmp(type, "[logistic]")==0) return LOGXENT;
    if (strcmp(type, "[l2norm]")==0) return L2NORM;
    if (strcmp(type, "[net]")==0
            || strcmp(type, "[network]")==0) return NETWORK;
    if (strcmp(type, "[crnn]")==0) return CRNN;
    if (strcmp(type, "[gru]")==0) return GRU;
    if (strcmp(type, "[lstm]") == 0) return LSTM;
    if (strcmp(type, "[rnn]")==0) return RNN;
    if (strcmp(type, "[conn]")==0
            || strcmp(type, "[connected]")==0) return CONNECTED;
    if (strcmp(type, "[max]")==0
            || strcmp(type, "[maxpool]")==0) return MAXPOOL;
    if (strcmp(type, "[reorg]")==0) return REORG;
    if (strcmp(type, "[avg]")==0
            || strcmp(type, "[avgpool]")==0) return AVGPOOL;
    if (strcmp(type, "[dropout]")==0) return DROPOUT;
    if (strcmp(type, "[lrn]")==0
            || strcmp(type, "[normalization]")==0) return NORMALIZATION;
    if (strcmp(type, "[batchnorm]")==0) return BATCHNORM;
    if (strcmp(type, "[soft]")==0
            || strcmp(type, "[softmax]")==0) return SOFTMAX;
    if (strcmp(type, "[route]")==0) return ROUTE;
    if (strcmp(type, "[upsample]")==0) return UPSAMPLE;
    return BLANK;
}

void free_section(section *s)
{
    free(s->type);
    node *n = s->options->front;
    while(n){
        kvp *pair = (kvp *)n->val;
        free(pair->key);
        free(pair);
        node *next = n->next;
        free(n);
        n = next;
    }
    free(s->options);
    free(s);
}

void parse_data(char *data, float *a, int n)
{
    int i;
    if(!data) return;
    char *curr = data;
    char *next = data;
    int done = 0;
    for(i = 0; i < n && !done; ++i){
        while(*++next !='\0' && *next != ',');
        if(*next == '\0') done = 1;
        *next = '\0';
        sscanf(curr, "%g", &a[i]);
        curr = next+1;
    }
}

typedef struct size_params{
    int batch;
    int inputs;
    int h;
    int w;
    int c;
    int index;
    int time_steps;
    network *net;
} size_params;

layer parse_convolutional(list *options, size_params params)
{
    int n = option_find_int(options, (char*)"filters",1);
    int size = option_find_int(options, (char*)"size",1);
    int stride = option_find_int(options, (char*)"stride",1);
    int pad = option_find_int_quiet(options, (char*)"pad",0);
    int padding = option_find_int_quiet(options, (char*)"padding",0);
    int groups = option_find_int_quiet(options, (char*)"groups", 1);
    if(pad) padding = size/2;

    char *activation_s = option_find_str(options, (char*)"activation", (char*)"logistic");
    ACTIVATION activation = get_activation(activation_s);

    int batch,h,w,c;
    h = params.h;
    w = params.w;
    c = params.c;
    batch=params.batch;
    if(!(h && w && c)) error((char*)"Layer before convolutional layer must output image.");
    int batch_normalize = option_find_int_quiet(options, (char*)"batch_normalize", 0);
    int binary = option_find_int_quiet(options, (char*)"binary", 0);
    int xnor = option_find_int_quiet(options, (char*)"xnor", 0);

    layer l = make_convolutional_layer(batch,h,w,c,n,groups,size,stride,padding,activation, batch_normalize, binary, xnor, params.net->adam);
    l.flipped = option_find_int_quiet(options, (char*)"flipped", 0);
    l.dot = option_find_float_quiet(options, (char*)"dot", 0);

    return l;
}

int *parse_yolo_mask(char *a, int *num)
{
    int *mask = 0;
    if(a){
        int len = strlen(a);
        int n = 1;
        int i;
        for(i = 0; i < len; ++i){
            if (a[i] == ',') ++n;
        }
        mask = (int *)calloc(n, sizeof(int));
        for(i = 0; i < n; ++i){
            int val = atoi(a);
            mask[i] = val;
            a = strchr(a, ',')+1;
        }
        *num = n;
    }
    return mask;
}

layer parse_yolo(list *options, size_params params)
{
    int classes = option_find_int(options, (char*)"classes", 20);
    int total = option_find_int(options, (char*)"num", 1);
    int num = total;

    char *a = option_find_str(options, (char*)"mask", 0);
    int *mask = parse_yolo_mask(a, &num);
    layer l = make_yolo_layer(params.batch, params.w, params.h, num, total, mask, classes);
    assert(l.outputs == params.inputs);

    l.max_boxes = option_find_int_quiet(options, (char*)"max",90);
    l.jitter = option_find_float(options, (char*)"jitter", .2);

    l.ignore_thresh = option_find_float(options, (char*)"ignore_thresh", .5);
    l.truth_thresh = option_find_float(options, (char*)"truth_thresh", 1);
    l.random = option_find_int_quiet(options, (char*)"random", 0);

    a = option_find_str(options, (char*)"anchors", 0);
    if(a){
        int len = strlen(a);
        int n = 1;
        int i;
        for(i = 0; i < len; ++i){
            if (a[i] == ',') ++n;
        }
        for(i = 0; i < n; ++i){
            float bias = atof(a);
            l.biases[i] = bias;
            a = strchr(a, ',')+1;
        }
    }
    return l;
}

layer parse_shortcut(list *options, size_params params, network *net)
{
    char *l = option_find(options, (char*)"from");
    int index = atoi(l);
    if(index < 0) index = params.index + index;

    int batch = params.batch;
    layer from = net->layers[index];

    layer s = make_shortcut_layer(batch, index, params.w, params.h, params.c, from.out_w, from.out_h, from.out_c);

    char *activation_s = option_find_str(options, (char*)"activation", (char*)"linear");
    ACTIVATION activation = get_activation(activation_s);
    s.activation = activation;
    s.alpha = option_find_float_quiet(options, (char*)"alpha", 1);
    s.beta = option_find_float_quiet(options, (char*)"beta", 1);
    return s;
}

layer parse_upsample(list *options, size_params params, network *net)
{

    int stride = option_find_int(options, (char*)"stride",2);
    layer l = make_upsample_layer(params.batch, params.w, params.h, params.c, stride);
    l.scale = option_find_float_quiet(options, (char*)"scale", 1);
    return l;
}

layer parse_route(list *options, size_params params, network *net)
{
    char *l = option_find(options, (char*)"layers");
    int len = strlen(l);
    if(!l) error((char*)"Route Layer must specify input layers");
    int n = 1;
    int i;
    for(i = 0; i < len; ++i){
        if (l[i] == ',') ++n;
    }

    int *layers = (int *)calloc(n, sizeof(int));
    int *sizes = (int *)calloc(n, sizeof(int));
    for(i = 0; i < n; ++i){
        int index = atoi(l);
        l = strchr(l, ',')+1;
        if(index < 0) index = params.index + index;
        layers[i] = index;
        sizes[i] = net->layers[index].outputs;
    }
    int batch = params.batch;

    layer route_layer = make_route_layer(batch, n, layers, sizes);

    layer first = net->layers[layers[0]];
    route_layer.out_w = first.out_w;
    route_layer.out_h = first.out_h;
    route_layer.out_c = first.out_c;
    for(i = 1; i < n; ++i){
        int index = layers[i];
        layer next = net->layers[index];
        if(next.out_w == first.out_w && next.out_h == first.out_h){
            route_layer.out_c += next.out_c;
        }else{
            route_layer.out_h = route_layer.out_w = route_layer.out_c = 0;
        }
    }

    return route_layer;
}

void softmax(float *input, int n, float temp, int stride, float *output)
{
    int i;
    float sum = 0;
    float largest = -FLT_MAX;
    for(i = 0; i < n; ++i){
        if(input[i*stride] > largest) largest = input[i*stride];
    }
    for(i = 0; i < n; ++i){
        float e = exp(input[i*stride]/temp - largest/temp);
        sum += e;
        output[i*stride] = e;
    }
    for(i = 0; i < n; ++i){
        output[i*stride] /= sum;
    }
}

void softmax_cpu(float *input, int n, int batch, int batch_offset, int groups, int group_offset, int stride, float temp, float *output)
{
    int g, b;
    for(b = 0; b < batch; ++b){
        for(g = 0; g < groups; ++g){
            softmax(input + b*batch_offset + g*group_offset, n, temp, stride, output + b*batch_offset + g*group_offset);
        }
    }
}

void forward_region_layer(const layer l, network net)
{
    int i,j,b,t,n;
    memcpy(l.output, net.input, l.outputs*l.batch*sizeof(float));

#ifndef GPU
    for (b = 0; b < l.batch; ++b){
        for(n = 0; n < l.n; ++n){
            int index = entry_index(l, b, n*l.w*l.h, 0);
            activate_array(l.output + index, 2*l.w*l.h, LOGISTIC);
            index = entry_index(l, b, n*l.w*l.h, l.coords);
            if(!l.background) activate_array(l.output + index,   l.w*l.h, LOGISTIC);
            index = entry_index(l, b, n*l.w*l.h, l.coords + 1);
            //if(!l.softmax) activate_array(l.output + index, l.classes*l.w*l.h, LOGISTIC);
        }
    }

	if (l.softmax){
        int index = entry_index(l, 0, 0, l.coords + !l.background);
        softmax_cpu(net.input + index, l.classes + l.background, l.batch*l.n, l.inputs/l.n, l.w*l.h, 1, l.w*l.h, 1, l.output + index);
    }

//	double time1,time2;
//	time1 = what_time_is_it_now();
//	char line[256];
//	FILE *fp3;
//	char filename[256];
//	sprintf(filename, "yolo_region_input_float32_%d.txt", 13*13*425);
//	printf("YOLO_layer:outputs=%d,%s\n",l.outputs,filename);
//    if( (fp3 = fopen(filename, "w")) == NULL)fprintf(stderr,"CANNOT OPEN\n");
//	int x;
//    for( x = 0; x < l.outputs; x++)
//	{
//		sprintf(line, "%f\n", net.input[x]);
//		if(fputs(line,fp3)<0)fprintf(stderr,"write FILE failed\n");
//    }
//    fclose(fp3);
//	time2 = what_time_is_it_now();
//	printf("Predicted in %f seconds.\n",time2 - time1);


#endif

    if(!net.train) return;
}

layer make_region_layer(int batch, int w, int h, int n, int classes, int coords)
{
    layer l;
	memset(&l,0,sizeof(layer));
    l.type = REGION;

    l.n = n;
    l.batch = batch;
    l.h = h;
    l.w = w;
    l.c = n*(classes + coords + 1);
    l.out_w = l.w;
    l.out_h = l.h;
    l.out_c = l.c;
    l.classes = classes;
    l.coords = coords;
    l.biases = (float *)calloc(n*2, sizeof(float));
    l.outputs = h*w*n*(classes + coords + 1);
    l.inputs = l.outputs;
    l.truths = 30*(l.coords + 1);
    l.output = (float *)calloc(batch*l.outputs, sizeof(float));
    int i;
    for(i = 0; i < n*2; ++i){
        l.biases[i] = .5;
    }

    l.forward = forward_region_layer;

    fprintf(stderr, "detection\n");
    srand(0);

    return l;
}

layer parse_region(list *options, size_params params)
{
    int coords = option_find_int(options, (char*)"coords", 4);
    int classes = option_find_int(options, (char*)"classes", 20);
    int num = option_find_int(options, (char*)"num", 1);

    layer l = make_region_layer(params.batch, params.w, params.h, num, classes, coords);
    assert(l.outputs == params.inputs);

    l.log = option_find_int_quiet(options, (char*)"log", 0);
    l.sqrt = option_find_int_quiet(options, (char*)"sqrt", 0);

    l.softmax = option_find_int(options, (char*)"softmax", 0);
    l.background = option_find_int_quiet(options, (char*)"background", 0);
    l.max_boxes = option_find_int_quiet(options, (char*)"max",30);
    l.jitter = option_find_float(options, (char*)"jitter", .2);
    l.rescore = option_find_int_quiet(options, (char*)"rescore",0);

    l.thresh = option_find_float(options, (char*)"thresh", .5);
    l.classfix = option_find_int_quiet(options, (char*)"classfix", 0);
    l.absolute = option_find_int_quiet(options, (char*)"absolute", 0);
    l.random = option_find_int_quiet(options, (char*)"random", 0);

    l.coord_scale = option_find_float(options, (char*)"coord_scale", 1);
    l.object_scale = option_find_float(options, (char*)"object_scale", 1);
    l.noobject_scale = option_find_float(options, (char*)"noobject_scale", 1);
    l.mask_scale = option_find_float(options, (char*)"mask_scale", 1);
    l.class_scale = option_find_float(options, (char*)"class_scale", 1);
    l.bias_match = option_find_int_quiet(options, (char*)"bias_match",0);

    char *tree_file = option_find_str(options, (char*)"tree", 0);
   // if (tree_file) l.softmax_tree = read_tree(tree_file);
    char *map_file = option_find_str(options, (char*)"map", 0);
   // if (map_file) l.map = read_map(map_file);

    char *a = option_find_str(options, (char*)"anchors", 0);
    if(a){
        int len = strlen(a);
        int n = 1;
        int i;
        for(i = 0; i < len; ++i){
            if (a[i] == ',') ++n;
        }
        for(i = 0; i < n; ++i){
            float bias = atof(a);
            l.biases[i] = bias;
            a = strchr(a, ',')+1;
        }
    }
    return l;
}

void reorg_cpu(float *x, int w, int h, int c, int batch, int stride, int forward, float *out)
{
    int b,i,j,k;
    int out_c = c/(stride*stride);

    for(b = 0; b < batch; ++b){
        for(k = 0; k < c; ++k){
            for(j = 0; j < h; ++j){
                for(i = 0; i < w; ++i){
                    int in_index  = i + w*(j + h*(k + c*b));
                    int c2 = k % out_c;
                    int offset = k / out_c;
                    int w2 = i*stride + offset % stride;
                    int h2 = j*stride + offset / stride;
                    int out_index = w2 + w*stride*(h2 + h*stride*(c2 + out_c*b));
                    if(forward) out[out_index] = x[in_index];
                    else out[in_index] = x[out_index];
                }
            }
        }
    }
}

void forward_reorg_layer(const layer l, network net)
{
    int i;
    //if(l.flatten){
    //    memcpy(l.output, net.input, l.outputs*l.batch*sizeof(float));
    //    if(l.reverse){
    //        flatten(l.output, l.w*l.h, l.c, l.batch, 0);
    //    }else{
    //        flatten(l.output, l.w*l.h, l.c, l.batch, 1);
    //    }
    //} else if (l.extra) {
    //    for(i = 0; i < l.batch; ++i){
    //        copy_cpu(l.inputs, net.input + i*l.inputs, 1, l.output + i*l.outputs, 1);
    //    }
    //} else if (l.reverse){
    //    reorg_cpu(net.input, l.w, l.h, l.c, l.batch, l.stride, 1, l.output);
    //} else {
        reorg_cpu(net.input, l.w, l.h, l.c, l.batch, l.stride, 0, l.output);
    //}
}

layer make_reorg_layer(int batch, int w, int h, int c, int stride, int reverse, int flatten, int extra)
{
    layer l;
	memset(&l,0,sizeof(layer));

    l.type = REORG;
    l.batch = batch;
    l.stride = stride;
    l.extra = extra;
    l.h = h;
    l.w = w;
    l.c = c;
    l.flatten = flatten;
    if(reverse){
        l.out_w = w*stride;
        l.out_h = h*stride;
        l.out_c = c/(stride*stride);
    }else{
        l.out_w = w/stride;
        l.out_h = h/stride;
        l.out_c = c*(stride*stride);
    }
    l.reverse = reverse;

    l.outputs = l.out_h * l.out_w * l.out_c;
    l.inputs = h*w*c;
    if(l.extra){
        l.out_w = l.out_h = l.out_c = 0;
        l.outputs = l.inputs + l.extra;
    }

    if(extra){
        fprintf(stderr, "reorg              %4d   ->  %4d\n",  l.inputs, l.outputs);
    } else {
        fprintf(stderr, "reorg              /%2d  %4d x%4d x%4d   ->  %4d x%4d x%4d\n",  stride, w, h, c, l.out_w, l.out_h, l.out_c);
    }
    int output_size = l.outputs * batch;
    //l.output =  (float *)calloc(output_size, sizeof(float));

    l.forward = forward_reorg_layer;


    return l;
}

layer parse_reorg(list *options, size_params params)
{
    int stride = option_find_int(options, (char*)"stride",1);
    int reverse = option_find_int_quiet(options, (char*)"reverse",0);
    int flatten = option_find_int_quiet(options, (char*)"flatten",0);
    int extra = option_find_int_quiet(options, (char*)"extra",0);

    int batch,h,w,c;
    h = params.h;
    w = params.w;
    c = params.c;
    batch=params.batch;
    if(!(h && w && c)) error("Layer before reorg layer must output image.");

    layer layer = make_reorg_layer(batch,w,h,c,stride,reverse, flatten, extra);
    return layer;
}

void forward_maxpool_layer(layer l, network net)
{
    int b,i,j,k,m,n;
    int w_offset = -l.pad;
    int h_offset = -l.pad;

    int h = l.out_h;
    int w = l.out_w;
    int c = l.c;

    for(b = 0; b < l.batch; ++b){
        for(k = 0; k < c; ++k){
            for(i = 0; i < h; ++i){
                for(j = 0; j < w; ++j){
                    int out_index = j + w*(i + h*(k + c*b));
                    float max = -FLT_MAX;
                    int max_i = -1;
                    for(n = 0; n < l.size; ++n){
                        for(m = 0; m < l.size; ++m){
                            int cur_h = h_offset + i*l.stride + n;
                            int cur_w = w_offset + j*l.stride + m;
                            int index = cur_w + l.w*(cur_h + l.h*(k + b*l.c));
                            int valid = (cur_h >= 0 && cur_h < l.h &&
                                         cur_w >= 0 && cur_w < l.w);
                            float val = (valid != 0) ? net.input[index] : -FLT_MAX;
                            max_i = (val > max) ? index : max_i;
                            max   = (val > max) ? val   : max;
                        }
                    }
                    l.output[out_index] = max;
                    l.indexes[out_index] = max_i;
                }
            }
        }
    }
}

layer make_maxpool_layer(int batch, int h, int w, int c, int size, int stride, int padding)
{
    layer l;
	memset(&l,0,sizeof(layer));
    l.type = MAXPOOL;
    l.batch = batch;
    l.h = h;
    l.w = w;
    l.c = c;
    l.pad = padding;
    l.out_w = (w + padding - size)/stride + 1;
    l.out_h = (h + padding - size)/stride + 1;
    l.out_c = c;
    l.outputs = l.out_h * l.out_w * l.out_c;
    l.inputs = h*w*c;
    l.size = size;
    l.stride = stride;
    int output_size = l.out_h * l.out_w * l.out_c * batch;

    fprintf(stderr, "max          %d x %d / %d  %4d x%4d x%4d   ->  %4d x%4d x%4d\n", size, size, stride, w, h, c, l.out_w, l.out_h, l.out_c);
    return l;
}

layer parse_maxpool(list *options, size_params params)
{
    int stride = option_find_int(options, (char*)"stride",1);
    int size = option_find_int(options, (char*)"size",stride);
    int padding = option_find_int_quiet(options, (char*)"padding", size-1);

    int batch,h,w,c;
    h = params.h;
    w = params.w;
    c = params.c;
    batch=params.batch;
    if(!(h && w && c)) error("Layer before maxpool layer must output image.");

    layer maxpool_layer = make_maxpool_layer(batch,h,w,c,size,stride,padding);
    return maxpool_layer;
}


//insert
typedef layer detection_layer;

void forward_detection_layer(const detection_layer l, network net)
{
    int locations = l.side*l.side;
    int i,j;
    memcpy(l.output, net.input, l.outputs*l.batch*sizeof(float));
    //if(l.reorg) reorg(l.output, l.w*l.h, size*l.n, l.batch, 1);
    int b;
    if (l.softmax){
        for(b = 0; b < l.batch; ++b){
            int index = b*l.inputs;
            for (i = 0; i < locations; ++i) {
                int offset = i*l.classes;
                softmax(l.output + index + offset, l.classes, 1, 1,
                        l.output + index + offset);
            }
        }

    }
}


learning_rate_policy get_policy(char *s)
{
    if (strcmp(s, "random")==0) return RANDOM;
    if (strcmp(s, "poly")==0) return POLY;
    if (strcmp(s, "constant")==0) return CONSTANT;
    if (strcmp(s, "step")==0) return STEP;
    if (strcmp(s, "exp")==0) return EXP;
    if (strcmp(s, "sigmoid")==0) return SIG;
    if (strcmp(s, "steps")==0) return STEPS;
    fprintf(stderr, "Couldn't find policy %s, going with constant\n", s);
    return CONSTANT;
}

void parse_net_options(list *options, network *net)
{
    net->batch = option_find_int(options, (char*)"batch",1);
    net->learning_rate = option_find_float(options, (char*)"learning_rate", .001);
    net->momentum = option_find_float(options, (char*)"momentum", .9);
    net->decay = option_find_float(options, (char*)"decay", .0001);
    int subdivs = option_find_int(options, (char*)"subdivisions",1);
    net->time_steps = option_find_int_quiet(options, (char*)"time_steps",1);
    net->notruth = option_find_int_quiet(options, (char*)"notruth",0);
    net->batch /= subdivs;
    net->batch *= net->time_steps;
    net->subdivisions = subdivs;
    net->random = option_find_int_quiet(options, (char*)"random", 0);

    net->adam = option_find_int_quiet(options, (char*)"adam", 0);
    if(net->adam){
        net->B1 = option_find_float(options, (char*)"B1", .9);
        net->B2 = option_find_float(options, (char*)"B2", .999);
        net->eps = option_find_float(options, (char*)"eps", .0000001);
    }

    net->h = option_find_int_quiet(options, (char*)"height",0);
    net->w = option_find_int_quiet(options, (char*)"width",0);
    net->c = option_find_int_quiet(options, (char*)"channels",0);
    net->inputs = option_find_int_quiet(options, (char*)"inputs", net->h * net->w * net->c);
    net->max_crop = option_find_int_quiet(options, (char*)"max_crop",net->w*2);
    net->min_crop = option_find_int_quiet(options, (char*)"min_crop",net->w);
    net->max_ratio = option_find_float_quiet(options, (char*)"max_ratio", (float) net->max_crop / net->w);
    net->min_ratio = option_find_float_quiet(options, (char*)"min_ratio", (float) net->min_crop / net->w);
    net->center = option_find_int_quiet(options, (char*)"center",0);
    net->clip = option_find_float_quiet(options, (char*)"clip", 0);

    net->angle = option_find_float_quiet(options, (char*)"angle", 0);
    net->aspect = option_find_float_quiet(options, (char*)"aspect", 1);
    net->saturation = option_find_float_quiet(options, (char*)"saturation", 1);
    net->exposure = option_find_float_quiet(options, (char*)"exposure", 1);
    net->hue = option_find_float_quiet(options, (char*)"hue", 0);

    if(!net->inputs && !(net->h && net->w && net->c)) error("No input parameters supplied");

    char *policy_s = option_find_str(options, (char*)"policy", (char*)"constant");
    net->policy = get_policy(policy_s);
    net->burn_in = option_find_int_quiet(options, (char*)"burn_in", 0);
    net->power = option_find_float_quiet(options, (char*)"power", 4);
    if(net->policy == STEP){
        net->step = option_find_int(options, (char*)"step", 1);
        net->scale = option_find_float(options, (char*)"scale", 1);
    } else if (net->policy == STEPS){
        char *l = option_find(options, (char*)"steps");
        char *p = option_find(options, (char*)"scales");
        if(!l || !p) error("STEPS policy must have steps and scales in cfg file");

        int len = strlen(l);
        int n = 1;
        int i;
        for(i = 0; i < len; ++i){
            if (l[i] == ',') ++n;
        }
        int *steps = (int *)calloc(n, sizeof(int));
        float *scales = (float *)calloc(n, sizeof(float));
        for(i = 0; i < n; ++i){
            int step    = atoi(l);
            float scale = atof(p);
            l = strchr(l, ',')+1;
            p = strchr(p, ',')+1;
            steps[i] = step;
            scales[i] = scale;
        }
        net->scales = scales;
        net->steps = steps;
        net->num_steps = n;
    } else if (net->policy == EXP){
        net->gamma = option_find_float(options, (char*)"gamma", 1);
    } else if (net->policy == SIG){
        net->gamma = option_find_float(options, (char*)"gamma", 1);
        net->step = option_find_int(options, (char*)"step", 1);
    } else if (net->policy == POLY || net->policy == RANDOM){
    }
    net->max_batches = option_find_int(options, (char*)"max_batches", 0);
}

int is_network(section *s)
{
    return (strcmp(s->type, "[net]")==0
            || strcmp(s->type, "[network]")==0);
}

network *parse_network_cfg(char *filename)
{
    list *sections = read_cfg(filename);
    node *n = sections->front;
    if(!n) error("Config file has no sections");
    network *net = make_network(sections->size - 1);
    net->gpu_index = -1;
    size_params params;

    section *s = (section *)n->val;
    list *options = s->options;
    if(!is_network(s)) error("First section must be [net] or [network]");
    parse_net_options(options, net);

    params.h = net->h;
    params.w = net->w;
    params.c = net->c;
    params.inputs = net->inputs;
    params.batch = net->batch;
    params.time_steps = net->time_steps;
    params.net = net;

    size_t workspace_size = 0;
    n = n->next;
    int count = 0;
    free_section(s);
    fprintf(stderr, "layer     filters    size              input                output\n");
    while(n){
        params.index = count;
        fprintf(stderr, "%5d ", count);
        s = (section *)n->val;
        options = s->options;
        //layer l = {0};
		layer l;
		memset(&l,0,sizeof(layer));
        LAYER_TYPE lt = string_to_layer_type(s->type);
        if(lt == CONVOLUTIONAL){
            l = parse_convolutional(options, params);
        }else if(lt == YOLO){
            l = parse_yolo(options, params);
        }else if(lt == ROUTE){
            l = parse_route(options, params, net);
        }else if(lt == UPSAMPLE){
            l = parse_upsample(options, params, net);
        }else if(lt == SHORTCUT){
            l = parse_shortcut(options, params, net);
        }else if(lt == REGION){
            l = parse_region(options, params);
        }else if(lt == YOLO){
            l = parse_yolo(options, params);
        }else if(lt == MAXPOOL){
            l = parse_maxpool(options, params);
        }else if(lt == REORG){
            l = parse_reorg(options, params);

		}else{
            fprintf(stderr, "Type not recognized: %s\n", s->type);
        }
        l.clip = net->clip;
        l.truth = option_find_int_quiet(options, (char*)"truth", 0);
        l.onlyforward = option_find_int_quiet(options, (char*)"onlyforward", 0);
        l.stopbackward = option_find_int_quiet(options, (char*)"stopbackward", 0);
        l.dontsave = option_find_int_quiet(options, (char*)"dontsave", 0);
//        l.dontload = option_find_int_quiet(options, "dontload", 0);
      //  l.dontloadscales = option_find_int_quiet(options, "dontloadscales", 0);
        //l.learning_rate_scale = option_find_float_quiet(options, "learning_rate", 1);
        l.smooth = option_find_float_quiet(options, (char*)"smooth", 0);
        option_unused(options);
        net->layers[count] = l;
        if (l.workspace_size > workspace_size) workspace_size = l.workspace_size;
        free_section(s);
        n = n->next;
        ++count;
        if(n){
            params.h = l.out_h;
            params.w = l.out_w;
            params.c = l.out_c;
            params.inputs = l.outputs;
        }
    }
    free_list(sections);
    layer out = get_network_output_layer(net);
    net->outputs = out.outputs;
    net->output = out.output;
    //net->input = (float *)calloc(net->inputs*net->batch, sizeof(float));

	workspace_size = 0;//donot calloc workspace
    //if(workspace_size){
    //    //printf("%ld\n", workspace_size);
    //    net->workspace = (float *)calloc(1, workspace_size);

    //}
    return net;
}

list *read_cfg(char *filename)
{
    FILE *file = fopen(filename, "r");
    if(file == 0) file_error(filename);
    char *line;
    int nu = 0;
    list *options = make_list();
    section *current = 0;
    while((line=fgetl(file)) != 0){
        ++ nu;
        strip(line);
        switch(line[0]){
            case '[':
                current = (section *)malloc(sizeof(section));
                list_insert(options, current);
                current->options = make_list();
                current->type = line;
                break;
            case '\0':
            case '#':
            case ';':
                free(line);
                break;
            default:
                if(!read_option(line, current->options)){
                    fprintf(stderr, "Config file error line %d, could parse: %s\n", nu, line);
                    free(line);
                }
                break;
        }
    }
    fclose(file);
    return options;
}

void load_convolutional_weights(layer l, FILE *fp)
{
    int num = l.nweights;
    fread(l.biases, sizeof(float), l.n, fp);
    if (l.batch_normalize){
        fread(l.scales, sizeof(float), l.n, fp);
        fread(l.rolling_mean, sizeof(float), l.n, fp);
        fread(l.rolling_variance, sizeof(float), l.n, fp);
    }
    fread(l.weights, sizeof(float), num, fp);

}


void load_weights_upto(network *net, char *filename, int start, int cutoff)
{
    fprintf(stderr, "Loading weights from %s...", filename);
    fflush(stdout);
    FILE *fp = fopen(filename, "rb");
    if(!fp) file_error(filename);

    int major;
    int minor;
    int revision;
    fread(&major, sizeof(int), 1, fp);
    fread(&minor, sizeof(int), 1, fp);
    fread(&revision, sizeof(int), 1, fp);
	printf("major=%d;minor=%d;revision=%d\n",major,minor,revision);// 0 2 0
	printf("if true ro false:%d\n",(major*10 + minor) >= 2 && major < 1000 && minor < 1000);
    if ((major*10 + minor) >= 2 && major < 1000 && minor < 1000){
        //fread(net->seen, sizeof(size_t), 1, fp);
		fread(net->seen, sizeof(size_t), 1, fp);
		fread(net->seen, sizeof(size_t), 1, fp);
	}else {
        int iseen = 0;
        fread(&iseen, sizeof(int), 1, fp);
        *net->seen = iseen;
    }

	//printf("sizeof(size_t)=%u\n",sizeof(size_t));// in my PC is 4

    int i;
    for(i = start; i < net->n && i < cutoff; ++i){
        layer l = net->layers[i];
        if(l.type == CONVOLUTIONAL){
            load_convolutional_weights(l, fp);
        }
    }
    fprintf(stderr, "Done!\n");
    fclose(fp);
}

void load_weights(network *net, char *filename)
{
    load_weights_upto(net, filename, 0, net->n);
}
/////////////////praser end

/////////////////network begin
load_args get_base_args(network *net)
{
    load_args args = {0};
    args.w = net->w;
    args.h = net->h;
    args.size = net->w;

    args.min = net->min_crop;
    args.max = net->max_crop;
    args.angle = net->angle;
    args.aspect = net->aspect;
    args.exposure = net->exposure;
    args.center = net->center;
    args.saturation = net->saturation;
    args.hue = net->hue;
    return args;
}

network *load_network(char *cfg, char *weights, int clear)
{
    network *net = parse_network_cfg(cfg);
    //if(weights && weights[0] != 0){
    //    load_weights(net, weights);
    //}
    if(clear) (*net->seen) = 0;
    return net;
}

char *get_layer_string(LAYER_TYPE a)
{
    switch(a){
        case CONVOLUTIONAL:
            return (char*)"convolutional";
        case ACTIVE:
            return (char*)"activation";
        case LOCAL:
            return (char*)"local";
        case DECONVOLUTIONAL:
            return (char*)"deconvolutional";
        case CONNECTED:
            return (char*)"connected";
        case RNN:
            return (char*)"rnn";
        case GRU:
            return (char*)"gru";
        case LSTM:
	    return (char*)"lstm";
        case CRNN:
            return (char*)"crnn";
        case MAXPOOL:
            return (char*)"maxpool";
        case REORG:
            return (char*)"reorg";
        case AVGPOOL:
            return (char*)"avgpool";
        case SOFTMAX:
            return (char*)"softmax";
        case DETECTION:
            return (char*)"detection";
        case REGION:
            return (char*)"region";
        case YOLO:
            return (char*)"yolo";
        case DROPOUT:
            return (char*)"dropout";
        case CROP:
            return (char*)"crop";
        case COST:
            return (char*)"cost";
        case ROUTE:
            return (char*)"route";
        case SHORTCUT:
            return (char*)"shortcut";
        case NORMALIZATION:
            return (char*)"normalization";
        case BATCHNORM:
            return (char*)"batchnorm";
        default:
            break;
    }
    return (char*)"none";
}

network *make_network(int n)
{
    network *net = (network *)calloc(1, sizeof(network));
    net->n = n;
    net->layers = (layer *)calloc(net->n, sizeof(layer));
    net->seen = (size_t *)calloc(1, sizeof(size_t));
    net->t    = (int *)calloc(1, sizeof(int));
    net->cost = (float *)calloc(1, sizeof(float));
    return net;
}

void forward_network(network *netp)
{
    network net = *netp;
    int i;
    for(i = 0; i < net.n; ++i){
        net.index = i;
        layer l = net.layers[i];
        l.forward(l, net);
        net.input = l.output;

	//	printf("layer [%d]\n",i);
    }

}


void set_temp_network(network *net, float t)
{
    int i;
    for(i = 0; i < net->n; ++i){
        net->layers[i].temperature = t;
    }
}

void set_batch_network(network *net, int b)
{
    net->batch = b;
    int i;
    for(i = 0; i < net->n; ++i){
        net->layers[i].batch = b;
    }
}

float *network_predict(network *net, float *input)
{
    network orig = *net;
    net->input = input;
    net->truth = 0;
    net->train = 0;
    net->delta = 0;
    forward_network(net);
    float *out = net->output;
    *net = orig;
    return out;
}
int yolo_num_detections(layer l, float thresh)
{
    int i, n,kk;
    int count = 0;
	
	#ifdef TEST
	printf("yolo_num_detections:");
	n = entry_index(l, 0, 0*l.w*l.h + 0, 4);
	printf("l.output[%d] = %f,thresh = %f\n",n,l.output[n],thresh);
	
	char line[256];
	FILE *fp3;
	char filename[256];
	sprintf(filename, "YOLO_layer_%d.txt", l.outputs);
	if( (fp3 = fopen(filename, "w")) == NULL)fprintf(stderr,"CANNOT OPEN\n");
	for( kk = 0; kk < l.outputs; kk++)
	{
		sprintf(line, "%f\n", l.output[kk]);
		if(fputs(line,fp3)<0)fprintf(stderr,"write FILE failed\n");
	}
	fclose(fp3);
	printf("END\n");
	#endif
	
    for (i = 0; i < l.w*l.h; ++i){
        for(n = 0; n < l.n; ++n){
            int obj_index  = entry_index(l, 0, n*l.w*l.h + i, 4);
            if(l.output[obj_index] > thresh){
                ++count;
            }
        }
    }
	//printf("count = %d\n",count);
    return count;
}

int num_detections(network *net, float thresh)
{
    int i;
    int s = 0;
    for(i = 0; i < net->n; ++i){
        layer l = net->layers[i];
        if(l.type == YOLO){
			
            s += yolo_num_detections(l, thresh);
			//printf("l.output[1] = %f,s = %d,l.n = %d\n",l.output[1],s,l.n);
        }
        if(l.type == DETECTION || l.type == REGION){
            s += l.w*l.h*l.n;
        }
    }
    return s;
}

detection *make_network_boxes(network *net, float thresh, int *num)
{
    layer l = net->layers[net->n - 1];
    int i;
    int nboxes = num_detections(net, thresh);
	//printf("num_detections nboxes = %d\n",nboxes);
    if(num) *num = nboxes;
    detection *dets = (detection *)calloc(nboxes, sizeof(detection));
    for(i = 0; i < nboxes; ++i){
        dets[i].prob = (float *)calloc(l.classes, sizeof(float));
    }
    return dets;
}

box get_yolo_box(float *x, float *biases, int n, int index, int i, int j, int lw, int lh, int w, int h, int stride)
{
    box b;
    b.x = (i + x[index + 0*stride]) / lw;
    b.y = (j + x[index + 1*stride]) / lh;
    b.w = exp(x[index + 2*stride]) * biases[2*n]   / w;
    b.h = exp(x[index + 3*stride]) * biases[2*n+1] / h;
    return b;
}

void correct_yolo_boxes(detection *dets, int n, int w, int h, int netw, int neth, int relative)
{
    int i;
    int new_w=0;
    int new_h=0;
    if (((float)netw/w) < ((float)neth/h)) {
        new_w = netw;
        new_h = (h * netw)/w;
    } else {
        new_h = neth;
        new_w = (w * neth)/h;
    }
    for (i = 0; i < n; ++i){
        box b = dets[i].bbox;
        b.x =  (b.x - (netw - new_w)/2./netw) / ((float)new_w/netw);
        b.y =  (b.y - (neth - new_h)/2./neth) / ((float)new_h/neth);
        b.w *= (float)netw/new_w;
        b.h *= (float)neth/new_h;
        if(!relative){
            b.x *= w;
            b.w *= w;
            b.y *= h;
            b.h *= h;
        }
        dets[i].bbox = b;
    }
}


int get_yolo_detections(layer l, int w, int h, int netw, int neth, float thresh, int *map, int relative, detection *dets)
{
    int i,j,n;
    float *predictions = l.output;
   // if (l.batch == 2) avg_flipped_yolo(l);
    int count = 0;
    for (i = 0; i < l.w*l.h; ++i){
        int row = i / l.w;
        int col = i % l.w;
        for(n = 0; n < l.n; ++n){
            int obj_index  = entry_index(l, 0, n*l.w*l.h + i, 4);
            float objectness = predictions[obj_index];
            if(objectness <= thresh) continue;
            int box_index  = entry_index(l, 0, n*l.w*l.h + i, 0);
            dets[count].bbox = get_yolo_box(predictions, l.biases, l.mask[n], box_index, col, row, l.w, l.h, netw, neth, l.w*l.h);
            dets[count].objectness = objectness;
            dets[count].classes = l.classes;
            for(j = 0; j < l.classes; ++j){
                int class_index = entry_index(l, 0, n*l.w*l.h + i, 4 + 1 + j);
                float prob = objectness*predictions[class_index];
                dets[count].prob[j] = (prob > thresh) ? prob : 0;
            }
            ++count;
        }
    }
    correct_yolo_boxes(dets, count, w, h, netw, neth, relative);
	//printf("get_det count = %d\n",count);
    return count;
}

box get_region_box(float *x, float *biases, int n, int index, int i, int j, int w, int h, int stride)
{
    box b;
    b.x = (i + x[index + 0*stride]) / w;
    b.y = (j + x[index + 1*stride]) / h;
    b.w = exp(x[index + 2*stride]) * biases[2*n]   / w;
    b.h = exp(x[index + 3*stride]) * biases[2*n+1] / h;
    return b;
}

void correct_region_boxes(detection *dets, int n, int w, int h, int netw, int neth, int relative)
{
    int i;
    int new_w=0;
    int new_h=0;
    if (((float)netw/w) < ((float)neth/h)) {
        new_w = netw;
        new_h = (h * netw)/w;
    } else {
        new_h = neth;
        new_w = (w * neth)/h;
    }
    for (i = 0; i < n; ++i){
        box b = dets[i].bbox;
        b.x =  (b.x - (netw - new_w)/2./netw) / ((float)new_w/netw);
        b.y =  (b.y - (neth - new_h)/2./neth) / ((float)new_h/neth);
        b.w *= (float)netw/new_w;
        b.h *= (float)neth/new_h;
        if(!relative){
            b.x *= w;
            b.w *= w;
            b.y *= h;
            b.h *= h;
        }
        dets[i].bbox = b;
    }
}

void get_region_detections(layer l, int w, int h, int netw, int neth, float thresh, int *map, float tree_thresh, int relative, detection *dets)
{
    int i,j,n,z;
    float *predictions = l.output;
    if (l.batch == 2) {
        float *flip = l.output + l.outputs;
        for (j = 0; j < l.h; ++j) {
            for (i = 0; i < l.w/2; ++i) {
                for (n = 0; n < l.n; ++n) {
                    for(z = 0; z < l.classes + l.coords + 1; ++z){
                        int i1 = z*l.w*l.h*l.n + n*l.w*l.h + j*l.w + i;
                        int i2 = z*l.w*l.h*l.n + n*l.w*l.h + j*l.w + (l.w - i - 1);
                        float swap = flip[i1];
                        flip[i1] = flip[i2];
                        flip[i2] = swap;
                        if(z == 0){
                            flip[i1] = -flip[i1];
                            flip[i2] = -flip[i2];
                        }
                    }
                }
            }
        }
        for(i = 0; i < l.outputs; ++i){
            l.output[i] = (l.output[i] + flip[i])/2.;
        }
    }
    for (i = 0; i < l.w*l.h; ++i){
        int row = i / l.w;
        int col = i % l.w;
        for(n = 0; n < l.n; ++n){
            int index = n*l.w*l.h + i;
            for(j = 0; j < l.classes; ++j){
                dets[index].prob[j] = 0;
            }
            int obj_index  = entry_index(l, 0, n*l.w*l.h + i, l.coords);
            int box_index  = entry_index(l, 0, n*l.w*l.h + i, 0);
            int mask_index = entry_index(l, 0, n*l.w*l.h + i, 4);
            float scale = l.background ? 1 : predictions[obj_index];
            dets[index].bbox = get_region_box(predictions, l.biases, n, box_index, col, row, l.w, l.h, l.w*l.h);
            dets[index].objectness = scale > thresh ? scale : 0;
            if(dets[index].mask){
                for(j = 0; j < l.coords - 4; ++j){
                    dets[index].mask[j] = l.output[mask_index + j*l.w*l.h];
                }
            }

            int class_index = entry_index(l, 0, n*l.w*l.h + i, l.coords + !l.background);

            if(dets[index].objectness){
                for(j = 0; j < l.classes; ++j){
                    int class_index = entry_index(l, 0, n*l.w*l.h + i, l.coords + 1 + j);
                    float prob = scale*predictions[class_index];
                    dets[index].prob[j] = (prob > thresh) ? prob : 0;
                }
            }

        }
    }
    correct_region_boxes(dets, l.w*l.h*l.n, w, h, netw, neth, relative);
}

void fill_network_boxes(network *net, int w, int h, float thresh, float hier, int *map, int relative, detection *dets)
{
    int j;
    for(j = 0; j < net->n; ++j){
        layer l = net->layers[j];
        if(l.type == YOLO){
            int count = get_yolo_detections(l, w, h, net->w, net->h, thresh, map, relative, dets);
            dets += count;
        }
        if(l.type == REGION){
            get_region_detections(l, w, h, net->w, net->h, thresh, map, hier, relative, dets);
            dets += l.w*l.h*l.n;
        }
    }
}

detection *get_network_boxes(network *net, int w, int h, float thresh, float hier, int *map, int relative, int *num)
{
    //printf("make_network_boxes begin!\n");
	detection *dets = make_network_boxes(net, thresh, num);
	//printf("fill_network_boxes begin\n");
    fill_network_boxes(net, w, h, thresh, hier, map, relative, dets);
    return dets;
}

void free_detections(detection *dets, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        free(dets[i].prob);
        if(dets[i].mask) free(dets[i].mask);
    }
    free(dets);
}

int network_width(network *net){return net->w;}
int network_height(network *net){return net->h;}

layer get_network_output_layer(network *net)
{
    int i;
    for(i = net->n - 1; i >= 0; --i){
        if(net->layers[i].type != COST) break;
    }
    return net->layers[i];
}

void free_network(network *net)
{
    int i;
    for(i = 0; i < net->n; ++i){
        free_layer(net->layers[i]);
    }
    free(net->layers);
    if(net->input) free(net->input);
    if(net->truth) free(net->truth);

    free(net);
}

layer network_output_layer(network *net)
{
    int i;
    for(i = net->n - 1; i >= 0; --i){
        if(net->layers[i].type != COST) break;
    }
    return net->layers[i];
}

int network_inputs(network *net)
{
    return net->layers[0].inputs;
}

int network_outputs(network *net)
{
    return network_output_layer(net).outputs;
}

float *network_output(network *net)
{
    return network_output_layer(net).output;
}

//////////////////network end

//////////////////////box begin

int nms_comparator(const void *pa, const void *pb)
{
    detection a = *(detection *)pa;
    detection b = *(detection *)pb;
    float diff = 0;
    if(b.sort_class >= 0){
        diff = a.prob[b.sort_class] - b.prob[b.sort_class];
    } else {
        diff = a.objectness - b.objectness;
    }
    if(diff < 0) return 1;
    else if(diff > 0) return -1;
    return 0;
}

float overlap(float x1, float w1, float x2, float w2)
{
    float l1 = x1 - w1/2;
    float l2 = x2 - w2/2;
    float left = l1 > l2 ? l1 : l2;
    float r1 = x1 + w1/2;
    float r2 = x2 + w2/2;
    float right = r1 < r2 ? r1 : r2;
    return right - left;
}

float box_intersection(box a, box b)
{
    float w = overlap(a.x, a.w, b.x, b.w);
    float h = overlap(a.y, a.h, b.y, b.h);
    if(w < 0 || h < 0) return 0;
    float area = w*h;
    return area;
}

float box_union(box a, box b)
{
    float i = box_intersection(a, b);
    float u = a.w*a.h + b.w*b.h - i;
    return u;
}

float box_iou(box a, box b)
{
    return box_intersection(a, b)/box_union(a, b);
}

void do_nms_sort(detection *dets, int total, int classes, float thresh)
{
    int i, j, k;
    k = total-1;
    for(i = 0; i <= k; ++i){
        if(dets[i].objectness == 0){
            detection swap = dets[i];
            dets[i] = dets[k];
            dets[k] = swap;
            --k;
            --i;
        }
    }
    total = k+1;

    for(k = 0; k < classes; ++k){
        for(i = 0; i < total; ++i){
            dets[i].sort_class = k;
        }
        qsort(dets, total, sizeof(detection), nms_comparator);
        for(i = 0; i < total; ++i){
            if(dets[i].prob[k] == 0) continue;
            box a = dets[i].bbox;
            for(j = i+1; j < total; ++j){
                box b = dets[j].bbox;
                if (box_iou(a, b) > thresh){
                    dets[j].prob[k] = 0;
                }
            }
        }
    }
}
//////////////////////box end

//////////////////////image begin

float colors[6][3] = { {1,0,1}, {0,0,1},{0,1,1},{0,1,0},{1,1,0},{1,0,0} };

float get_color(int c, int x, int max)
{
    float ratio = ((float)x/max)*5;
    int i = floor(ratio);
    int j = ceil(ratio);
    ratio -= i;
    float r = (1-ratio) * colors[i][c] + ratio*colors[j][c];
    //printf("%f\n", r);
    return r;
}

static float get_pixel_extend(image m, int x, int y, int c)
{
    if(x < 0 || x >= m.w || y < 0 || y >= m.h) return 0;
    /*
    if(x < 0) x = 0;
    if(x >= m.w) x = m.w-1;
    if(y < 0) y = 0;
    if(y >= m.h) y = m.h-1;
    */
    if(c < 0 || c >= m.c) return 0;
    return get_pixel(m, x, y, c);
}

void composite_image(image source, image dest, int dx, int dy)
{
    int x,y,k;
    for(k = 0; k < source.c; ++k){
        for(y = 0; y < source.h; ++y){
            for(x = 0; x < source.w; ++x){
                float val = get_pixel(source, x, y, k);
                float val2 = get_pixel_extend(dest, dx+x, dy+y, k);
                set_pixel(dest, dx+x, dy+y, k, val * val2);
            }
        }
    }
}

image border_image(image a, int border)
{
    image b = make_image(a.w + 2*border, a.h + 2*border, a.c);
    int x,y,k;
    for(k = 0; k < b.c; ++k){
        for(y = 0; y < b.h; ++y){
            for(x = 0; x < b.w; ++x){
                float val = get_pixel_extend(a, x - border, y - border, k);
                if(x - border < 0 || x - border >= a.w || y - border < 0 || y - border >= a.h) val = 1;
                set_pixel(b, x, y, k, val);
            }
        }
    }
    return b;
}

image copy_image(image p)
{
    image copy = p;
    copy.data = (float *)calloc(p.h*p.w*p.c, sizeof(float));
    memcpy(copy.data, p.data, p.h*p.w*p.c*sizeof(float));
    return copy;
}

image tile_images(image a, image b, int dx)
{
    if(a.w == 0) return copy_image(b);
    image c = make_image(a.w + b.w + dx, (a.h > b.h) ? a.h : b.h, (a.c > b.c) ? a.c : b.c);
    fill_cpu(c.w*c.h*c.c, 1, c.data, 1);
    embed_image(a, c, 0, 0);
    composite_image(b, c, a.w + dx, 0);
    return c;
}

image get_label(image **characters, char *string, int size)
{
    size = size/10;
    if(size > 7) size = 7;
    image label = make_empty_image(0,0,0);
    while(*string){
        image l = characters[size][(int)*string];
        image n = tile_images(label, l, -size - 1 + (size+1)/2);
        free_image(label);
        label = n;
        ++string;
    }
    image b = border_image(label, label.h*.25);
    free_image(label);
    return b;
}

void draw_label(image a, int r, int c, image label, const float *rgb)
{
    int w = label.w;
    int h = label.h;
    if (r - h >= 0) r = r - h;

    int i, j, k;
    for(j = 0; j < h && j + r < a.h; ++j){
        for(i = 0; i < w && i + c < a.w; ++i){
            for(k = 0; k < label.c; ++k){
                float val = get_pixel(label, i, j, k);
                set_pixel(a, i+c, j+r, k, rgb[k] * val);
            }
        }
    }
}

void draw_box(image a, int x1, int y1, int x2, int y2, float r, float g, float b)
{
    //normalize_image(a);
    int i;
    if(x1 < 0) x1 = 0;
    if(x1 >= a.w) x1 = a.w-1;
    if(x2 < 0) x2 = 0;
    if(x2 >= a.w) x2 = a.w-1;

    if(y1 < 0) y1 = 0;
    if(y1 >= a.h) y1 = a.h-1;
    if(y2 < 0) y2 = 0;
    if(y2 >= a.h) y2 = a.h-1;

    for(i = x1; i <= x2; ++i){
        a.data[i + y1*a.w + 0*a.w*a.h] = r;
        a.data[i + y2*a.w + 0*a.w*a.h] = r;

        a.data[i + y1*a.w + 1*a.w*a.h] = g;
        a.data[i + y2*a.w + 1*a.w*a.h] = g;

        a.data[i + y1*a.w + 2*a.w*a.h] = b;
        a.data[i + y2*a.w + 2*a.w*a.h] = b;
    }
    for(i = y1; i <= y2; ++i){
        a.data[x1 + i*a.w + 0*a.w*a.h] = r;
        a.data[x2 + i*a.w + 0*a.w*a.h] = r;

        a.data[x1 + i*a.w + 1*a.w*a.h] = g;
        a.data[x2 + i*a.w + 1*a.w*a.h] = g;

        a.data[x1 + i*a.w + 2*a.w*a.h] = b;
        a.data[x2 + i*a.w + 2*a.w*a.h] = b;
    }
}

void draw_box_width(image a, int x1, int y1, int x2, int y2, int w, float r, float g, float b)
{
    int i;
    for(i = 0; i < w; ++i){
        draw_box(a, x1+i, y1+i, x2-i, y2-i, r, g, b);
    }
}

image float_to_image(int w, int h, int c, float *data)
{
    image out = make_empty_image(w,h,c);
    out.data = data;
    return out;
}

image threshold_image(image im, float thresh)
{
    int i;
    image t = make_image(im.w, im.h, im.c);
    for(i = 0; i < im.w*im.h*im.c; ++i){
        t.data[i] = im.data[i]>thresh ? 1 : 0;
    }
    return t;
}

void draw_detections(image im, detection *dets, int num, float thresh, char **names, image **alphabet, int classes)
{
    int i,j;

    for(i = 0; i < num; ++i){
        char labelstr[4096] = {0};
        int class_t = -1;
        for(j = 0; j < classes; ++j){
            if (dets[i].prob[j] > thresh){
                if (class_t < 0) {
                    strcat(labelstr, names[j]);
                    class_t = j;
                } else {
                    strcat(labelstr, ", ");
                    strcat(labelstr, names[j]);
                }
                printf("%s: %.0f%%\n", names[j], dets[i].prob[j]*100);
            }
        }
        if(class_t >= 0){
            int width = im.h * .006;
            //printf("%d %s: %.0f%%\n", i, names[class], prob*100);
            int offset = class_t*123457 % classes;
            float red = get_color(2,offset,classes);
            float green = get_color(1,offset,classes);
            float blue = get_color(0,offset,classes);
            float rgb[3];

            //width = prob*20+2;

            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
            box b = dets[i].bbox;
            //printf("%f %f %f %f\n", b.x, b.y, b.w, b.h);

            int left  = (b.x-b.w/2.)*im.w;
            int right = (b.x+b.w/2.)*im.w;
            int top   = (b.y-b.h/2.)*im.h;
            int bot   = (b.y+b.h/2.)*im.h;

            if(left < 0) left = 0;
            if(right > im.w-1) right = im.w-1;
            if(top < 0) top = 0;
            if(bot > im.h-1) bot = im.h-1;

            draw_box_width(im, left, top, right, bot, width, red, green, blue);
            if (alphabet) {
                image label = get_label(alphabet, labelstr, (im.h*.03));
                draw_label(im, top + width, left, label, rgb);
                free_image(label);
            }
            if (dets[i].mask){
                image mask = float_to_image(14, 14, 1, dets[i].mask);
                image resized_mask = resize_image(mask, b.w*im.w, b.h*im.h);
                image tmask = threshold_image(resized_mask, .5);
                embed_image(tmask, im, left, top);
                free_image(mask);
                free_image(resized_mask);
                free_image(tmask);
            }
        }
    }
}
//////////////////////image end


//////////////////////////HLS begin


#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define S 2
#define K 3

#define Tn 4
#define Tm 32
#define Tr 26
#define Tc 26
#define OnChipIB_Width  ((Tc-1)*S+K)
#define OnChipIB_Height ((Tr-1)*S+K)
#define ALPHA_BETA_MAX_NUM 1024
#define INTERWIDTH 20

void copy_mem2dev(uint8_t *orig,uint32_t byte_num, unsigned long in_buffer)
{
	int fd = open("/dev/mem", O_RDWR);
	unsigned char *virtual_addr;
	uint32_t RequestByteNum;// must page
	if(byte_num%(4*1024)==0){
		RequestByteNum = byte_num;

	}
	else
	{
		RequestByteNum = (byte_num/(4*1024)+1)*(4*1024);
	}

	virtual_addr = (unsigned char *)mmap(NULL, RequestByteNum, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)in_buffer);
	if(virtual_addr == MAP_FAILED)
	{
		printf("copy_mem2dev:Virtual_addr_in mappong for absolute memory access failed!\n");
		return;
	}
	memcpy(virtual_addr,orig,byte_num);

	munmap((void *)virtual_addr, byte_num);
	close(fd);
}

void copy_dev2mem(uint8_t *dst,uint32_t byte_num, unsigned long in_buffer)
{
	int fd = open("/dev/mem", O_RDWR);
	unsigned char *virtual_addr;
	uint32_t RequestByteNum;// must page
	if(byte_num%(4*1024)==0){
		RequestByteNum = byte_num;
	}
	else
	{
		RequestByteNum = (byte_num/(4*1024)+1)*(4*1024);
	}
	virtual_addr = (unsigned char *)mmap(NULL, RequestByteNum, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)in_buffer);
	if(virtual_addr == MAP_FAILED)
	{
		printf("copy_dev2mem:Virtual_addr_in mappong for absolute memory access failed!\n");
		return;
	}
	memcpy((uint8_t *)dst,virtual_addr,byte_num);

	munmap((void *)virtual_addr, byte_num);
	close(fd);
}

int copy_file2mem(char *bin_file,uint32_t byte_num,unsigned long in_buffer)
{
	unsigned char *buffer = (unsigned char *)malloc(1024*1024);
	if(buffer==NULL){
		printf("copy_file2mem:cannot malloc buffer 1024*1024 byte\n");
		return -1;
	}

	FILE *fp;
	if( (fp = fopen(bin_file, "rb")) == NULL)fprintf(stderr,"CANNOT OPEN bin_file\n");
	int rd_num;
	unsigned long offset = 0;
	while(rd_num = fread(buffer, sizeof(unsigned char), 1024*1024, fp))
	{
		copy_mem2dev(buffer,rd_num, in_buffer+offset);
		offset += rd_num;
	}
	fclose(fp);

	free(buffer);


	return 0;
}

int copy_mem2file(char *bin_file,uint32_t byte_num,unsigned long in_buffer)
{
	void *buffer = malloc(1024*1024);
	if(buffer==NULL){
		printf("copy_mem2file:cannot malloc buffer 1024*1024 byte\n");
		return -1;
	}

	FILE *fp;
	if( (fp = fopen(bin_file, "wb")) == NULL)fprintf(stderr,"CANNOT OPEN bin_file\n");

	int x = byte_num;
	int addbyte;
	unsigned long offset = 0;
	while(addbyte=((x<1024*1024)?x:(1024*1024)))
	{
		copy_dev2mem((uint8_t *)buffer,addbyte, in_buffer+offset);
		fwrite(buffer , sizeof(unsigned char), addbyte, fp);
		x -= addbyte;
		offset += addbyte;
	}
	fclose(fp);

	free(buffer);

	return 0;
}


int YOLO2_FPGA(int In_Address0,int In_Address1,int Out_Address,int Weight_offset,int Beta_offset,const int InFM_num,const int OutFM_num,
							  const int Kernel_size,const int Kernel_stride,
							  const int Input_w,const int Input_h,const int Output_w,const int Output_h,
							  const int Padding,const bool IsNL,const bool IsBN,
							  const int TM,const int TN,const int TR,const int TC,
							  const int mLoops,const int nLoops,const int rLoops,const int cLoops,const int LayerType,
							  int InputQ,int OutputQ,int WeightQ,int BetaQ,unsigned int WEIGHT_BASE,unsigned int BETA_BASE)
{

	int T2Rate;
	switch(Input_w)
	{
		case 26:
			T2Rate = 2;
			break;
		case 13:
			T2Rate = 4;
			break;
		default:
			T2Rate = 1;
			break;
	}
	const unsigned char TRow = (TR-1)*Kernel_stride+Kernel_size;
	int trow_loops = (int)ceil(((float)TRow/T2Rate));
	if(LayerType == 2) trow_loops = 13;

	unsigned int ap_idle;
	unsigned int ap_done;

	unsigned long int PhysicalAddress = YOLO2_BASEADDR;
	int map_len = 0x180;
	int fd = open("/dev/mem", O_RDWR);

	unsigned char *xbase_address;
	xbase_address = (unsigned char *)mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)PhysicalAddress);
	if(xbase_address == MAP_FAILED)
	{
		perror("YOLO2_FPGA:Init Mapping memory for absolute memory access failed.\n");
		return -1;
	}

	while(1)
	{
		ap_idle = ((ReadReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_AP_CTRL) >> 2) && 0x1);
		if(ap_idle)
			break;
	}

//#define WEIGHT_BASE (0x10000000)
//#define BETA_BASE (0x1C25F000)

	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_INPUT_R_DATA,  In_Address0);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_INPUT1_DATA,  In_Address0);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_INPUT2_DATA,  In_Address1);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_INPUT3_DATA,  In_Address1);

	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_OUTPUT_R_DATA, Out_Address);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_OUTPUT1_DATA, Out_Address);
//	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_OUTPUT2_DATA, Out_Address);
//	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_OUTPUT3_DATA, Out_Address);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_WEIGHT_DATA,   WEIGHT_BASE + Weight_offset*4);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_BETA_DATA,     BETA_BASE + Beta_offset*4);

	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_INFM_NUM_DATA, InFM_num);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_OUTFM_NUM_DATA, OutFM_num);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_KERNEL_SIZE_DATA, Kernel_size);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_KERNEL_STRIDE_DATA, Kernel_stride);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_INPUT_W_DATA, Input_w);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_INPUT_H_DATA, Input_h);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_OUTPUT_W_DATA, Output_w);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_OUTPUT_H_DATA, Output_h);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_PADDING_DATA, Padding);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_ISNL_DATA, IsNL);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_ISBN_DATA, IsBN);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_TM_DATA, TM);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_TN_DATA, TN);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_TR_DATA, TR);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_TC_DATA, TC);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_MLOOPS_DATA, mLoops);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_NLOOPS_DATA, nLoops);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_RLOOPS_DATA, rLoops);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_CLOOPS_DATA, cLoops);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_LAYERTYPE_DATA, LayerType);

	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_INPUTQ_DATA, InputQ);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_OUTPUTQ_DATA, OutputQ);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_WEIGHTQ_DATA, WeightQ);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_BETAQ_DATA, BetaQ);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_TROW_LOOPS_DATA, trow_loops);

	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_GIE, 0x0);
	WriteReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_AP_CTRL, 0x1);//Start
	while(1)
	{
		ap_done = ((ReadReg(xbase_address, XYOLO2_FPGA_CTRL_BUS_ADDR_AP_CTRL) >> 1) && 0x1);
		if(ap_done)
			break;
	}
	munmap((void *)xbase_address, map_len);
	close(fd);

	return 0;
}


network* yolov2_hls_ps(network *net, float *input,unsigned int WEIGHT_BASE,unsigned int BETA_BASE,unsigned int MEM_BASE)
{
	int x;

	network* orig = net;
	net->input = input;

	int weight_offset[128] = {864,18432,2048,18432,
		73728,8192,73728,
		8192,73728,
		294912,32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		1179648,131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		4718592,524288,4718592,
		524288,4718592,
		524288,4718592,
		524288,4718592,
		524288,4718592,524288,4718592,524288,4718592,261120,
		131072,
		196608,1179648,131072,1179648,131072,1179648,130560,
		32768,
		49152,294912,32768,294912,32768,294912,65280,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0};
	int beta_offset[128] = {32,64,32,64,
		128,64,128,
		64,128,
		256,128,256,
		128,256,
		128,256,
		128,256,
		128,256,
		128,256,
		128,256,
		128,256,
		512,256,512,
		256,512,
		256,512,
		256,512,
		256,512,
		256,512,
		256,512,
		256,512,
		1024,512,1024,
		512,1024,
		512,1024,
		512,1024,
		512,1024,512,1024,512,1024,255,
		256,
		256,512,256,512,256,512,255,
		128,
		128,256,128,256,128,256,255,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0};
	int offset_index = 0;

	double time1,time2,timecs,timece,timeos,timeoe;

	copy_file2mem((char*)"weightsv3_comb_reorg_ap16.bin",(247583104)/2,WEIGHT_BASE);
	copy_file2mem((char*)"biasv3_comb_ap16.bin",(108276+4)/2,BETA_BASE);

	float* tmpout = (float *)calloc(HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*64,sizeof(float));
	if(!tmpout) printf("tmpout calloc fail\n");
	float* tmpadd = (float *)calloc(HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*64,sizeof(float));
	if(!tmpadd) printf("tmpadd calloc fail\n");
	float* tmpin = (float *)calloc(HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*64,sizeof(float));
	if(!tmpin) printf("tmpin calloc fail\n");
	
	int* tmpout1 = (int *)calloc(HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*64*2,sizeof(int));
	if(!tmpout1) printf("tmpout1 calloc fail\n");
	int* tmpadd1 = (int *)calloc(HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*64*2,sizeof(int));
	if(!tmpadd1) printf("tmpadd1 calloc fail\n");
	int* tmpin1 = (int *)calloc(HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*64*2,sizeof(int));
	if(!tmpin1) printf("tmpin1 calloc fail\n");
	
	float *yolo_buf = tmpin;
	int *yolo_input_buffer = tmpin1;
	float* yolo_out = tmpout;
	int* ylout = tmpout1;
	
	float* upsmp_in = tmpin;
	int* upsmp_in1 = tmpin1;
	float* upsmp_out = tmpout;
	int* upsmp_out1 = tmpout1;
	


#define MEM_LEN (HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*32*4)


	unsigned int Memory_buf = MEM_BASE;

	unsigned int Memory_top1 = Memory_buf;
	unsigned int Memory_mid1 = Memory_top1 + MEM_LEN;
	unsigned int Memory_bot1 = Memory_mid1 + MEM_LEN;

	unsigned int Memory_top2 = Memory_bot1 + 4096;
	unsigned int Memory_mid2 = Memory_top2 + MEM_LEN;
	unsigned int Memory_bot2 = Memory_mid2 + MEM_LEN;

	unsigned int Memory_top3 = Memory_bot2 + 4096;
	unsigned int Memory_mid3 = Memory_top3 + MEM_LEN;
	unsigned int Memory_bot3 = Memory_mid3 + MEM_LEN;

	unsigned int Memory_top4 = Memory_bot3 + 4096;
	unsigned int Memory_mid4 = Memory_top4 + MEM_LEN;
	unsigned int Memory_bot4 = Memory_mid4 + MEM_LEN;

	unsigned int Memory_top5 = Memory_bot4 + 4096;
	unsigned int Memory_mid5 = Memory_top5 + MEM_LEN;
	unsigned int Memory_bot5 = Memory_mid5 + MEM_LEN;


	int in_ptr[107];
	int out_ptr[107];

#define QNUM 75
	int inputQ[QNUM+1];
	int weightQ[QNUM];
	int betaQ[QNUM];
	FILE *Qin;

	Qin = fopen("yolov3_ap16_inout_maxQ_76.bin","rb");
	if(!Qin) file_error((char*)"v3 Qin error 1\n");
	fread(inputQ,sizeof(int),QNUM+1,Qin);
	fclose(Qin);

	Qin = fopen("weightsv3_comb_reorg_ap16_maxQ_75.bin","rb");
	if(!Qin) file_error((char*)"v3 Qin error 2\n");
	fread(weightQ,sizeof(int),QNUM,Qin);
	fclose(Qin);

	Qin = fopen("biasv3_comb_ap16_maxQ_75.bin","rb");
	if(!Qin) file_error((char*)"v3 Qin error 4\n");
	fread(betaQ,sizeof(int),QNUM,Qin);
	fclose(Qin);


#define ROUTE85_LEN (MINIST_NET_SIZE*MINIST_NET_SIZE*512)
#define ROUTE97_LEN (MINIST_NET_SIZE*MINIST_NET_SIZE*1024)

	int *input_tmp_mem = (int *)calloc(HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*32/2,sizeof(int));
	if(!input_tmp_mem) file_error((char*)"input_tmp_mem error \n");
	
	float *test_tmp_mem = (float *)calloc(HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*32/2,sizeof(float));
	if(!test_tmp_mem) file_error((char*)"test_tmp_mem error \n");
	

	float tmpmpmp;

	int i = 0;
	float* test_mem = (float *)calloc(HALF_INPUT_SIZE*HALF_INPUT_SIZE*4*32/2,sizeof(float));
	int tmp_in;
	int firstLayerInput = inputQ[0];
	int firpara = pow(2.0,firstLayerInput);
	short current_in,next_in;
	bool NextPixelInFlag = true;
	int InputPixelOffset = 0;
	int picturesize = HALF_INPUT_SIZE*HALF_INPUT_SIZE*4;
	for(x=0;x<picturesize*3;x++)//1st Layer input Q14
	{
		if(NextPixelInFlag)
		{
			current_in = (short)(input[x]*firpara);
			NextPixelInFlag = false;
		}
		else
		{
			next_in = (short)(input[x]*firpara);
			tmp_in = (next_in<<16) + (current_in);
			//tmp_in = (current_in<<16) + (next_in);
			input_tmp_mem[InputPixelOffset] = tmp_in;
			InputPixelOffset++;
			NextPixelInFlag = true;
		}
	}
    copy_mem2dev((uint8_t *)input_tmp_mem,picturesize*3*4/2, MEM_BASE);
	//copy_mem2dev((uint8_t *)input_tmp_mem,picturesize*3*4/2, in_ptr[0]);
	free(input_tmp_mem);

	in_ptr[0] = Memory_top1;
	//out_ptr[0] = Memory_bot1 - net->layers[0].outputs*4/2;
	out_ptr[0] = Memory_mid1;
	//res1
	in_ptr[1] = out_ptr[0];
	//out_ptr[1] = Memory_top2;//L1,save in Buff1
	out_ptr[1] = Memory_top1;

	in_ptr[2] = Memory_top1;
	//out_ptr[2] = Memory_bot5 - net->layers[2].outputs*4/2;//L2
	out_ptr[2] = Memory_mid1;

	in_ptr[3] = out_ptr[2];
	//out_ptr[3] = Memory_top5;//L3
	out_ptr[3] = Memory_top2;//L3

	//in_ptr[4] = Memory_top5;
	//out_ptr[4] = Memory_bot5 - net->layers[4].outputs*4/2;//L4
	in_ptr[4] = Memory_top2;
	out_ptr[4] = Memory_mid2;
	//res2
	in_ptr[5] = out_ptr[4];
	out_ptr[5] = Memory_top3;

	//L5,save in Buff3

	in_ptr[6] = Memory_top3;
	out_ptr[6] = Memory_top1;//L6

	in_ptr[7] = Memory_top1;
	out_ptr[7] = Memory_bot1 - net->layers[7].outputs*4/2;//L7

	in_ptr[8] = out_ptr[7];
	out_ptr[8] = Memory_top1;//L8,save in Buff1

	in_ptr[9] = Memory_top1;
	out_ptr[9] = Memory_bot5 - net->layers[9].outputs*4/2;//L9

	in_ptr[10] = out_ptr[9];
	out_ptr[10] = Memory_top5;//L10

	in_ptr[11] = Memory_top5;
	out_ptr[11] = Memory_bot5 - net->layers[11].outputs*4/2;//L11
	//res8
	in_ptr[12] = out_ptr[11];
	out_ptr[12] = Memory_top5;//L12,save in Buff5

	in_ptr[13] = Memory_top5;
	out_ptr[13] = Memory_bot5 - net->layers[13].outputs*4/2;//L13

	in_ptr[14] = out_ptr[13];
	out_ptr[14] = Memory_top1;//L14

	in_ptr[15] = Memory_top1;
	out_ptr[15] = Memory_bot1 - net->layers[15].outputs*4/2;//L15,save in buff1

	in_ptr[16] = out_ptr[15];
	out_ptr[16] = Memory_top4;

	in_ptr[17] = Memory_top4;
	//out_ptr[17] = Memory_bot5 - net->layers[17].outputs*4/2;
	out_ptr[17] = Memory_mid5;

	in_ptr[18] = out_ptr[17];
	out_ptr[18] = Memory_top5;//L18,save in buff5


	in_ptr[19] = Memory_top5;
	out_ptr[19] = Memory_bot1 - net->layers[19].outputs*4/2;//L19

	in_ptr[20] = out_ptr[19];
	out_ptr[20] = Memory_top1;//L20

	in_ptr[21] = Memory_top1;
	out_ptr[21] = Memory_bot1 - net->layers[21].outputs*4/2;//L21,save in buff1


	in_ptr[22] = out_ptr[21];
	out_ptr[22] = Memory_top5;

	in_ptr[23] = Memory_top5;
	out_ptr[23] = Memory_bot5 - net->layers[23].outputs*4/2;

	in_ptr[24] = out_ptr[23];
	out_ptr[24] = Memory_top5;//L24,save in buff5


	in_ptr[25] = Memory_top5;
	out_ptr[25] = Memory_bot5 - net->layers[25].outputs*4/2;

	in_ptr[26] = out_ptr[25];
	out_ptr[26] = Memory_top1;

	in_ptr[27] = Memory_top1;
	out_ptr[27] = Memory_bot1 - net->layers[27].outputs*4/2;//L27,save in buff1


	in_ptr[28] = out_ptr[27];
	out_ptr[28] = Memory_top5;

	in_ptr[29] = Memory_top5;
	out_ptr[29] = Memory_bot5 - net->layers[29].outputs*4/2;

	in_ptr[30] = out_ptr[29];
	out_ptr[30] = Memory_top5;//L30,save in buff5


	in_ptr[31] = Memory_top5;
	out_ptr[31] = Memory_bot1 - net->layers[31].outputs*4/2;

	in_ptr[32] = out_ptr[31];
	out_ptr[32] = Memory_top1;

	in_ptr[33] = Memory_top1;
	out_ptr[33] = Memory_bot1 - net->layers[33].outputs*4/2;//L33,save in buff1


	in_ptr[34] = out_ptr[33];
	out_ptr[34] = Memory_top5;

	in_ptr[35] = Memory_top5;
	//out_ptr[35] = Memory_bot5 - net->layers[35].outputs*4/2;
	out_ptr[35] = Memory_mid5;

	in_ptr[36] = out_ptr[35];
	out_ptr[36] = Memory_top2+ROUTE97_LEN;//L30,save in buff5
	//out_ptr[36] = Memory_top5;

	//res8
	in_ptr[37] = out_ptr[36];
	//out_ptr[37] = Memory_bot5 - net->layers[37].outputs*4/2;
	out_ptr[37] = Memory_mid5;

	in_ptr[38] = out_ptr[37];
	out_ptr[38] = Memory_top1;

	in_ptr[39] = Memory_top1;
	out_ptr[39] = Memory_bot1 - net->layers[39].outputs*4/2;

	in_ptr[40] = out_ptr[39];
	out_ptr[40] = Memory_top1;//L40,save in Buff1

	in_ptr[41] = Memory_top1;
	//out_ptr[41] = Memory_bot4 - net->layers[41].outputs*4;
	out_ptr[41] = Memory_mid2;

	in_ptr[42] = out_ptr[41];
	out_ptr[42] = Memory_top4;

	in_ptr[43] = Memory_top4;
	//out_ptr[43] = Memory_bot4 - net->layers[43].outputs*4/2;//L43,save in Buff4
	out_ptr[43] = Memory_mid4;

	in_ptr[44] = out_ptr[43];
	out_ptr[44] = Memory_top1;

	in_ptr[45] = Memory_top1;
	out_ptr[45] = Memory_bot1 - net->layers[43].outputs*4/2;

	in_ptr[46] = out_ptr[45];
	out_ptr[46] = Memory_top1;//L46,save in Buff1


	in_ptr[47] = Memory_top1;
	out_ptr[47] = Memory_bot4 - net->layers[47].outputs*4;

	in_ptr[48] = out_ptr[47];
	out_ptr[48] = Memory_top4;

	in_ptr[49] = Memory_top4;
	out_ptr[49] = Memory_bot4 - net->layers[49].outputs*4/2;//L49,save in Buff4


	in_ptr[50] = out_ptr[49];
	out_ptr[50] = Memory_top1;

	in_ptr[51] = Memory_top1;
	out_ptr[51] = Memory_bot1 - net->layers[51].outputs*4/2;

	in_ptr[52] = out_ptr[51];
	out_ptr[52] = Memory_top1;//L52,save in Buff1


	in_ptr[53] = Memory_top1;
	out_ptr[53] = Memory_bot4 - net->layers[53].outputs*4;

	in_ptr[54] = out_ptr[53];
	out_ptr[54] = Memory_top4;

	in_ptr[55] = Memory_top4;
	out_ptr[55] = Memory_bot4 - net->layers[55].outputs*4/2;//L55,save in Buff4

	in_ptr[56] = out_ptr[55];
	out_ptr[56] = Memory_top1;

	in_ptr[57] = Memory_top1;
	out_ptr[57] = Memory_bot1 - net->layers[57].outputs*4/2;

	in_ptr[58] = out_ptr[57];
	out_ptr[58] = Memory_top1;//L58,save in Buff1


	in_ptr[59] = Memory_top1;
	out_ptr[59] = Memory_bot4 - net->layers[59].outputs*4;

	in_ptr[60] = out_ptr[59];
	out_ptr[60] = Memory_top4;

	in_ptr[61] = Memory_top4;
	//out_ptr[61] = Memory_bot4 - net->layers[61].outputs*4/2;//L61,save in Buff4
	//out_ptr[61] = Memory_mid4;
	out_ptr[61] = Memory_mid4+ROUTE85_LEN;

	//res4
	in_ptr[62] = out_ptr[61];
	out_ptr[62] = Memory_top4;//L62

	in_ptr[63] = Memory_top4;
	out_ptr[63] = Memory_bot1 - net->layers[63].outputs*4*2;

	in_ptr[64] = out_ptr[63];
	out_ptr[64] = Memory_top1;

	in_ptr[65] = Memory_top1;
	out_ptr[65] = Memory_bot1 - net->layers[65].outputs*4;//L65,save in Buff1

	in_ptr[66] = out_ptr[65];
	out_ptr[66] = Memory_top3;

	in_ptr[67] = Memory_top3;
	out_ptr[67] = Memory_bot3 - net->layers[67].outputs*4;

	in_ptr[68] = out_ptr[67];
	out_ptr[68] = Memory_top3;//L68,save in Buff3


	in_ptr[69] = Memory_top3;
	out_ptr[69] = Memory_bot1 - net->layers[69].outputs*4*2;

	in_ptr[70] = out_ptr[69];
	out_ptr[70] = Memory_top1;

	in_ptr[71] = Memory_top1;
	out_ptr[71] = Memory_bot1 - net->layers[71].outputs*4;//L71,save in Buff1

	in_ptr[72] = out_ptr[71];
	out_ptr[72] = Memory_top3;

	in_ptr[73] = Memory_top3;
	out_ptr[73] = Memory_bot3 - net->layers[73].outputs*4;

	in_ptr[74] = out_ptr[73];
	out_ptr[74] = Memory_top3;//L74,save in Buff3

	//DBL*5
	for(x = 75; x <= 79; x++){
		if(x % 2 == 0){
			in_ptr[x] = out_ptr[x-1];
			out_ptr[x] = Memory_top3;
		} else {
			in_ptr[x] = Memory_top3;
			//out_ptr[x] = Memory_bot3 - net->layers[x].outputs*4*2;
			out_ptr[x] = Memory_mid3;
		}
	}

	//DBL
	in_ptr[80] = out_ptr[79];
	out_ptr[80] = Memory_top1;
	//conv
	in_ptr[81] = Memory_top1;
	//out_ptr[81] = Memory_bot1 - (net->layers[81].outputs+MINIST_NET_SIZE*MINIST_NET_SIZE)*16;
	out_ptr[81] = Memory_mid1;
	//y1
	in_ptr[82] = out_ptr[81];
	out_ptr[82] = Memory_top3;

	in_ptr[83] = out_ptr[79];
	out_ptr[83] = out_ptr[79];

	//route 不写

	//DBL
	in_ptr[84] = out_ptr[79];
	out_ptr[84] = Memory_top1;
	//UPSAMPLE
	in_ptr[85] = Memory_top1;
	//out_ptr[85] = Memory_bot1 - net->layers[85].outputs*4/2 - ROUTE61_LEN;
	//out_ptr[85] = Memory_mid1;
	out_ptr[85] = Memory_mid4;

	//concate
	//memcpy(Memory_bot1 - ROUTE61_LEN,out_ptr[61],ROUTE61_LEN);
	in_ptr[86] = out_ptr[85];
	//out_ptr[86] = Memory_top4;
	out_ptr[86] = Memory_mid4;

	//DBL*5
	in_ptr[87] = out_ptr[86];
	out_ptr[87] = Memory_top1;

	in_ptr[88] = Memory_top1;
	out_ptr[88] = Memory_bot1 - net->layers[88].outputs*4/2;

	in_ptr[89] = out_ptr[88];
	out_ptr[89] = Memory_top1;

	in_ptr[90] = Memory_top1;
	out_ptr[90] = Memory_bot1 - net->layers[90].outputs*4/2;

	in_ptr[91] = out_ptr[90];
	out_ptr[91] = Memory_top1;

	//DBL
	in_ptr[92] = Memory_top1;
	out_ptr[92] = Memory_bot2 - net->layers[92].outputs*4/2;
	//conv
	in_ptr[93] = out_ptr[92];
	out_ptr[93] = Memory_top3;
	//yolo
	in_ptr[94] = Memory_top3;
	out_ptr[94] = Memory_top4;

	//L95,不写
	in_ptr[95] = Memory_top1;
	out_ptr[95] = Memory_top1;
	//DBL
	in_ptr[96] = Memory_top1;
	out_ptr[96] = Memory_top3;
	//conv
	in_ptr[97] = Memory_top3;
	//out_ptr[97] = Memory_bot1 - net->layers[97].outputs*4/2 - ROUTE36_LEN;
	//out_ptr[97] = Memory_mid1;
	out_ptr[97] = Memory_top2;


	//L98
	//memcpy(Memory_bot1 - ROUTE36_LEN,out_ptr[36],ROUTE36_LEN);
	in_ptr[98] = out_ptr[97];
	//out_ptr[98] = Memory_top1;
	out_ptr[98] = Memory_top5;

	in_ptr[99] = out_ptr[98];
	out_ptr[99] = Memory_top2;
	in_ptr[100] = Memory_top2;
	out_ptr[100] = Memory_bot1 - net->layers[100].outputs*4/2;

	in_ptr[101] = out_ptr[100];
	out_ptr[101] = Memory_top1;
	in_ptr[102] = Memory_top1;
	out_ptr[102] = Memory_bot1 - net->layers[102].outputs*4/2;
	in_ptr[103] = out_ptr[102];
	out_ptr[103] = Memory_top1;
	in_ptr[104] = Memory_top1;
	out_ptr[104] = Memory_bot1 - net->layers[104].outputs*4/2;
	in_ptr[105] = out_ptr[104];
	out_ptr[105] = Memory_top1;
	in_ptr[106] = Memory_top1;
	out_ptr[106] = Memory_mid1;

    network netp = *net;
    //int i;
	int j;
	int n=0,b=0;
	int woffset = 0;
	int boffset = 0;
	int TR,TC,TM,TN;
	int output_w,output_h;
	int rLoops,cLoops,mLoops,nLoops;
	double time_sum = 0.0;
	double time_sum_conv = 0.0;
	double time_sum_othr = 0.0;
	double convtime[128];
	double alltime[128];
	double othertime[64];
	
	int other_offset = 0;

//	struct timespec ts, ts1;
//	ts.tv_nsec = 50000000;
//    ts.tv_sec = 1;

	int INPUTQ;
	int *Output_ptr = NULL;
	int ii,jj,kk;
	int* tmp_out = NULL;
	int* tmp_ipt = NULL;
	int* tmp_add = NULL;
	int* tmp_up_in = NULL;
	int* tmp_up_out = NULL;
	int layerparai = 0;

	int temp_inputQ[128];
	temp_inputQ[0] = inputQ[0];

	time1 = what_time_is_it_now();
	printf("Load End %lf\n",time1);
	printf("Load end in %ld\n",getTimeStampMill());
    for(i = 0; i < netp.n; ++i)
	{
        netp.index = i;
        layer l = netp.layers[i];
		printf("Layer[%2d],Output=%d: ",i,l.outputs*4/2);
		switch(l.type)
		{
			case CONVOLUTIONAL:{
				temp_inputQ[i+1]=inputQ[offset_index+1];
				if(i == 97){
					temp_inputQ[i+1] = temp_inputQ[36+1];
				}
				if(i == 85){
					temp_inputQ[i+1] = temp_inputQ[61+1];
				}
				if(i == 23 || i == 14 || i == 17 || i == 26 || i == 35){
					temp_inputQ[i+1] = temp_inputQ[i+1] - 1;
				}

				printf("outputMemory:%8d;BN=%d;Activation=%d;conv  %5d %2d x%2d /%2d  %4d x%4d x%4d   ->  %4d x%4d x%4d  %5.3f BFLOPs\n",l.outputs,l.batch_normalize,l.activation, l.n, l.size, l.size, l.stride, l.w, l.h, l.c, l.out_w, l.out_h, l.out_c, (2.0 * l.n * l.size*l.size*l.c/l.groups * l.out_h*l.out_w)/1000000000.);


				
				output_w = (l.w - l.size + 2*l.pad)/l.stride + 1 ;
				output_h = (l.h - l.size + 2*l.pad)/l.stride + 1 ;

				TR = MIN(((OnChipIB_Height-l.size)/l.stride+1),Tr);//keep Kernel_stride>=1
				TR = MIN(output_h,TR);
				TC = MIN(((OnChipIB_Width-l.size)/l.stride+1),Tc);
				TC = MIN(output_w,TC);
				TM = MIN(l.n,Tm);
				TN = MIN(l.c,Tn);

				rLoops = (int)ceil(((float)output_h)/TR);
				cLoops = (int)ceil(((float)output_w)/TC);
				mLoops = (int)ceil(((float)l.n)/TM);
			    nLoops = (int)ceil(((float)l.c)/TN);

				INPUTQ = temp_inputQ[i];

			    timecs = what_time_is_it_now();

				YOLO2_FPGA(in_ptr[i],in_ptr[i],out_ptr[i],woffset/2,boffset/2,
					l.c,l.n,l.size,
					l.stride,l.w,l.h,output_w,output_h,
					l.pad,l.activation==LEAKY?1:0,l.batch_normalize?1:0,
					TM,TN,TR,TC,
					mLoops,nLoops,rLoops,cLoops,0,
					INPUTQ,temp_inputQ[i+1],weightQ[offset_index],betaQ[offset_index],
					WEIGHT_BASE,BETA_BASE);
				timece = what_time_is_it_now();
				time_sum_conv += (timece - timecs);
				convtime[offset_index] = timece - timecs;
				alltime[i] = timece - timecs;
				printf("TIME_SUM Predicted in %f seconds.\n",timece - timecs);

				woffset += weight_offset[offset_index];
				boffset += beta_offset[offset_index];
				offset_index++;

				break;
			}
			case ROUTE:{
				temp_inputQ[i+1] = temp_inputQ[i];
				printf("outputMemory:%8d;route ",l.outputs);
				for(j = 0; j < l.n; ++j){
					printf(" %d", l.input_layers[j]);
				}
				printf("\n");
				timeos = what_time_is_it_now();
				timeoe = what_time_is_it_now();
				othertime[other_offset] = 0;
				alltime[i] = 0;

				other_offset++;
				break;
			}

			case UPSAMPLE:{
				temp_inputQ[i+1] = temp_inputQ[i];
				printf("upsample           %2dx  %4d x%4d x%4d   ->  %4d x%4d x%4d\n", l.stride, l.w, l.h, l.c, l.out_w, l.out_h, l.out_c);

				output_w = l.out_w;
				output_h = l.out_h;
				
				TR = 13;
				TC = 13;
				TN = MIN(l.c,Tn);
				TM = TN;
				
				rLoops = (int)ceil(((float)l.h)/TR);
				cLoops = (int)ceil(((float)l.w)/TC);
				//mLoops = (int)ceil(((float)l.n)/TM);
			    nLoops = (int)ceil(((float)l.c)/TN);
				mLoops = nLoops;
				
				timeos = what_time_is_it_now();
				YOLO2_FPGA(in_ptr[i],in_ptr[i],out_ptr[i],NULL,NULL,l.c,l.c,
							1,l.stride,
							l.w,l.h,output_w,output_h,
							l.pad,0,0,
							TM,TN,TR,TC,
							mLoops,nLoops,rLoops,cLoops,2,
							temp_inputQ[i],temp_inputQ[i+1],INTERWIDTH,INTERWIDTH,WEIGHT_BASE,BETA_BASE);
				timeoe = what_time_is_it_now();
				othertime[other_offset] = timeoe - timeos;
				alltime[i] = timeoe - timeos;
				printf("TIME_SUM Predicted in %f seconds.\n",timeoe - timeos);

				other_offset++;
				break;

			}

			case SHORTCUT:{
				temp_inputQ[i+1] = temp_inputQ[i];
			    printf("res  %3d                %4d x%4d x%4d   ->  %4d x%4d x%4d\n",l.index, netp.layers[i-1].w,netp.layers[i-1].h,netp.layers[i-1].n, l.w,l.h,l.c);

				
				output_w = l.out_h;
				output_h = l.out_w;
				
				TR = MIN(output_h,Tr);
				TC = MIN(output_w,Tc);
				TN = MIN(l.c,Tn);
				TM = TN;

				rLoops = (int)ceil(((float)output_h)/TR);
				cLoops = (int)ceil(((float)output_w)/TC);
				nLoops = (int)ceil(((float)l.c)/TN);
				mLoops = nLoops;

				timeos = what_time_is_it_now();
				YOLO2_FPGA(out_ptr[i-1],out_ptr[l.index],out_ptr[i],NULL,NULL,l.c,l.c,
							1,1,
							l.w,l.h,output_w,output_h,
							l.pad,0,0,
							TM,TN,TR,TC,
							mLoops,nLoops,rLoops,cLoops,1,
							temp_inputQ[l.index+1],temp_inputQ[i],INTERWIDTH,INTERWIDTH,WEIGHT_BASE,BETA_BASE);
							
				timeoe = what_time_is_it_now();
				othertime[other_offset] = timeoe - timeos;
				alltime[i] = timeoe - timeos;
				printf("TIME_SUM Predicted in %f seconds.\n",timeoe - timeos);

				other_offset++;
				break;
			}
			case YOLO:
			{
				temp_inputQ[i+1] = temp_inputQ[i];
				printf("outputMemory:%8d;YOLO！\n",l.outputs);

				timeos = what_time_is_it_now();
				
				l.output = (float*)calloc(l.outputs,sizeof(float));
				copy_dev2mem((uint8_t *)yolo_input_buffer,l.outputs*4/2, in_ptr[i]);
				Output_ptr = (int *)(yolo_input_buffer);
				bool NextPixelFlag = true;
				int OutputPixelOffset = 0;
				short current_p,next_p,output_p;
				//printf("temp_inputQ=%d\n",temp_inputQ[i+1]);
				for(j=0;j<l.outputs;j++)
				{
					if(NextPixelFlag)
					{
						int tmp_p = Output_ptr[OutputPixelOffset];
						OutputPixelOffset++;
						current_p = tmp_p;
						next_p = tmp_p >> 16;
						output_p = current_p;
						//output_p = next_p;
						NextPixelFlag = false;
					}else
					{
						output_p = next_p;
						//output_p = current_p;
						NextPixelFlag = true;
					}
					//yolo_buf[j] = output_p*LayerPara[layerparai];
					yolo_buf[j] = output_p*pow(2.0,-temp_inputQ[i+1]);
				}
				//layerparai = layerparai + 1;
				netp.layers[i].output = forward_yolo_array(l,yolo_buf);
				timeoe = what_time_is_it_now();
				othertime[other_offset] = timeoe - timeos;
				alltime[i] = timeoe - timeos;
				printf("TIME_SUM Predicted in %f seconds.\n",timeoe - timeos);

				timeoe = what_time_is_it_now();
				time_sum_othr += (timeoe - timeos);
				other_offset++;
				break;
			}
			
			//my code end
		}
		netp.input = l.output;

    }
	printf("Predicted end in %ld\n",getTimeStampMill());
	time2 = what_time_is_it_now();
    printf("TIME_SUM Predicted in %f seconds.\n",time2 - time1);

	free(tmpin);
	free(tmpadd);
	free(tmpout);
	free(tmpin1);
	free(tmpadd1);
	free(tmpout1);

	return net;

}


//////////////////////////HLS end



#endif /* SRC_YOLOV3_H_ */

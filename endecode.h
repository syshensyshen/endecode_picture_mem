/*
作者沈圣远-syshen
使用libjpeg9开源库
介绍libjpeg对内存中的图片数据进行编解码操作，并对cJSON的用法做出示范
使用opencv对解码图片显示
此代码为公司的算法封装代码，去掉了人脸识别算法包的调用
*/
//使用libjpeg解码
extern "C"{
#include "jpeglib.h"
#include "cJSON.h"
};

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/time.h>
#include <pthread.h> 
#include <string>
#include <vector>
//内存管理头文件
#include "apr-1/apr_pools.h"
#include "apr-1/apr_shm.h"
#include "apr-1/apr.h"

using namespace cv;



//全局的内存池结构
apr_pool_t *root;
//#define JSON_DATA_LENGTH 400
//

typedef int Model_type;


typedef struct _rect
{
	int left;
	int top;
	int width;
	int height;
} RECT;

typedef int Boolean;

#define MAX_FACE_RECT 1000*1000*3
#define MAX_JSON_LENGTH 2048
static char result_faceinfo_json[MAX_JSON_LENGTH];

const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Create_Pool();
int Destroy_Pool();



//解码
int Decode(const char *src, Mat &image, uint32_t len);

//model有MG_DETECTION_MODE_NORMAL和MG_DETECTION_MODE_TRACKING_FAST，分别表示静态检测和视频检测
//count为人脸的数量
char *Detect_Face(int model, uint8_t Image[]);

//base64编码
char * base64_encode( const unsigned char * bindata, char * base64, int binlength );
//json解析
int parseJson(char *src_json_data, RECT *rect);
//编码
int Encode_to_Huffman(Mat &image, char **dst);
int Encode_Image(cv::Mat &image, int *dst_image_length, unsigned char **image_data);

void write_JPEG_file (char * filename, int quality);
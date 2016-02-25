#include "endecode.h"
int main(int argc, char **argv)
{
	unsigned long long int i;
	Create_Pool();
	RECT result_rect;
	char jsondata[100] = "{\"left\":269,\"top\":359,\"width\":259,\"height\":259}";
	parseJson(jsondata, &result_rect);
	for(;;)
	{	
		char *dst = Detect_Face(0, NULL);
		printf("dst json is: %s\r\n", dst);
		Get_Feature_Info(NULL, 0, NULL);
		i++;
		printf("%llu\r\n", i);
	}
	Destroy_Pool();
	return 0;
}

char *Detect_Face(int model, uint8_t Image[])
{
	//检测是否存在人脸句柄
	
	//
	Mat image; //= imread("Lena.jpg", 0);
	//MG_DT_HANDLE handle_start1 = MG_DT_CreateHandle();
	//handle_result = MG_DT_CreateResult(NULL);
	//读入文件测试
	FILE *pFile = fopen("1.jpg", "r");
	fseek(pFile,0,SEEK_END);
	int len = ftell(pFile);
	rewind(pFile);
	
	char *pBuff = new char[len];
	fread(pBuff, 1, len, pFile);
	pBuff[len] = 0;
	fclose(pFile);
	printf("\r\n");	
	Decode(pBuff, image, len);
	cvNamedWindow("precious");
	cv::imshow("precious", image);
	cvWaitKey(0);
	cvDestroyWindow("precious");
	delete [] pBuff;
	//检测人脸信息
	//组装jSON数据
	//result_faceinfo_json[MAX_JSON_LENGTH];
	memset(result_faceinfo_json, '\0', MAX_JSON_LENGTH);
	char *tmp_faceinfo_json;
	cJSON *root=cJSON_CreateObject();
	cJSON *face, *rect1, *attrs, *quality, *crop, *rect2, *image_rect;
	cJSON_AddNumberToObject(root, "id", 0);
	//
	cJSON_AddItemToObject(root, "face", face = cJSON_CreateObject());
	//
	cJSON_AddItemToObject(face, "rect", rect1 = cJSON_CreateObject());
	cJSON_AddNumberToObject(rect1,"left",    0);
	cJSON_AddNumberToObject(rect1,"top",     0);
	cJSON_AddNumberToObject(rect1,"width",   0);
	cJSON_AddNumberToObject(rect1,"height",	 0);
	//
	
	cJSON_AddItemToObject(face, "attrs", attrs= cJSON_CreateObject());
	cJSON_AddNumberToObject(attrs, "yaw",      0);
	cJSON_AddNumberToObject(attrs, "pitch",    0);
	cJSON_AddNumberToObject(attrs, "motion",   0);
	cJSON_AddNumberToObject(attrs, "gaussian", 0);	
	
	//
	cJSON_AddNumberToObject(face, "quality", 80);
		
	//
	cJSON_AddItemToObject(face, "crop", crop = cJSON_CreateObject());
	cJSON_AddItemToObject(crop, "rect2", rect2 = cJSON_CreateObject());
	cJSON_AddNumberToObject(rect2,"left",    0);
	cJSON_AddNumberToObject(rect2,"top",     0);
	cJSON_AddNumberToObject(rect2,"width",   0);
	cJSON_AddNumberToObject(rect2,"height",	 0);
	
	cJSON_AddItemToObject(crop, "image", cJSON_CreateString(""));
	cJSON_AddItemToObject(root, "image_rect", image_rect = cJSON_CreateObject());
	cJSON_AddNumberToObject(image_rect,"left",    0);
	cJSON_AddNumberToObject(image_rect,"top",     0);
	cJSON_AddNumberToObject(image_rect,"width",   0);
	cJSON_AddNumberToObject(image_rect,"height",  0);
	
	cJSON_AddItemToObject(root, "crop", crop = cJSON_CreateObject());
	cJSON_AddNumberToObject(crop,"left",    0);
	cJSON_AddNumberToObject(crop,"top",     0);
	cJSON_AddNumberToObject(crop,"width",   0);
	cJSON_AddNumberToObject(crop,"height",	0);
	
	//泄露
	tmp_faceinfo_json = cJSON_Print(root);
	
	cJSON_Delete(root);
	
	printf("tmp_faceinfo_json %s and length is: %d\r\n",tmp_faceinfo_json, (int)strlen(tmp_faceinfo_json));
	//释放句柄
	strcpy(result_faceinfo_json, tmp_faceinfo_json);
	result_faceinfo_json[strlen(tmp_faceinfo_json)+1] = '\0';
	printf("result_faceinfo_json %s\r\n",result_faceinfo_json);
	free(tmp_faceinfo_json);
	int length_encode_length;
	apr_pool_t *apr_encode_pool;
	apr_pool_create(&apr_encode_pool, ::root);
	unsigned char *image_data = (unsigned char *)apr_palloc(apr_encode_pool, image.cols*image.rows*3);
	Encode_Image(image, &length_encode_length, &image_data);
	FILE *file_encode = fopen("12.jpg", "w+");
	fwrite(image_data, length_encode_length, 1, file_encode);
	fclose(file_encode);
	//printf("%s\r\n", image_data);
	apr_pool_destroy(apr_encode_pool);
	return result_faceinfo_json;
}

int Decode(const char *src, Mat &image, uint32_t len)
{
	//使用libjpeg解码内存中的数据流
	printf("enter the decode function\r\n");
	struct jpeg_error_mgr jerr;
	int row_stride;
	if(NULL == src)
	{
		printf("image is NULL\r\n");
		return -1;
	}
	//初始化解码信息
	struct jpeg_decompress_struct cinfo;  
	//将cinfo的错误信息与jerr绑定
	cinfo.err = jpeg_std_error(&jerr);
	printf("init and err bind sucessful\r\n");
	//解码完成自动填充
	//cinfo.input_components = 3;  /* # of color components per pixel */  
	//cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
	//设置解码图片与原图的压缩比
	cinfo.scale_num=1;
	cinfo.scale_denom=1;
	//设置输出图像为RGB图像
	cinfo.out_color_space=JCS_RGB;
	//创建解码器
	jpeg_create_decompress(&cinfo);
	printf("create decompress sucessful\r\n");
	//
	jpeg_mem_src(&cinfo, (uint8_t*)src, len);
	printf("input buffer sucessful\r\n");
	//jpeg_mem_src(&cinfo, (uint8_t*)src, len);
	//1.设置读取jpg文件头部，Read file header, set default decompression parameters
    (void) jpeg_read_header(&cinfo, boolean(TRUE));
	printf("read header sucessful\r\n");
    //2.开始解码数据 Start decompressor  
    (void) jpeg_start_decompress(&cinfo); 
	printf("start decompress\r\n");
	//获取图像的高度和宽度
	int width = cinfo.image_width ;
	int height = cinfo.image_height ;
	IplImage *pImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	printf("image width is %d\n", cinfo.image_width);
	/* nominal image width (from SOF marker) */ 
    printf("image height is %d\n", cinfo.image_height);
	/* nominal image width (from SOF marker) */ 
    printf("numcom is %d\n", cinfo.num_components);
	/* nominal image width (from SOF marker) */ 
	//分配buffer数据
	int row_width = cinfo.output_width * cinfo.output_components;
	//会产生内存碎片，采用apr内存管理
	apr_pool_t *apr_decode_pool;
    apr_pool_create(&apr_decode_pool,root);
    unsigned char *output_buffer = (unsigned char *)apr_palloc(apr_decode_pool, row_width);
	unsigned char *output_temp = (unsigned char *)apr_palloc(apr_decode_pool, row_width);
	
	//内存碎片太多
	//unsigned char *output_buffer = (unsigned char *)malloc(row_width );
	//unsigned char *output_temp = (unsigned char *)malloc(row_width);	
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_width, 1);
	printf("get buffer sucessful\r\n");
	char *data = pImage->imageData;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		int count;
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy(output_buffer, *buffer, row_width);
		for (count=0;count<width;count++) 
		{
			//printf("head in for loop\r\n");
			//opencv的图像存储为BGR，为保持一致所以采用这种形式
			//出错点
			//测试解码通过
			unsigned char *copy_data = output_buffer;
			output_temp[count*cinfo.output_components+2] = *(copy_data+count*3+2);
			//printf("test %d\r\n", *(copy_data+2));
			output_temp[count*cinfo.output_components+1] = *(copy_data+count*3+1);
			//printf("test %d\r\n", *(copy_data+1));
			output_temp[count*cinfo.output_components] = *(copy_data+count*3);
			//printf("test %d\r\n", *(copy_data));
			if(count != width-1)
				copy_data += 3;
			//printf("temp is: %s\r\n", output_temp);
		}
		memcpy(data, output_temp, row_width);
		//memcpy(data, *buffer, row_width);
		data += row_width;
	}
	//apr_pool_destroy(apr_decode_pool);
	printf("decode sucessful\r\n");
	//解码完成释放数据
	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);	
	printf("destroy sucessful\r\n");
	//实现IplImage构造Mat
	//cv::Mat decode_image = cvarrToMat(pImage);
	image = cvarrToMat(pImage, true);
	//速度下降很多
	//cvtColor(decode_image, image, CV_RGB2BGR);
	//image = Mat(pImage, 1);
	printf("image width is: %d, height is: %d channels is: %d\r\n", image.cols, image.rows, image.channels());
	/*FILE *file = fopen("12.jpg", "w+");
	fwrite(pImage->imageData, row_width*image.rows, 1, file);
	fclose(file);*/
	cvReleaseImage(&pImage);
	
	return 0;
}

int Create_Pool()
{
	apr_pool_initialize();
    apr_status_t error_msg;
    error_msg = apr_pool_create(&root, NULL);
    if (error_msg != APR_SUCCESS) 
	{
        fprintf(stderr, "apr_create_pool error\n");
        return -1;
    }
	/*apr_shm_t *shm;
	rc = apr_shm_create(&shm, 20008, file, pool);
	if (rc != APR_SUCCESS)
	{
		fprintf(stderr, "apr_create_shm error\n");
		return -1;
	}*/  
	//
    
	return 0;
}
int Destroy_Pool()
{
	if(NULL == root)
	{
		apr_pool_destroy(root);
	}
	apr_pool_terminate();//释放全局池，释放全局allocator，将内存归还给系统
	return 0;
}

char *base64_encode( const unsigned char * bindata, char * base64, int binlength )
{
    int i, j;
    unsigned char current;

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return base64;
}

int parseJson(char * src_json_data, RECT *rect)
{
	if(NULL == src_json_data)
    {
		printf("input json data is empty\r\n");
		return -1;
    }
    cJSON * pJson = cJSON_Parse(src_json_data);
    if(NULL == pJson)                                                                                         
    {
		printf("json 解析失败\r\n");
		return -1;
    }
    cJSON * pSub = cJSON_GetObjectItem(pJson, "left");
    if(NULL == pSub)
    {
        printf("json 解析失败\r\n");
		return -1;
    }
	rect->left = pSub->valueint;
    printf("top : %d\r\n", pSub->valueint);
    pSub = cJSON_GetObjectItem(pJson, "top");
    if(NULL == pSub)
    {
        printf("json 解析失败\r\n");
		return -1;
    }
	rect->top = pSub->valueint;
    printf("bottom : %d\r\n", pSub->valueint);
    pSub = cJSON_GetObjectItem(pJson, "width");
    if(NULL == pSub)
    {
        printf("json 解析失败\r\n");
		return -1;
    }
	rect->width = pSub->valueint;
    printf("width : %d\r\n", pSub->valueint);
    pSub = cJSON_GetObjectItem(pJson, "height");
	if(NULL == pSub)
    {
        printf("json 解析失败\r\n");
		return -1;
    }
	rect->width = pSub->valueint;
    printf("height : %d\r\n", pSub->valueint);
    /*cJSON * pSubSub = cJSON_GetObjectItem(pSub, "subjsonobj");
    if(NULL == pSubSub)
    {
        // get object from subject object faild
    }
    printf("sub_obj_1 : %s\n", pSubSub->valuestring);*/
 
    cJSON_Delete(pJson);	
 }
 
int Encode_Image(cv::Mat &image, int *dst_image_length, unsigned char **image_data)
{
	printf("encode begin\r\n");
	int row_width = image.cols*image.rows*image.channels();
	printf("%d\r\n", row_width);
	//BGR转RGB
	apr_pool_t *apr_encode_image_pool;
    apr_pool_create(&apr_decode_pool,root);
	unsigned char *Convert_RGB_buffer = (unsigned char *)apr_palloc(apr_encode_image_pool, row_width);
	unsigned char *Convert_RGB_Src = image.data;
	//printf("image data is: %s\r\n", image.data);
	printf("convert begin\r\n");
	for(int i=0; i<image.cols*image.rows; i++)
	{
		*Convert_RGB_buffer =  *(Convert_RGB_Src+2);
		Convert_RGB_buffer++;
		*Convert_RGB_buffer =  *(Convert_RGB_Src+1);
		Convert_RGB_buffer++;
		*Convert_RGB_buffer =  *(Convert_RGB_Src);
		Convert_RGB_buffer++;
		if(i != row_width-1)
		{
			Convert_RGB_Src += 3;
		}
	}
	
	JSAMPLE *image_buffer = image.data;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride;
	//错误绑定
	cinfo.err = jpeg_std_error(&jerr);
	//创建解码器
	jpeg_create_compress(&cinfo);
	unsigned char *pCompressBuffer=NULL;
	//输入需要编码的内容
	//输出到内存中
	jpeg_mem_dest(&cinfo, &pCompressBuffer, (unsigned long *)dst_image_length);
	printf("dst_image_length is:%d\r\n", *dst_image_length);
	//jpeg_stdio_dest(&cinfo, outfile);/*输出到文件*/
	//设置图像的信息
	cinfo.image_width = image.cols;
	cinfo.image_height = image.rows;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	//设置编码的信息
	jpeg_set_defaults(&cinfo);
	//第二个参数为压缩系数，暂时设置为90
	jpeg_set_quality(&cinfo, 90, boolean(TRUE) /* limit to baseline-JPEG values */);
	//开始编码
	jpeg_start_compress(&cinfo, boolean(TRUE));
	row_stride = image.cols * 3;
	//编码信息的解析
	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	//结束编码
	jpeg_finish_compress(&cinfo);
	printf("test finish\r\n");
	//unsigned char Encode_face_crop[MAX_FACE_RECT];
    memcpy(*image_data, pCompressBuffer, *dst_image_length);
	//销毁编码开销
	jpeg_destroy_compress(&cinfo);
	apr_pool_destroy(apr_encode_image_pool);
	return 0;
}

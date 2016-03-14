#makefile
#syshen
#end

CC = g++
LIB_PATH = -L/home/endecode/lib/opencv \
						-lopencv_calib3d \
						-lopencv_contrib \
						-lopencv_core \
						-lopencv_features2d \
						-lopencv_flann \
						-lopencv_gpu \
						-lopencv_highgui \
						-lopencv_imgproc \
						-lopencv_legacy \
						-lopencv_ml \
						-lopencv_nonfree \
						-lopencv_objdetect \
						-lopencv_ocl \
						-lopencv_photo \
						-lopencv_stitching \
						-lopencv_superres \
						-lopencv_ts \
						-lopencv_video \
						-lopencv_videostab \
						-lrt \
						-lpthread \
						-lm \
						-ldl 
LIB_PATH += -L/home/endecode/lib/jpeg \
								-ljpeg

LIB_PATH += -L/home/endecode/lib/apr \
						-lapr-1
						
#-R添加.so.0的搜索路径
LIB_PATH += -Wl,-R/home/endecode/lib/opencv -Wl,-R/home/endecode/lib/apr  

INCLUDE_PATH = -I/home/endecode/include/jpeg \
				-I/home/endecode/include/opencv \
				-I/home/endecode/include/opencv2 \
				-I/home/endecode/include/apr

#LFLAGS += '-Wl,-rpath,$(LIB_PATH)'

SOURCE = src/interface.cpp
CJSON_SOURCE = src/cJSON.h src/cJSON.c

PROPERTY = -Wall

TARGET = test

all : $(TARGET)  target.o cJSON.o clean 

.PHONE : all

$(TARGET):target.o cJSON.o
	$(CC)  target.o cJSON.o $(LIB_PATH) -o $(TARGET) 
target.o:$(SOURCE)
	$(CC) -c $(SOURCE) $(INCLUDE_PATH) -o target.o
cJSON.o:$(CJSON_SOURCE)
	$(CC) -c $(CJSON_SOURCE) -o cJSON.o
clean:
	rm target.o cJSON.o


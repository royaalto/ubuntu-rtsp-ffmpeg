// tutorial02.c
// A pedagogical video player that will stream through every video frame as fast as it can.
//
// Code based on FFplay, Copyright (c) 2003 Fabrice Bellard,
// and a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1
// With updates from https://github.com/chelyaev/ffmpeg-tutorial
// Updates tested on:
// LAVC 54.59.100, LAVF 54.29.104, LSWS 2.1.101, SDL 1.2.15
// on GCC 4.7.2 in Debian February 2015
//
// Use
//
// gcc -o tutorial02 tutorial02.c -lavformat -lavcodec -lswscale -lz -lm `sdl-config --cflags --libs`
// to build (assuming libavformat and libavcodec are correctly installed,
// and assuming you have sdl-config. Please refer to SDL docs for your installation.)
//
// Run using
// tutorial02 myvideofile.mpg
//
// to play the video stream on your screen.

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

// #include "sdl/SDL.h"
}

// #include <SDL.h>
// #include <SDL_thread.h>

#include "SDL2/SDL.h"

#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif

#include <stdio.h>

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

#include "rtspClient.h"
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


using namespace std;
using namespace cv;
IplImage * MyResizeImage(IplImage* pSrc, double dScale)
{
	CvSize nSize;
	nSize.width = pSrc->width*dScale;
	nSize.height = pSrc->height*dScale;
	IplImage* pDes = cvCreateImage(nSize, pSrc->depth, pSrc->nChannels);
	cvResize(pSrc, pDes, CV_INTER_CUBIC);
	cvReleaseImage(&pSrc);
	return pDes;
}

int main(int argc, char *argv[])
{
	//unsigned char* data1 = new unsigned char[ 1920*1080*4];
	unsigned int    i;
	int             ret;
	int             video_st_index = -1;
	int             audio_st_index = -1;
	AVFormatContext *ifmt_ctx = NULL;
	AVPacket        pkt;
	AVStream        *st = NULL;
	char            errbuf[64];
	AVDictionary *optionsDict = NULL;
	const char * rtsp = "rtsp://service:1234hailee@192.168.0.22:554";
	av_register_all();                                                          // Register all codecs and formats so that they can be used.
	avformat_network_init();                                                    // Initialization of network components
	av_dict_set(&optionsDict, "rtsp_transport", "tcp", 0);                //采用tcp传输
	av_dict_set(&optionsDict, "stimeout", "2000000", 0);                  //如果没有设置stimeout，那么把ipc网线拔掉，av_read_frame会阻塞（时间单位是微妙）
	if ((ret = avformat_open_input(&ifmt_ctx, rtsp, 0, &optionsDict)) < 0)
	{            // Open the input file for reading.
		printf("Could not open input file '%s' (error '%s')\n", rtsp, av_make_error_string(errbuf, sizeof(errbuf), ret));
		//goto EXIT;
	}

	if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0)
	{                // Get information on the input file (number of streams etc.).
		printf("Could not open find stream info (error '%s')\n", av_make_error_string(errbuf, sizeof(errbuf), ret));
	//goto EXIT;
	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++)
	{                                // dump information
		av_dump_format(ifmt_ctx, i, rtsp, 0);
	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++)
	{                                // find video stream index
		st = ifmt_ctx->streams[i];
		switch (st->codec->codec_type)
		{
			case AVMEDIA_TYPE_AUDIO: audio_st_index = i; break;
			case AVMEDIA_TYPE_VIDEO: video_st_index = i; break;
			default: break;
		}
	}
	std::cout<<"video_st_index"<<video_st_index<<std::endl;
	if (-1 == video_st_index)
	{
		printf("No H.264 video stream in the input file\n");
		//goto EXIT;
	}
	av_init_packet(&pkt);                                                       // initialize packet.
	pkt.data = NULL;
	pkt.size = 0;
	bool nRestart = false;
	int videoindex = -1;
	int audioindex = -1;
	AVStream *pVst;
	uint8_t* buffer_rgb = NULL;
	AVCodecContext *pVideoCodecCtx = NULL;
	AVFrame         *pFrame = av_frame_alloc();
	AVFrame         *pFrameRGB = av_frame_alloc();
	int got_picture;
	SwsContext      *img_convert_ctx = NULL;
	AVCodec *pVideoCodec = NULL;
	while(1)
	{
		ret = av_read_frame(ifmt_ctx, &pkt);
		//cout << pkt.size << endl;
		if (!nRestart)
		{
			std:cout<<"enter"<<std::endl;
			for (int i = 0; i < ifmt_ctx->nb_streams; i++)
			{
				if ((ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) && (videoindex < 0))
				{
					videoindex = i;
				}
				if ((ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) && (audioindex < 0))
				{
					audioindex = i;
				}
			}
			pVst = ifmt_ctx->streams[videoindex];
			pVideoCodecCtx = pVst->codec;
			pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
			if (pVideoCodec == NULL)
			{
				fprintf(stderr, "Unsupported codec!\n");
				return -1; // Codec not found
			}
			//pVideoCodecCtx = avcodec_alloc_context3(pVideoCodec);

			if (avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL) < 0)
			{
				printf("avcodec_open2 error!!!\n");
				return -1; // Could not open codec
			}

			nRestart = true;
			printf("first frame \n");
		}


		if (pkt.stream_index == videoindex)
		{
			//fprintf(stdout, "pkt.size=%d,pkt.pts=%lld, pkt.data=0x%x.", pkt.size, pkt.pts, (unsigned int)pkt.data);
			fprintf(stdout, "pkt.size=%d,pkt.pts=%lld\n", pkt.size, pkt.pts);
			int av_result = avcodec_decode_video2(pVideoCodecCtx, pFrame, &got_picture, &pkt);

			if (got_picture)
			{
				fprintf(stdout, "decode one video frame!\n");
			}

			if (av_result < 0)
			{
				fprintf(stderr, "decode failed: inputbuf = 0x%x , input_framesize = %d\n", pkt.data, pkt.size);
			}
			printf("get pic %d--->%d--->%d\n", got_picture, pVideoCodecCtx->width, pVideoCodecCtx->height);

			if (got_picture)
			{
				int bytes = avpicture_get_size(AV_PIX_FMT_RGB24, pVideoCodecCtx->width, pVideoCodecCtx->height);
				buffer_rgb = (uint8_t *)av_malloc(bytes);
				avpicture_fill((AVPicture *)pFrameRGB, buffer_rgb, AV_PIX_FMT_RGB24, pVideoCodecCtx->width, pVideoCodecCtx->height);

				img_convert_ctx = sws_getContext(pVideoCodecCtx->width, pVideoCodecCtx->height, pVideoCodecCtx->pix_fmt,
				pVideoCodecCtx->width, pVideoCodecCtx->height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
				if (img_convert_ctx == NULL)
				{

					printf("can't init convert context!\n");

				}
				sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pVideoCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
				printf("iplimage-=-====-=-\n");
			}
			//IplImage *pRgbImg = cvCreateImage(cvSize(pVideoCodecCtx->width, pVideoCodecCtx->height), 8, 3);

			//memcpy(data1, buffer_rgb, pVideoCodecCtx->width * 3 * pVideoCodecCtx->height);
			av_free(buffer_rgb);
			av_free_packet(&pkt);
			sws_freeContext(img_convert_ctx);

			//delete [] data1;

		}
	}
}

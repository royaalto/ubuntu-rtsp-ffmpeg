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


#include <iostream>
#include <string>
using namespace std;

int ffmpeg_encode_jpeg(AVFrame *frame, std::string &filename);

const int bpp=12;
int screen_w=500,screen_h=500;
const int pixel_w=640,pixel_h=360;
unsigned char buffer[pixel_w*pixel_h*bpp/8];






int main(int argc, char *argv[]) 
{
    AVFormatContext *pFormatCtx = NULL;
    int             i, videoStream;
    AVCodecContext  *pCodecCtxOrig = NULL;
    AVCodecContext  *pCodecCtx = NULL;
    AVCodec         *pCodec = NULL;
    AVFrame         *pFrame = NULL;
    AVPacket        packet;
    int             frameFinished;
    float           aspect_ratio;
    struct SwsContext *sws_ctx = NULL;

    AVInputFormat *piFmt = NULL;


    if(argc != 2) {
        cout << "Usage: " << argv[0] << " <URL>" << endl;
        cout << "For example: " << endl;
        cout << argv[0] << " rtsp://127.0.0.1/ansersion" << endl;
        return 1;
    }


    AVFrame * pFrame_YUV;
    int count =0;
    SDL_Window * pWindow1;
    SDL_Renderer * pRenderer1;
    SDL_Texture * bmpText1;
    uint8_t * pixels1;
    int pitch1, size1;

    SDL_Window * pWindow2;
    SDL_Renderer * pRenderer2;
    SDL_Texture * bmpText2;
    uint8_t * pixels2;
    int pitch2, size2;
    uint8_t * buffer_YUV;

    SDL_Event event;


    // Register all formats and codecs
    av_register_all();
    avformat_network_init();
    if(SDL_Init(SDL_INIT_VIDEO)) 
    {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    // fp_open = fopen("test_packet_recv.h264", "rb+");
    pFormatCtx = NULL;



    const char * rtsp = "rtsp://service:1234hailee@192.168.100.203:554";
    AVDictionary *optionsDict = NULL;
    av_dict_set(&optionsDict, "rtsp_transport", "tcp", 0);                //采用tcp传输
    av_dict_set(&optionsDict, "stimeout", "2000000", 0);


    int err = avformat_open_input(&pFormatCtx, rtsp, NULL, &optionsDict);
    if(err) 
    {
        printf("avformat_open_input error: %d\n", err);
        return -1;
    }

    cout << "before avformat_find_stream_info" << endl;
    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0) 
    {
        printf("avformat_find_stream_info error!!!\n");
        return -1; // Couldn't find stream information
    }

    // cout << "before av_dump_format" << endl;
    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, "", 0);

    // Find the first video stream
    videoStream=-1;
    // cout << "before for(i = 0; i < pFormatCtx->nb_streams; i++)" << endl;
    for(i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) 
        {
            videoStream=i;
            break;
        }
    }

    if(videoStream==-1) 
    {
        printf("videoStream error!!!\n");
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtxOrig=pFormatCtx->streams[videoStream]->codec;
    // Find the decoder for the video stream
    // cout << "before avcodec_find_decoder" << endl;
    pCodec=avcodec_find_decoder(pCodecCtxOrig->codec_id);
    if(pCodec==NULL) 
    {
        fprintf(stderr, "Unsupported codec!\n");
        return -1; // Codec not found
    }

  // Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    // cout << "before avcodec_copy_context" << endl;
    if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) 
    {
        fprintf(stderr, "Couldn't copy codec context");
        return -1; // Error copying codec context
    }

    // Open codec
    cout << "before avcodec_open2" << endl;
    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0) 
    {
        printf("avcodec_open2 error!!!\n");
        return -1; // Could not open codec
    }

    // Allocate video frame
    pFrame=av_frame_alloc();
    // pFrame_YUV = avcodec_alloc_frame();
    pFrame_YUV = av_frame_alloc();
    if(pFrame_YUV == NULL) 
    {
        printf("avcodec_alloc_frame error!!!\n");
        return -1; // Could not open codec
    }
    buffer_YUV = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height));
    avpicture_fill((AVPicture *)pFrame_YUV, buffer_YUV, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

    printf("Everything OK\n");

    // Make a screen to put our video
    // #ifndef __DARWIN__
    //         screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, 0);
    // #else
    //         screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 24, 0);
    // #endif
    // if(!screen) {
    //   fprintf(stderr, "SDL: could not set video mode - exiting\n");
    //   exit(1);
    // }

    // // Allocate a place to put our YUV image on that screen
    // bmp = SDL_CreateYUVOverlay(pCodecCtx->width,
    //   			 pCodecCtx->height,
    //   			 SDL_YV12_OVERLAY,
    //   			 screen);

    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(pCodecCtx->width,
                pCodecCtx->height,
                pCodecCtx->pix_fmt,
                pCodecCtx->width,
                pCodecCtx->height,
                AV_PIX_FMT_YUV420P,
                SWS_BILINEAR,
                NULL,
                NULL,
                NULL
                );



    // Read frames and save first five frames to disk
    i=0;
    // 3.1.1 prepare SDL for YUV
    // allocate window, renderer, texture
    pWindow1    = SDL_CreateWindow( "YUV", 0, 0, pCodecCtx->width, pCodecCtx->height, SDL_WINDOW_SHOWN);
    pRenderer1  = SDL_CreateRenderer(pWindow1, -1, SDL_RENDERER_ACCELERATED);
    bmpText1     = SDL_CreateTexture(pRenderer1, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
    size1       = pCodecCtx->width * pCodecCtx->height;
    if(pWindow1==NULL | pRenderer1==NULL | bmpText1==NULL) 
    {
        fprintf(stderr, "Could not open window1\n%s\n", SDL_GetError());
        return -1;
    }


    while(av_read_frame(pFormatCtx, &packet)>=0) 
    {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) 
        {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Did we get a video frame?
            if(frameFinished) 
            {
                // SDL_LockYUVOverlay(bmp);

                // AVPicture pict;
                // pict.data[0] = bmp->pixels[0];
                // pict.data[1] = bmp->pixels[2];
                // pict.data[2] = bmp->pixels[1];

                // pict.linesize[0] = bmp->pitches[0];
                // pict.linesize[1] = bmp->pitches[2];
                // pict.linesize[2] = bmp->pitches[1];

                // Convert the image into YUV format that SDL uses
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                    pFrame->linesize, 0, pCodecCtx->height,
                    pFrame_YUV->data, pFrame_YUV->linesize);

                std::cout<<"pFrame_YUV"<<pFrame_YUV->width<<std::endl;
                std::cout<<"pFrame"<<pFrame->width<<std::endl;
                std::cout<<"pFrame_YUV"<<pFrame_YUV->height<<std::endl;
                std::cout<<"size"<<sizeof(*pFrame_YUV->data[0])<<std::endl;

                // 3.1.2. copy converted YUV to SDL 2.0 texture
                SDL_LockTexture(bmpText1, NULL, (void **)&pixels1, &pitch1);
                memcpy(pixels1,             pFrame_YUV->data[0], size1  );
                memcpy(pixels1 + size1,     pFrame_YUV->data[2], size1/4);
                memcpy(pixels1 + size1*5/4, pFrame_YUV->data[1], size1/4);
                SDL_UnlockTexture(bmpText1);
                SDL_UpdateTexture(bmpText1, NULL, pixels1, pitch1);
                // refresh screen
                SDL_RenderClear(pRenderer1);
                SDL_RenderCopy(pRenderer1, bmpText1, NULL, NULL);
                SDL_RenderPresent(pRenderer1);

                // // 3.2.2. copy converted BGR to SDL 2.0 texture
                // SDL_LockTexture(bmpTex2, NULL, (void **)&pixels2, &pitch2);
                // memcpy(pixels2,             pFrame_BGR24->data[0], size2);
                // SDL_UnlockTexture(bmpTex2);
                // SDL_UpdateTexture(bmpTex2, NULL, pixels2, pitch2);
                // // refresh screen
                // SDL_RenderClear(pRenderer2);
                // SDL_RenderCopy(pRenderer2, bmpTex2, NULL, NULL);
                // SDL_RenderPresent(pRenderer2);


                // SDL_UnlockYUVOverlay(bmp);

                // rect.x = 0;
                // rect.y = 0;
                // rect.w = pCodecCtx->width;
                // rect.h = pCodecCtx->height;
                // SDL_DisplayYUVOverlay(bmp, &rect);
                std::string filename = to_string(count);
                ffmpeg_encode_jpeg(pFrame,filename);
                count++;
            }
        }

        // Free the packet that was allocated by av_read_frame

        av_free_packet(&packet);
        SDL_PollEvent(&event);
        switch(event.type) 
        {
            case SDL_QUIT:
            SDL_Quit();
            exit(0);
            break;
            default:
            break;
        }

    }//while(frame)

    SDL_DestroyTexture(bmpText1);
    // SDL_DestroyTexture(bmpTex2);

    SDL_DestroyRenderer(pRenderer1);
    // SDL_DestroyRenderer(pRenderer2);

    SDL_DestroyWindow(pWindow1);
    // SDL_DestroyWindow(pWindow2);

    av_free(pFrame_YUV);
    av_free(buffer_YUV);

    // Free the YUV frame
    // av_frame_free(&pFrame);
    // av_frame_free(&pFrame_YUV);

    // Close the codec
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    //Client.DoTEARDOWN();

    SDL_Quit();
    return 0;
}



int ffmpeg_encode_jpeg(AVFrame *frame, std::string &filename)
 {
	AVCodec *codec = NULL;
	AVCodecContext *codec_ctx = NULL;
	codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
	//硬编码
	//codec = avcodec_find_encoder_by_name("h264_nvenc");
	//codec = avcodec_find_encoder_by_name("hevc_nvenc");
	if (NULL == codec) {
		return -1;
	}
	codec_ctx = avcodec_alloc_context3(codec);
	if (NULL == codec_ctx) {
		return -2;
	}

	AVRational rate;
	rate.num = 1;
	rate.den = 25;
	codec_ctx->time_base = rate;
	//codec_ctx->thread_count = 1;
	codec_ctx->pix_fmt = AV_PIX_FMT_YUVJ420P;
	//codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	//codec_ctx->pix_fmt = AV_PIX_FMT_NV12;
	codec_ctx->height = 1080;
	codec_ctx->width = 1920;

	int err = avcodec_open2(codec_ctx, codec, NULL);
	if (err < 0) {
		av_log(NULL, AV_LOG_INFO, "avcodec_open2 failed:%d", err);
		return -3;
	}

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;
    int gotFrame;
    avcodec_encode_video2(codec_ctx, &pkt, frame, &gotFrame);
	// avcodec_send_frame(codec_ctx, frame);
	// avcodec_receive_packet(codec_ctx, &pkt);
	printf("pkt.size: %d\n", pkt.size);

	FILE *fp = fopen(filename.c_str(), "wb");
	if (fp) {
		fwrite(pkt.data, 1, pkt.size, fp);
		fclose(fp);
	}

	av_packet_unref(&pkt);
	avcodec_close(codec_ctx);
	av_freep(&codec_ctx);

	return 0;
}

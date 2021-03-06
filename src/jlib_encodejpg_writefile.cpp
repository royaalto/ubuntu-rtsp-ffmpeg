
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavdevice/avdevice.h>
}

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
#include <mutex>
using namespace std;
 int ffmpeg_encode_jpeg(AVFrame *frame,std::string &filename);
int rtspClientRequest(RtspClient * Client, string url);
int fill_iobuffer(void * opaque, uint8_t * buf, int bufsize);
/*
    fill the data to ffmpeg
*/
int fill_iobuffer(void * opaque, uint8_t * buf, int bufsize)
{
    size_t size = 0;
    if(!opaque) return -1;
    RtspClient * Client = (RtspClient *)opaque;
    while(true)
    {
        if(Client->GetMediaData("video", buf, &size, bufsize)) break;
    }
    //Client->GetMediaData("video", buf, &size, bufsize);
    printf("fill_iobuffer size: %u\n", size);
    return size;
}

int main(int argc, char *argv[])
{
    AVFormatContext *pFormatCtx = NULL;
    int             i, videoStream;
    AVCodecContext  *pCodecCtxOrig = NULL;
    AVCodecContext  *pCodecCtx = NULL;
    AVCodec         *pCodec = NULL;
    AVFrame         *pFrame = NULL;
    AVFrame         *pict=NULL;
    AVPacket        packet;
    int             frameFinished;
    float           aspect_ratio;
    struct SwsContext *sws_ctx = NULL;
    std::mutex m_playMutex;

    AVInputFormat *piFmt = NULL;
    RtspClient Client;
    int count=0;

    if(argc != 2)
    {
        cout << "Usage: " << argv[0] << " <URL>" << endl;
        cout << "For example: " << endl;
        cout << argv[0] << " rtsp://127.0.0.1/ansersion" << endl;
        return 1;
    }
    rtspClientRequest(&Client, argv[1]);


      // Register all formats and codecs
    av_register_all();

    pFormatCtx = NULL;
    pFormatCtx = avformat_alloc_context();
    unsigned char * iobuffer = (unsigned char *)av_malloc(98304);
    AVIOContext * avio = avio_alloc_context(iobuffer, 98304, 0, &Client, fill_iobuffer, NULL, NULL);
    pFormatCtx->pb = avio;

    if(!avio)
    {
        printf("avio_alloc_context error!!!\n");
        return -1;
    }

    if(av_probe_input_buffer(avio, &piFmt, "", NULL, 0, 0) < 0)
    {
        printf("av_probe_input_buffer error!\n");
        return -1;
    }
    else
    {
        printf("probe success\n");
        printf("format: %s[%s]\n", piFmt->name, piFmt->long_name);
    }

    cout << "before avformat_open_input" << endl;
    int err = avformat_open_input(&pFormatCtx, "nothing", NULL, NULL);
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
        if(videoStream==-1)
        {
            printf("videoStream error!!!\n");
            return -1; // Didn't find a video stream
        }
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

    /*=+++++++++*/
    pCodecCtx->time_base.num = 1 ;
    pCodecCtx->time_base.den = 30;
    pCodecCtx->gop_size = 10; /* emit one intra frame every ten frames */
    pCodecCtx->max_b_frames = 0;
    pCodecCtx->thread_count = 1;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    /*_--------------*/

    // Open codec
    cout << "before avcodec_open2" << endl;
    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
    {
        printf("avcodec_open2 error!!!\n");
        return -1; // Could not open codec
    }

      // Allocate video frame
    pFrame=av_frame_alloc();

    printf("Everything OK\n");



    printf("create swscontext\n");
      // Read frames and save first five frames to disk
    av_init_packet(&packet);
    uint8_t* out_buffer;
    pict=av_frame_alloc();

    while(av_read_frame(pFormatCtx, &packet)>=0)
    {
        sws_ctx = sws_getContext(
            pCodecCtx->width,
            pCodecCtx->height,
            pCodecCtx->pix_fmt,
            pCodecCtx->width,
            pCodecCtx->height,
            AV_PIX_FMT_YUV420P,
            SWS_BICUBIC,
            NULL,
            NULL,
            NULL
        );
        printf("read frame\n");
        // Is this a packet from the video stream?
        int size = avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
        out_buffer = (uint8_t* )av_malloc(size);

        printf("outbuffer malloc\n");

        std::cout<<"，pCodecCtx width"<<pCodecCtx->width<<std::endl;
        avpicture_fill((AVPicture*)pict, out_buffer, AV_PIX_FMT_YUVJ420P, pCodecCtx->width, pCodecCtx->height);
        if(packet.stream_index==videoStream)
        {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
            printf("pkt.size: %d\n", packet.size);
            std::cout<<"data:"<<packet.data<<std::endl;
            // Did we get a video frame?
            if(frameFinished)
            {
                printf("read frame finnish\n");
                //add mutex
                //m_playMutex.lock();
                std::cout<<"frame width"<<pFrame->width<<std::endl;

                std::cout<<"====pCodecCtx->width size======"<<pCodecCtx->width<<std::endl;
                std::cout<<"====pCodecCtx->height size======"<<pCodecCtx->height<<std::endl;
                std::cout<<"====size======"<<size<<std::endl;

                std::cout<<"pict width"<<pict->width<<std::endl;
                printf("outbuffer picture combine\n");
                // Convert the image into YUV format that SDL uses

                sws_scale(
                    sws_ctx,
                    (const uint8_t* const *)pFrame->data,
                    pFrame->linesize,
                    0,
                    pCodecCtx->height,
                    pict->data,
                    pict->linesize);

                // std::cout<<"pict frame width"<<pict->width<<std::endl;
                // FILE *fp_yuv=fopen("./output.yuv","wb+");
                // // int y_size=pCodecCtx->width*pCodecCtx->height;
				// // fwrite(pict->data[0],1,y_size,fp_yuv);    //Y
				// // fwrite(pict->data[1],1,y_size/4,fp_yuv);  //U
				// // fwrite(pict->data[2],1,y_size/4,fp_yuv);  //V
                std::string filename = to_string(count);
                //ffmpeg_encode_jpeg(pict,filename);
                //m_playMutex.unlock();
                //av_free(out_buffer);
                count++;
            }
        }
        //delete []out_buffer;
    // Free the packet that was allocated by av_read_frame

        av_free_packet(&packet);
        //av_free(out_buffer);
    // av_frame_free(&pict);//can not delet pict dont know why
    // delete []out_buffer;

    }

    // Free the YUV frame
    av_frame_free(&pFrame);
    //av_frame_free(&pict);
    // Close the codec
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    Client.DoTEARDOWN();
    return 0;
}

//int ffmpeg_encode_jpeg(AVFrame *frame, std::string &filename)
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

int rtspClientRequest(RtspClient * Client, string url)
{
    if(!Client) return -1;

    // cout << "Start play " << url << endl;
    string RtspUri(url);
    // string RtspUri("rtsp://192.168.81.145/ansersion");

    /* Set up rtsp server resource URI */
    Client->SetURI(RtspUri);
    Client->SetUsername("service");
    Client->SetPassword("1234hailee");
    /* Send DESCRIBE command to server */
    if(Client->DoOPTIONS() != RTSP_NO_ERROR)
    {
        printf("DoOPTIONS error\n");
        return 0;
    }
    printf("%s\n", Client->GetResponse().c_str());
    /* Check whether server return '200'(OK) */
    if(!Client->IsResponse_200_OK())
    {
        printf("DoOPTIONS error\n");
        return 0;
    }

    /* Send DESCRIBE command to server */
    if(Client->DoDESCRIBE() != RTSP_NO_ERROR)
    {
        printf("DoDESCRIBE error\n");
        return 0;
    }
    printf("%s\n", Client->GetResponse().c_str());
/* Check whether server return '200'(OK) */
    if(!Client->IsResponse_200_OK())
    {
        printf("DoDESCRIBE error\n");
        return 0;
    }

/* Parse SDP message after sending DESCRIBE command */
    printf("%s\n", Client->GetSDP().c_str());
    if(Client->ParseSDP() != RTSP_NO_ERROR)
    {
        printf("ParseSDP error\n");
        return 0;
    }

    /* Send SETUP command to set up all 'audio' and 'video'
    * sessions which SDP refers. */
    if(Client->DoSETUP() != RTSP_NO_ERROR)
    {
        printf("DoSETUP error\n");
        return 0;
    }
    //Client->SetVideoByeFromServerClbk(ByeFromServerClbk);
    printf("%s\n", Client->GetResponse().c_str());

    /* Check whether server return '200'(OK) */
    if(!Client->IsResponse_200_OK())
    {
        printf("DoSETUP error\n");
        return 0;
    }

    printf("start PLAY\n");
    /* Send PLAY command to play only 'video' sessions.
    * note(FIXME):
    * if there are several 'video' session
    * refered in SDP, only play the first 'video'
    * session, the same as 'audio'.*/
    if(Client->DoPLAY("video", NULL, NULL, NULL) != RTSP_NO_ERROR)
    {
        printf("DoPLAY error\n");
        return 0;
    }
}

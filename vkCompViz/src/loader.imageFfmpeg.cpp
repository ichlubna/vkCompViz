//TODO use import std only
module;
#include <string>
#include "ffmpeg.h"
module loader;
import std;
using namespace Loader;

AVFrame *frameFromFile(std::string path)
{
    AVFormatContext *formatContext;
    formatContext = avformat_alloc_context();
    if(avformat_open_input(&formatContext, path.c_str(), nullptr, nullptr) != 0)
        throw std::runtime_error("Cannot open file: " + path);
    avformat_find_stream_info(formatContext, nullptr);
    AVCodec *codec;
    auto streamID = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, const_cast<const AVCodec **>(&codec), 0);
    if(streamID < 0)
        throw std::runtime_error("No video or image stream available");
    if(!codec)
        throw std::runtime_error("No suitable codec found");
    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, formatContext->streams[streamID]->codecpar);
    if(avcodec_open2(codecContext, codec, nullptr) < 0)
        throw std::runtime_error("Cannot open codec.");

    AVFrame *decodedFrame = av_frame_alloc();
    AVPacket *packet = av_packet_alloc();
    av_read_frame(formatContext, packet);
    avcodec_send_packet(codecContext, packet);
    avcodec_send_packet(codecContext, nullptr);
    int err = avcodec_receive_frame(codecContext, decodedFrame);
    if(err < 0)
		throw std::runtime_error("Cannot receive frame");
	av_packet_free(&packet);
    return decodedFrame;
}

void convertFrame(AVFrame *input, AVFrame *output)
{
    struct SwsContext *cRGB = nullptr;  
    cRGB = sws_getCachedContext(cRGB, input->width, input->height, (enum AVPixelFormat)input->format, output->width, output->height, static_cast<AVPixelFormat>(output->format), SWS_BICUBIC, nullptr, nullptr, nullptr);
    sws_scale(cRGB, (const uint8_t *const *)input->data, input->linesize, 0, input->height, output->data, output->linesize);
}

ImageFfmpeg::ImageFfmpeg(std::string inputPath) : Image(inputPath)
{
    path = inputPath;
    auto decodedFrame = frameFromFile(inputPath);
    frame = av_frame_alloc();
    frame->width = decodedFrame->width;
    frame->height = decodedFrame->height;
    av_frame_get_buffer(frame, 0);
 
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(static_cast<enum AVPixelFormat>(decodedFrame->format));
    int bitDepth = desc->comp[0].depth;

    if (bitDepth == 8)
    {
        frame->format = AV_PIX_FMT_RGBA;
        format = Image::Format::RGBA_8_INT; 
    }
    else
    {
        frame->format = AV_PIX_FMT_RGBAF32;
        format = Image::Format::RGBA_32_FLOAT;
    }

    convertFrame(decodedFrame, frame);
      
    av_frame_unref(decodedFrame);
    av_frame_free(&decodedFrame);
}
       
ImageFfmpeg::ImageFfmpeg(size_t width, size_t height, size_t stride, Format imageFormat, std::string outputPath, uint8_t *data) : Image(width, height, imageFormat), format{imageFormat}
{    
    frame = av_frame_alloc(); 
    frame->width = width;
    frame->height = height;
    format = imageFormat;
    path = outputPath;
    if(data)
    {
        av_frame_get_buffer(frame, 0);
        throw std::runtime_error("Not implemented yet");
    }
}

void ImageFfmpeg::save(std::string outputPath) const
{
    std::string outputFilePath = outputPath;
    if(outputPath == "")
        outputFilePath = outputPath;
}

ImageFfmpeg::~ImageFfmpeg()
{
    av_frame_unref(frame);
    av_frame_free(&frame);
}


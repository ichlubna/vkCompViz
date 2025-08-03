//TODO use import std only
module;
#include <string>
#include "ffmpeg.h"
module loader;
import common;
import std;
using namespace Loader;

size_t ImageFfmpeg::channels() const
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(static_cast<enum AVPixelFormat>(frame->format));
    return desc->nb_components;
}

size_t ImageFfmpeg::channelSize() const
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(static_cast<enum AVPixelFormat>(frame->format));
    return desc->comp[0].depth / 8;
}

AVFrame *frameFromFile(std::string path)
{
    if(!DEBUG)
        av_log_set_level(AV_LOG_ERROR);

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
        throw std::runtime_error("Cannot open codec");

    AVFrame *decodedFrame = av_frame_alloc();
    AVPacket *packet = av_packet_alloc();
    av_read_frame(formatContext, packet);
    avcodec_send_packet(codecContext, packet);
    avcodec_send_packet(codecContext, nullptr);
    if(avcodec_receive_frame(codecContext, decodedFrame) < 0)
        throw std::runtime_error("Cannot receive frame");
    av_packet_free(&packet);
    return decodedFrame;
}

void convertFrame(AVFrame *input, AVFrame *output)
{
    struct SwsContext *cRGB = nullptr;

    if(input->format == AV_PIX_FMT_YUVJ420P)
        input->format = AV_PIX_FMT_YUV420P;
    else if(input->format == AV_PIX_FMT_YUVJ422P)
        input->format = AV_PIX_FMT_YUV422P;
    else if(input->format == AV_PIX_FMT_YUVJ444P)
        input->format = AV_PIX_FMT_YUV444P;

    cRGB = sws_getCachedContext(cRGB, input->width, input->height, static_cast<enum AVPixelFormat>(input->format), output->width, output->height, static_cast<AVPixelFormat>(output->format), SWS_BICUBIC, nullptr, nullptr, nullptr);
    sws_scale(cRGB, (const uint8_t *const *)input->data, input->linesize, 0, input->height, output->data, output->linesize);
}

ImageFfmpeg::ImageFfmpeg(std::string inputPath) : Image()
{
    path = inputPath;
    auto decodedFrame = frameFromFile(inputPath);
    frame = av_frame_alloc();
    frame->width = decodedFrame->width;
    frame->height = decodedFrame->height;

    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(static_cast<enum AVPixelFormat>(decodedFrame->format));
    int bitDepth = desc->comp[0].depth;

    if(bitDepth == 8)
    {
        frame->format = AV_PIX_FMT_RGBA;
        format = Image::Format::RGBA_8_INT;
    }
    else
    {
        frame->format = AV_PIX_FMT_RGBAF32;
        format = Image::Format::RGBA_32_FLOAT;
    }

    av_frame_get_buffer(frame, 0);
    convertFrame(decodedFrame, frame);

    av_frame_unref(decodedFrame);
    av_frame_free(&decodedFrame);
}

ImageFfmpeg::ImageFfmpeg(size_t width, size_t height, [[maybe_unused]] size_t stride, Format imageFormat, uint8_t *data) : Image()
{
    frame = av_frame_alloc();
    frame->width = width;
    frame->height = height;
    format = imageFormat;
    size_t pixelSize = 0;
    if(format == Image::Format::RGBA_8_INT)
    {
        frame->format = AV_PIX_FMT_RGBA;
        pixelSize = 4;
    }
    else
    {
        frame->format = AV_PIX_FMT_RGBAF32;
        pixelSize = 4 * sizeof(float);
    }
    if(data)
    {
        av_frame_get_buffer(frame, 0);
        memcpy(frame->data[0], data, width * height * pixelSize);
    }
}

void ImageFfmpeg::save(std::string outputPath) const
{
    if(!DEBUG)
        av_log_set_level(AV_LOG_ERROR);

    std::string outputFilePath = outputPath;
    if(outputPath == "")
        outputFilePath = outputPath;
    AVFormatContext *formatContext = avformat_alloc_context();
    if(avformat_alloc_output_context2(&formatContext, nullptr, nullptr, outputFilePath.c_str()) < 0)
        throw std::runtime_error("Could not open output file");
    const AVOutputFormat *outputFormat = av_guess_format(nullptr, outputFilePath.c_str(), nullptr);
    if(!outputFormat)
        throw std::runtime_error("Could not deduce output format");
    const AVCodec *codec = avcodec_find_encoder(outputFormat->video_codec);
    if(!codec)
        throw std::runtime_error("Could not find encoder");
    AVStream *stream;
    if(!(stream = avformat_new_stream(formatContext, codec)))
        throw std::runtime_error("Could not create a new output stream");
    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    if(!codecContext)
        throw std::runtime_error("Could not allocate the encoder context");

    AVFrame *outputFrame = av_frame_alloc();
    outputFrame->width = frame->width;
    outputFrame->height = frame->height;
    outputFrame->format = AV_PIX_FMT_NONE;
    const enum AVPixelFormat *pixFmts;
    int fmtCount = 0;
    avcodec_get_supported_config(codecContext, codec, AV_CODEC_CONFIG_PIX_FORMAT, 0, reinterpret_cast<const void **>(&pixFmts), &fmtCount);
    for(int fmtID = 0; fmtID < fmtCount; fmtID++)
        if(format == Image::Format::RGBA_8_INT)
        {
            if(pixFmts[fmtID] == AV_PIX_FMT_RGBA || pixFmts[fmtID] == AV_PIX_FMT_YUVJ444P)
            {
                outputFrame->format = pixFmts[fmtID];
                break;
            }
        }
        else if(format == Image::Format::RGBA_32_FLOAT)
        {
            if(pixFmts[fmtID] == AV_PIX_FMT_RGBAF32)
            {
                outputFrame->format = pixFmts[fmtID];
                break;
            }
        }
        else
            throw std::runtime_error("Unsupported image format");
    if(outputFrame->format == AV_PIX_FMT_NONE)
        outputFrame->format = pixFmts[0];
    av_frame_get_buffer(outputFrame, 0);
    convertFrame(frame, outputFrame);

    const int fps = 1;
    stream->time_base = {1, fps};
    codecContext->time_base = stream->time_base;
    codecContext->pix_fmt = static_cast<enum AVPixelFormat>(outputFrame->format);
    codecContext->width = outputFrame->width;
    codecContext->height = outputFrame->height;
    if(formatContext->oformat->flags & AVFMT_GLOBALHEADER)
        codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    if(avcodec_open2(codecContext, codec, nullptr) < 0)
        throw std::runtime_error("Could not open the encoder");
    if(avcodec_parameters_from_context(stream->codecpar, codecContext) < 0)
        throw std::runtime_error("Could not copy the encoder parameters to the stream");
    if(avio_open(&formatContext->pb, outputFilePath.c_str(), AVIO_FLAG_WRITE) < 0)
        throw std::runtime_error("Could not open the output format for writing");
    if(avformat_write_header(formatContext, nullptr) < 0)
        throw std::runtime_error("Could not write the output file header");
    if(avcodec_send_frame(codecContext, outputFrame) < 0)
        throw std::runtime_error("Could not send a frame in the encoder");
    avcodec_send_frame(codecContext, nullptr);
    AVPacket *outputPacket = av_packet_alloc();
    if(avcodec_receive_packet(codecContext, outputPacket) < 0)
        throw std::runtime_error("Could not receive a packet from the encoder");
    outputPacket->stream_index = stream->index;
    if(av_write_frame(formatContext, outputPacket) < 0)
        throw std::runtime_error("Could not write a packet in the file.");
    av_write_trailer(formatContext);
    av_packet_unref(outputPacket);
    av_packet_free(&outputPacket);
}

ImageFfmpeg::~ImageFfmpeg()
{
    av_frame_unref(frame);
    av_frame_free(&frame);
}


//TODO use import std only
module;
#include <string>
#include "ffmpeg.h"

module loader;
using namespace Loader;

ImageData::ImageData()
{
}

ImageData::~ImageData()
{
    //free?
}

const unsigned char* ImageData::getData() const
{
    return data;
}

ImageFfmpeg::ImageFfmpeg(std::string path) : Image(path)
{
    auto formatContext = avformat_alloc_context();
}

ImageFfmpeg::~ImageFfmpeg()
{
}

const unsigned char* ImageFfmpeg::getData() const
{
    return data->getData(); 
}

//TODO use import std only
module;
#include <string>
#include "ffmpeg.h"

module loader;
using namespace Loader;

const unsigned char *ImageFfmpeg::getData() const
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


//TODO use import std only
module;
#include <string>

module loader;
using namespace Loader;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
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
}

//TODO use import std only
module;
#include <string>

module loader;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

Image::Image(std::string path)
{
    auto formatContext = avformat_alloc_context();
}

Image::~Image()
{
}

const unsigned char* Image::getData() const
{
}

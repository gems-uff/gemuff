#include "Helper.h"

QImage toQImage(GEMUFF::VIMUFF::ImagePtr image){
    QImage result;

    result = QImage((const uchar*)  image->getData(),
        image->getWidth(), image->getHeight(), image->getStride(), QImage::Format_RGB32);

    return result;
}

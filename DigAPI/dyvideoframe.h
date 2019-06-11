#ifndef DYVIDEOFRAME_H
#define DYVIDEOFRAME_H

#include <qwindowdefs.h>

class DYVideoFrame
{
public:
    DYVideoFrame() {}
    virtual ~DYVideoFrame(){}

    virtual WId request() = 0;

    /*!
        \brief Release current video frame.
    */
    virtual void release() = 0;
};

#endif // DYVIDEOFRAME_H

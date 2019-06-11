#include "dyenums.h"

QList<float> DYVlc::scale()
{
    QList<float> list;
    list << (float)0
         << (float)1.05
         << (float)1.1
         << (float)1.2
         << (float)1.3
         << (float)1.4
         << (float)1.5
         << (float)1.6
         << (float)1.7
         << (float)1.8
         << (float)1.9
         << (float)2.0;

    return list;
}

QStringList DYVlc::ratio()
{
    QStringList list;
    list << "default"
         << "16:9"
         << "16:10"
         << "185:100"
         << "221:100"
         << "235:100"
         << "239:100"
         << "4:3"
         << "5:4"
         << "5:3"
         << "1:1";

    return list;
}

QStringList DYVlc::transform()
{
    QStringList list;
    list << "TRANSFORM_IDENTITY"
         << "TRANSFORM_HFLIP"
         << "TRANSFORM_VFLIP"
         << "TRANSFORM_R90"
         << "TRANSFORM_R180"
         << "TRANSFORM_R270"
         << "TRANSFORM_TRANSPOSE"
         << "TRANSFORM_ANTI_TRANSPOSE";
    return list;
}

QStringList DYVlc::deinterlacing()
{
    QStringList list;
    list << "Disabled"
         << "Discard"
         << "Blend"
         << "Mean"
         << "Bob"
         << "Linear"
         << "X"
         << "Yadif"
         << "Yadif2x"
         << "Phospor"
         << "IVTC";
    return list;
}

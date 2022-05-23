#include <stdio.h>

extern "C"  
{  
    #include "include/libavcodec/avcodec.h"
    // #include "include/libavformat/avformat.h"
    // #include "include/libavfilter/avfilter.h" 
};  
int main(int argc, char const *argv[])
{
    printf("%s\n", avcodec_configuration());
    return 0;
}





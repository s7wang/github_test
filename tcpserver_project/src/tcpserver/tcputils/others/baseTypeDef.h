/******************************************************************************
   FileName:      baseTypeDef.h
   Author:        panxiaoming
   Version :      1.0
   Date:          2014.08.29
   Description:

   History:
       <author>         <time>     <version >      <desc>
     panxiaoming      2014.08.29      1.0          create
******************************************************************************/

#ifndef _BASE_TYPE_DEF_H_
#define _BASE_TYPE_DEF_H_

#define ENV_SYBER   1
#define ENV_QINLIN  2
#define OS_ENV    ENV_SYBER

#define LITTLE_ENDIAN_USER  1
#define BIG_ENDIAN_USER     2
#define ENDIAN_TYPE    LITTLE_ENDIAN_USER



typedef signed char        Int8;
typedef unsigned char      Uint8;
typedef short              Int16;
typedef unsigned short     Uint16;
typedef int                Int32;
typedef unsigned int       Uint32;
typedef long long          Int64;
typedef unsigned long long Uint64;

typedef float              Float32;
typedef long double        Float64;

#define APP_RET_OK	            0
#define APP_RET_ERROR 	        (-1)
#define APP_RET_NOTFOUND        (-2)
#define APP_RET_BUF_TOO_SMALL   (-3)
#define APP_RET_SOURCE_NO_EXIST (-4)
#define APP_RET_DEST_NO_EXIST   (-5)

#define IP_MODE 1
#define ADHOC_NODE_NUM 200

typedef Int32  STATUS;

/*@-likelybool@*/
typedef unsigned char       BOOL;
#define TRUE                1
#define FALSE               0
/*@+likelybool@*/

typedef unsigned int        PTR;

#endif /* _BASE_TYPE_DEF_H_ */

/* end of file*/

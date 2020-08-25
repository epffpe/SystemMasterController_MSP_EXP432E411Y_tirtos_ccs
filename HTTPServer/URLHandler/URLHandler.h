/*
 * URLHandler.h
 *
 *  Created on: Jun 19, 2019
 *      Author: epenate
 */

#ifndef HTTPSERVER_URLHANDLER_URLHANDLER_H_
#define HTTPSERVER_URLHANDLER_URLHANDLER_H_


typedef int (*pfnURLHandler)(URLHandler_Handle urlHandler, int method,
                            const char * url, const char * urlArgs,
                            int contentLength, int ssock);


typedef struct
{
    //
    //! A pointer to a string containing the name of the command.
    //
    const char *pcCmd;

    //
    //! A function pointer to the implementation of the command.
    //
    pfnURLHandler pfnCmd;

    //
    //! A pointer to a string of brief help text for the command.
    //
    const char *pcHelp;
}
tURLHandlerEntry;




#ifdef  __HTTPSERVER_URLHANDLER_URLHANDLER_GLOBAL
    #define __HTTPSERVER_URLHANDLER_URLHANDLER_EXT
#else
    #define __HTTPSERVER_URLHANDLER_URLHANDLER_EXT  extern
#endif


#ifdef __cplusplus
extern "C" {
#endif

__HTTPSERVER_URLHANDLER_URLHANDLER_EXT tURLHandlerEntry g_psURLTable[];


__HTTPSERVER_URLHANDLER_URLHANDLER_EXT
int URL_apiVersion(URLHandler_Handle urlHandler, int method,
                   const char * url, const char * urlArgs,
                   int contentLength, int ssock);

__HTTPSERVER_URLHANDLER_URLHANDLER_EXT
int URL_apiConfiguration(URLHandler_Handle urlHandler, int method,
                         const char * url, const char * urlArgs,
                         int contentLength, int ssock);

__HTTPSERVER_URLHANDLER_URLHANDLER_EXT
int URL_apiConfigurationNVS(URLHandler_Handle urlHandler, int method,
                            const char * url, const char * urlArgs,
                            int contentLength, int ssock);

__HTTPSERVER_URLHANDLER_URLHANDLER_EXT
int URL_apiZipWebSite(URLHandler_Handle urlHandler, int method,
                      const char * url, const char * urlArgs,
                      int contentLength, int ssock);


__HTTPSERVER_URLHANDLER_URLHANDLER_EXT
int URL_home(URLHandler_Handle urlHandler, int method,
                            const char * url, const char * urlArgs,
                            int contentLength, int ssock);

__HTTPSERVER_URLHANDLER_URLHANDLER_EXT
int URL_index(URLHandler_Handle urlHandler, int method,
                            const char * url, const char * urlArgs,
                            int contentLength, int ssock);

__HTTPSERVER_URLHANDLER_URLHANDLER_EXT
int URL_testFilePost(URLHandler_Handle urlHandler, int method,
                            const char * url, const char * urlArgs,
                            int contentLength, int ssock);

__HTTPSERVER_URLHANDLER_URLHANDLER_EXT
int URL_fileUploadPost(URLHandler_Handle urlHandler, int method,
                            const char * url, const char * urlArgs,
                            int contentLength, int ssock);

#ifdef __cplusplus
}
#endif




#endif /* HTTPSERVER_URLHANDLER_URLHANDLER_H_ */

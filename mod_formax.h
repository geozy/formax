/* 
 * File:   mod_formax.h
 * Author: root
 *
 * Created on 09 November 2014, 13:55
 */

#ifndef MOD_FORMAX_H
#define	MOD_FORMAX_H

#ifdef __cplusplus
#define EXTERN_C_BLOCK_BEGIN    extern "C" {
#define EXTERN_C_BLOCK_END      }
#define EXTERN_C_FUNC           extern "C"
#else
#define EXTERN_C_BLOCK_BEGIN
#define EXTERN_C_BLOCK_END
#define EXTERN_C_FUNC
#endif

#include <stdio.h>
#include <httpd.h>
#include <http_protocol.h>
#include <http_config.h>
#include <ap_config.h>
#include <ap_provider.h>
#include <http_core.h>
#include <http_request.h>

#endif	/* MOD_FORMAX_H */


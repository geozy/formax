#include "mod_formax.h"
#include "CApplication.h"

/* Custom definition to hold any configuration data we may need.
   At this stage we just use it to keep a copy of the CApplication
   object pointer. Later we will add more when we need specific custom
   configuration information. */
EXTERN_C_BLOCK_BEGIN
typedef struct
{
    void* vpCApplication;
} 
FORMAXCONFIG_t;
EXTERN_C_BLOCK_END

/* Forward reference to our custom function to save the FORMAXCONFIG_t* 
   configuration pointer with Apache. */
EXTERN_C_FUNC 
void formax_register_config_ptr( request_rec* inpRequest, FORMAXCONFIG_t* inpFormaxConfig );

/* Forward reference to our custom function to get the FORMAXCONFIG_t* 
   configuration pointer when we need it. */
EXTERN_C_FUNC 
FORMAXCONFIG_t* formax_get_config_ptr( request_rec* inpRequest );

/* Custom function to ensure our CApplication get's deleted at the
   end of the request cycle. */
EXTERN_C_FUNC
apr_status_t formax_delete_capplication_object( void* inPtr )
{
    if ( inPtr )
        delete ( CApplication* )inPtr;
	
    return OK;
}

/* Our custom handler (content generator) 
   */
EXTERN_C_FUNC
int formax_handler( request_rec* inpRequest )
{
    /* Create an instance of our application. */
    CApplication* pApp = new CApplication( inpRequest );
	
    if ( pApp == NULL )
	    return HTTP_SERVICE_UNAVAILABLE;
		    
    /* Register a C function to delete pApp
       at the end of the request cycle. */
    apr_pool_cleanup_register( 
        inpRequest->pool, 
        ( void* )pApp, 
        formax_delete_capplication_object, 
        apr_pool_cleanup_null 
    );
		
    /* Reserve a temporary memory block from the
       request pool to store data between hooks. */
    FORMAXCONFIG_t* pFooConfig = 
        ( FORMAXCONFIG_t* ) apr_palloc( 
            inpRequest->pool, sizeof( FORMAXCONFIG_t ) );
		
    /* Remember our application pointer for future calls. */
    pFooConfig->vpCApplication = ( void* )pApp;
		
    /* Register our config data structure for our module. */
    formax_register_config_ptr( inpRequest, pFooConfig );
		
    /* Run our application handler. */
    return pApp->RunHandler();
}

/* Apache callback to register our hooks.
   */
EXTERN_C_FUNC
void formax_hooks( apr_pool_t* inpPool )
{
    ap_hook_handler( formax_handler, NULL, NULL, APR_HOOK_MIDDLE );
}

/* Our standard module definition.
   */
EXTERN_C_BLOCK_BEGIN
module AP_MODULE_DECLARE_DATA formax_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    formax_hooks
};
EXTERN_C_BLOCK_END

/* Custom function to register our FORMAXCONFIG_t* pointer with Apache
   for retrieval later as required. */
EXTERN_C_FUNC
void formax_register_config_ptr( request_rec* inpRequest, FORMAXCONFIG_t* inpPtr )
{
    ap_set_module_config( inpRequest->request_config, &formax_module, ( void* )inpPtr );
}

/* Custom function to retrieve our FORMAXCONFIG_t* pointer previously
   registered with Apache on this request cycle. */
EXTERN_C_FUNC
FORMAXCONFIG_t* formax_get_config_ptr( request_rec* inpRequest )
{
    FORMAXCONFIG_t* pReturnValue = NULL;
	
    if ( inpRequest != NULL )
    {
        pReturnValue = 
            ( FORMAXCONFIG_t* )ap_get_module_config( 
                inpRequest->request_config, &formax_module );
    }
	
    return pReturnValue;
}

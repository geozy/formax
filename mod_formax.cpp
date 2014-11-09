#include "mod_formax.h"
#include "CApplication.h"

/* Holds custom
   configuration information. */
EXTERN_C_BLOCK_BEGIN
typedef struct
{
    char    context[256];
    char    path[256];
    int     action;
    int     enabled;
} 
FORMAXCONFIG_t;
EXTERN_C_BLOCK_END

        
        
 /* Custom definition to hold any configuration data we may need.
   At this stage we just use it to keep a copy of the CApplication
   object pointer. Later we will add more when we need specific custom
   configuration information. */       
EXTERN_C_BLOCK_BEGIN
typedef struct
{
    void* vpCApplication;
} 
FORMAXMETA_t;
EXTERN_C_BLOCK_END
    
        
        
/* Forward reference to our custom function to save the FORMAXCONFIG_t* 
   configuration pointer with Apache. */
EXTERN_C_FUNC 
void formax_register_meta_ptr( request_rec* inpRequest, FORMAXMETA_t* inpFormaxConfig );

/* Forward reference to our custom function to get the FORMAXMETA_t* 
   configuration pointer when we need it. */
EXTERN_C_FUNC 
FORMAXMETA_t* formax_get_meta_ptr( request_rec* inpRequest );

EXTERN_C_FUNC 
void* formax_create_dir_conf( apr_pool_t* pool, char* context );

EXTERN_C_FUNC
void* formax_merge_dir_conf( apr_pool_t* pool, void* BASE,void* ADD );

EXTERN_C_FUNC
void formax_hooks( apr_pool_t* inpPool );


static const char* formax_set_enabled( cmd_parms* cmd,void *cfg,const char* arg );
static const char* formax_set_path( cmd_parms* cmd,void *cfg,const char* arg );
static const char* formax_set_action( cmd_parms* cmd,void *cfg,const char* arg1,const char* arg2 );

//############################################################################


EXTERN_C_BLOCK_BEGIN
static const command_rec    formax_directives []=
{  
    AP_INIT_TAKE1("formaxEnabled",(cmd_func)formax_set_enabled,NULL,RSRC_CONF, "Enable or disable mod_formax" ),
    AP_INIT_TAKE1("formaxPath",(cmd_func)formax_set_path,NULL,RSRC_CONF,"The path"),
    AP_INIT_TAKE2("formaxAction",(cmd_func)formax_set_action,NULL,RSRC_CONF,"Formax action value"),
    {NULL}
};
EXTERN_C_BLOCK_END


//#######################################################################

/* Custom function to ensure our CApplication get's deleted at the
   end of the request cycle. */
EXTERN_C_FUNC
apr_status_t formax_delete_capplication_object( void* inPtr )
{
    if ( inPtr )
        delete ( CApplication* )inPtr;
	
    return OK;
}





EXTERN_C_FUNC
void* formax_create_dir_conf( apr_pool_t* pool, char* context )
{
    context = context ? context : (char*)"undefined context";
    FORMAXCONFIG_t*  cfg=(FORMAXCONFIG_t*)apr_pcalloc(pool,sizeof(FORMAXCONFIG_t));
    if(cfg){
        // set default values
        strcpy(cfg->context,"");
        cfg->enabled=0;
        strcpy(cfg->path,"");
        cfg->action=0;
    }
    
    return cfg;
}

EXTERN_C_FUNC
void* formax_merge_dir_conf( apr_pool_t* pool, void* BASE,void* ADD )
{
    FORMAXCONFIG_t*  base=(FORMAXCONFIG_t*)BASE;
    FORMAXCONFIG_t*  add=(FORMAXCONFIG_t*)ADD;
    FORMAXCONFIG_t*  conf=(FORMAXCONFIG_t*)formax_create_dir_conf(pool,(char*)"Merged configuration");
    
    // Merge Configurations
    conf->enabled = (add->enabled == 0) ? base->enabled : add->enabled;
    conf->action =add->action ? add->action:base->action;
    strcpy(conf->path,strlen(add->path) ? add->path:base->path);
    
    return conf;
}






/* Handler for formaxEnabled Directive */

static const char* formax_set_enabled( cmd_parms* cmd,void *cfg,const char* arg )
{
    FORMAXCONFIG_t *conf=(FORMAXCONFIG_t*) cfg;
    if(conf){
        conf->enabled= strcasecmp(arg,"on") ? 0:1; 
    }
    
    return NULL;
}

/* Handler for formaxPath Directive */

static const char* formax_set_path( cmd_parms* cmd,void *cfg,const char* arg )
{
    FORMAXCONFIG_t *conf=(FORMAXCONFIG_t*) cfg;
    if(conf){
        strcpy(conf->path,arg);
    }
   
    return NULL;
}

/* Handler for formaxAction Directive */
static const char* formax_set_action( cmd_parms* cmd,void *cfg,const char* arg1,const char* arg2 )
{
    FORMAXCONFIG_t *conf=(FORMAXCONFIG_t*) cfg;
    if(conf){
        //TODO
    }
    return NULL;
}


/* Our standard module definition.
   */
EXTERN_C_BLOCK_BEGIN
module AP_MODULE_DECLARE_DATA formax_module =
{
    STANDARD20_MODULE_STUFF,
    formax_create_dir_conf,
    formax_merge_dir_conf,
    NULL,
    NULL,
    formax_directives,
    formax_hooks
};
EXTERN_C_BLOCK_END

        
 /* Custom function to retrieve our FORMAXMETA_t* pointer previously
   registered with Apache on this request cycle. */
EXTERN_C_FUNC
FORMAXMETA_t* formax_get_meta_ptr( request_rec* inpRequest )
{
    FORMAXMETA_t* pReturnValue = NULL;
	
    if ( inpRequest != NULL )
    {
        pReturnValue = 
            ( FORMAXMETA_t* )ap_get_module_config( 
                inpRequest->request_config, &formax_module );
    }
	
    return pReturnValue;
}
 
        
  /* Custom function to register our FORMAXMETA_t* pointer with Apache
   for retrieval later as required. */
EXTERN_C_FUNC
void formax_register_meta_ptr( request_rec* inpRequest, FORMAXMETA_t* inpPtr )
{
    ap_set_module_config( inpRequest->request_config, &formax_module, ( void* )inpPtr );
}      
        
 
        
        
/* Our custom handler (content generator) 
   */
EXTERN_C_FUNC
int formax_handler( request_rec* inpRequest )
{
    // fetch any configuration data
    FORMAXCONFIG_t* config=(FORMAXCONFIG_t*)ap_get_module_config(inpRequest->per_dir_config,&formax_module);
    
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
    FORMAXMETA_t* pMeta = 
        ( FORMAXMETA_t* ) apr_palloc( 
            inpRequest->pool, sizeof( FORMAXMETA_t ) );
		
    /* Remember our application pointer for future calls. */
    pMeta->vpCApplication = ( void* )pApp;
		
    /* Register our config data structure for our module. */
    formax_register_meta_ptr( inpRequest, pMeta );
		
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


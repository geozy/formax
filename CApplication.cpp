#include "CApplication.h"

int
CApplication::RunHandler()
{
    int nReturnVal = DECLINED;
	
    if ( m_pRequestRec->handler != NULL && strcmp( m_pRequestRec->handler, "formax" ) == 0 )
    {
        ap_rputs( "Hello World from FORMAX", m_pRequestRec );
        nReturnVal = OK;
    }

    return nReturnVal;
}


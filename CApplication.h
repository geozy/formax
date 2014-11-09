/* 
 * File:   CApplication.h
 * Author: root
 *
 * Created on 09 November 2014, 14:09
 */

#ifndef CAPPLICATION_H
#define	CAPPLICATION_H

#include "mod_formax.h"

class CApplication
{
private:

    request_rec*    m_pRequestRec;
	
public:
	
    CApplication( request_rec* inpRequestRec ) :
        m_pRequestRec( inpRequestRec )
    {}
	
    int RunHandler();
};

#endif	/* CAPPLICATION_H */


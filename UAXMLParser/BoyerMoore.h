/*
 *  Utils.h
 *  nntptest
 *
 *  Created by Kristian Trenskow on 12/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __BOYERMOORE_H
#define __BOYERMOORE_H

#include <string.h>

#define BM_NONEDSTEDRESULTS         (1)
#define BM_EXTENDSEARCHPREBUFFER      (1 << 1)
#define BM_EXTENDSEARCHPOSTBUFFER       (1 << 2)
#define BM_ALLOWPARTIALMATCH        BM_EXTENDSEARCHPREBUFFER | BM_EXTENDSEARCHPOSTBUFFER
#define BM_KEEPCOUNTINGBEYONDBUFFER (1 << 3)

namespace xml {

	class BoyerMoore {

	public:
		BoyerMoore(const char* n, size_t ns);
		~BoyerMoore();
		
		virtual size_t Search(const char* h, size_t hs, long* r, size_t rs, unsigned char options);
		virtual size_t StreamSearch(const char* h, size_t hs, long* r, size_t rs);
		
		void ResetStream();
		
	private:
		size_t _bcs[256];
		char* _n;
		size_t _ns;
		
		size_t _pp;
		
	};
	
}

#endif
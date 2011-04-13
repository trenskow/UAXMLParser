#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "BoyerMoore.h"

using namespace xml;

BoyerMoore::BoyerMoore(const char* n, size_t ns) {

	int i;
	for (i = 0 ; i < 256 ; i ++)
		this->_bcs[i] = ns;
	
	for (i = 0 ; i < ns - 1; i++)
		this->_bcs[(unsigned char)n[i]] = (ns - 1) - i;
	
	this->_n = (char*) malloc(ns);
	
	memcpy(this->_n, n, ns);
	
	this->_ns = ns;
	
	this->_pp = 0;
	
}

BoyerMoore::~BoyerMoore() {

	free(this->_n);

}

size_t BoyerMoore::Search(const char* h, size_t hs, long* r, size_t rs, unsigned char options) {
	
	if (hs < this->_ns)
		return 0;
	
	size_t ret = 0;
	
	long rj = 0;	
	if ((options & BM_NONEDSTEDRESULTS) == 1)
		rj = this->_ns - 1;
	
	long st = 0;
	long en = hs - (this->_ns - 1);
	
	if ((options & BM_EXTENDSEARCHPREBUFFER) >> 1 == 1)
		st = 1 - this->_ns;
	
	if ((options & BM_EXTENDSEARCHPOSTBUFFER) >> 2 == 1)
		en = hs;
	
	long i, bms, cpos;
	for (i = st ; i < en ; i++) {

		cpos = this->_ns - 1;
		while (cpos >= 0 && (i + cpos < 0 || i + cpos >= hs || h[i + cpos] == this->_n[cpos]))
			cpos--;
		
		if (cpos >= 0) {
			bms = this->_bcs[(unsigned char)h[i + cpos]] - ((this->_ns-1)-cpos) - 1;
			if (cpos == (long) this->_ns - 1 && bms > 0) // This is a hack to circumvent the seconds table. slows it down.
				i += bms;
		} else {
			if (ret < rs) {
				if (r != NULL)
					r[ret] = i;
				i += rj;
			}
			ret++;
		}

		
		if (ret == rs && ((options & BM_KEEPCOUNTINGBEYONDBUFFER) >> 3) == 0)
			break;
		
	}
	
	return ret;

}

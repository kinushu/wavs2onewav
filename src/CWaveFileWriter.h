/*
 * CWaveFileWriter.h
 *
 *  Created on: 2010/12/02
 *      Author: kinukawa
 */

#ifndef CWAVEFILEWRITER_H_
#define CWAVEFILEWRITER_H_

#include "CWavFileReader.h"

class CWavFileWriter  : public CWavFileBase {
public:
	CWavFileWriter();
	virtual ~CWavFileWriter();

	int open(const char* file,uint16_t & ch, uint32_t & smprate, uint16_t & bits);
	bool write(void*, int);
	void close();

protected:
    std::ofstream 	m_fstrm;

};

#endif /* CWAVEFILEWRITER_H_ */

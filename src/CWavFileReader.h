/*
 * CWavFileReader.h
 *
 *  Created on: 2010/12/01
 *      Author: shuhei kinukawa
 */

#ifndef CWAVFILEREADER_H_
#define CWAVFILEREADER_H_

#include <string>
#include <fstream>

typedef struct {
	uint16_t wFormatTag;
	uint16_t nChannels;
	uint32_t nSamplesPerSec;
	uint32_t nAvgBytesPerSec;
	uint16_t nBlockAlign;
	uint16_t wBitsPerSample;
} __attribute__ ((packed)) sWaveFormatEx;

class CWavFileBase {
public:
	CWavFileBase();
	virtual ~CWavFileBase();

	void clear();

protected:
    uint32_t		m_smpcnt;
	std::streampos	m_smptoppos;
public:
    sWaveFormatEx	m_wavfmt;

    uint32_t getSamplesCount(void);
};

class CWavFileReader : public CWavFileBase{
public:
	CWavFileReader();
	virtual ~CWavFileReader();

	/*success=0, err non 0*/
	int open(const char* srcfile);
	unsigned int read(void*, unsigned int);
	void close();

protected:
    std::string 	m_filename;
    std::ifstream 	m_fstrm;


};

#endif /* CWAVFILEREADER_H_ */

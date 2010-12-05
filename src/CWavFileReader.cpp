/*
 * CWavFileReader.cpp
 *
 *  Created on: 2010/12/01
 *      Author: shuhei kinukawa
 */
#include <iostream>
#include <stdexcept>

#include "CWavFileReader.h"

CWavFileBase::CWavFileBase()
{

}
CWavFileBase::~CWavFileBase()
{

}
void CWavFileBase::clear()
{
	memset(&m_wavfmt, 0, sizeof(m_wavfmt));
	m_smpcnt = 0;
	m_smptoppos = 0;
}

uint32_t CWavFileBase::getSamplesCount(void)
{
	return m_smpcnt;
}

//////////////

CWavFileReader::CWavFileReader() {
}

CWavFileReader::~CWavFileReader() {
	close();
}

int CWavFileReader::open(const char* srcfile)
{
	int ret = 0;

	uint32_t smpbytes = 0;

	try{
		m_fstrm.open(srcfile, std::ios::binary);
		if(m_fstrm.fail()){
			ret = -1;
			throw std::logic_error("open error");
		}

		{
			const int nRiff = 4;
			char sRiff[nRiff] = {0};
			m_fstrm.read(sRiff, nRiff);
			if( ::strncmp(sRiff,"RIFF", nRiff) != 0){
				ret = -2;
				throw std::logic_error("invalid RIFF");
			}
		}

		{
			const int nSize = 4;
			uint32_t fdsize = 0;
			char* p = (char*)&fdsize;
			if(nSize != m_fstrm.readsome(p, nSize)){
				ret = -3;
				throw std::logic_error("invalid size");
			}
		}

		{
			const int nRiff = 4;
			char sRiff[nRiff] = {0};
			m_fstrm.read(sRiff, nRiff);
			if( ::strncmp(sRiff,"WAVE", nRiff) != 0){
				ret = -4;
				throw std::logic_error("nonsupported WAV");
			}
		}

		//chunk
		const int nChunk = 4;
		char sChunk[nChunk+1] = {0};
		while( m_fstrm.readsome(sChunk, nChunk) == nChunk )
		{
			std::streampos datapos = m_fstrm.tellg();

			const int nSizeArea = 4;
			uint32_t chunksize = 0;
			{
				char* p = (char*)&chunksize;
				if(nSizeArea != m_fstrm.readsome(p, nSizeArea)){
					ret = -6;
					throw std::logic_error("invalid chunk size");
				}
			}
			/*
			std::cout << "chunk " << sChunk << std::endl;
			std::cout << "chunkpos " << datapos  << std::endl;
			std::cout << "chunksize " << chunksize << std::endl;
			*/
			if( ::strncmp(sChunk,"fmt ", nChunk) == 0){
				const uint32_t fmtsize = sizeof(m_wavfmt);
				//std::cout << "fmt size=" << fmtsize << std::endl;
				if( chunksize >= fmtsize){
					if( fmtsize < m_fstrm.readsome( (char*)&m_wavfmt, fmtsize) ){
						ret = -7;
						throw std::logic_error("read error wavfmt");
					}

					if( m_wavfmt.wFormatTag != 1){
						ret = -7;
						throw std::logic_error("nosupported wav format");
					}
					/*
					std::cout << "ch: " << m_wavfmt.nChannels << std::endl;
					std::cout << "smp:" << m_wavfmt.nSamplesPerSec << std::endl;
					std::cout << "bit:" << m_wavfmt.wBitsPerSample << std::endl;
					*/
				}
				else{
					ret = -8;
					throw std::logic_error("read error wavfmt sizemin");
				}
			}
			else if( ::strncmp(sChunk,"data", nChunk) == 0){
				smpbytes = chunksize;
				//std::cout << "data " << std::endl;
				m_smptoppos = m_fstrm.tellg();
			}

			/*seek to next chunk postion for read*/
			std::streamoff off = chunksize + nSizeArea;
			std::streampos nextchunkpos = datapos;
			nextchunkpos += off;
//			std::streampos nextchunkpos = datapos + (long long)(chunksize + nSizeArea);
			m_fstrm.seekg(nextchunkpos, std::ios_base::beg);
		}

		//successed : seek to sample top postision
		m_fstrm.seekg(m_smptoppos, std::ios_base::beg);
	}
	catch(std::exception & e){
		std::cerr << "catch: "<< e.what() << std::endl;
		close();
	}

	if(m_wavfmt.nBlockAlign > 0){
		m_smpcnt = smpbytes / m_wavfmt.nBlockAlign;
	}
	std::cout << "wavopen " << srcfile << " " << ret << std::endl;

	return ret;
}

unsigned int CWavFileReader::read(void* p, unsigned int smp)
{
	if(m_wavfmt.nBlockAlign > 0){
		unsigned int bytes = smp * m_wavfmt.nBlockAlign;

		unsigned int readbytes = m_fstrm.readsome((char*)p, bytes);

		return readbytes / m_wavfmt.nBlockAlign;
	}
	return 0;
}

void CWavFileReader::close()
{
	m_fstrm.close();
	clear();
}

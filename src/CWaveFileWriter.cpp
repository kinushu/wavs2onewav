/*
 * CWaveFileWriter.cpp
 *
 *  Created on: 2010/12/02
 *      Author: shuhei kinukawa
 */
#include <iostream>
#include <stdexcept>

#include "CWaveFileWriter.h"

CWavFileWriter::CWavFileWriter() {

}

CWavFileWriter::~CWavFileWriter() {

}

int CWavFileWriter::open(const char* file, uint16_t & ch, uint32_t & smprate, uint16_t & bits)
{
	int ret = 0;
	try{
		m_fstrm.open(file, std::ios::out | std::ios::binary | std::ios::trunc);
		if(m_fstrm.fail()){
			ret = -1;
			throw std::logic_error("open error");
		}

		m_fstrm.write("RIFF", 4);
		uint32_t fdsize = 0;
		m_fstrm.write((char*)&fdsize, sizeof(fdsize));

		m_fstrm.write("WAVE", 4);

		{
			m_fstrm.write("fmt ", 4);

			m_wavfmt.wFormatTag		= 1;
			m_wavfmt.nChannels		= ch;
			m_wavfmt.nSamplesPerSec	= smprate;
			m_wavfmt.wBitsPerSample	= bits;
			m_wavfmt.nBlockAlign	= m_wavfmt.nChannels * (m_wavfmt.wBitsPerSample/8);
			m_wavfmt.nAvgBytesPerSec= m_wavfmt.nBlockAlign * m_wavfmt.nSamplesPerSec;

			const uint32_t fmtsize = sizeof(m_wavfmt);
			m_fstrm.write((char*)&fmtsize, sizeof(fmtsize));
			m_fstrm.write((char*)&m_wavfmt, fmtsize);
		}
		{
			m_fstrm.write("data", 4);

			const uint32_t datasize = 0;
			m_fstrm.write((char*)&datasize, sizeof(datasize));

			m_smptoppos = m_fstrm.tellp();
		}
	}
	catch(std::exception & e){
		std::cerr << "catch: "<< e.what() << std::endl;
		close();
	}

//	std::cout << "wavopen " << srcfile << " " << ret << std::endl;

	return ret;

}
bool CWavFileWriter::write(void* p, int smpcnt)
{
	if(m_wavfmt.nBlockAlign > 0){
		int bytes = m_wavfmt.nBlockAlign * smpcnt;
		m_smpcnt += smpcnt;
		m_fstrm.write((char*)p, bytes);
		return true;
	}
	return false;
}

void CWavFileWriter::close()
{
	if(m_wavfmt.nBlockAlign > 0){
		int smpbytes = m_wavfmt.nBlockAlign * m_smpcnt;

		uint32_t fdsize = smpbytes + m_smptoppos - 8;
		m_fstrm.seekp(4);
		m_fstrm.write((char*)&fdsize, sizeof(fdsize));

		{
			const uint32_t datasize = smpbytes;
			std::streamoff off = sizeof(datasize);
			m_fstrm.seekp( m_smptoppos - off);
			m_fstrm.write((char*)&datasize, sizeof(datasize));
		}
	}
	clear();
	m_fstrm.close();
}

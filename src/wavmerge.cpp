//============================================================================
// Name        : wavmerge.cpp for wavs2onewav
// Author      : shuhei kinukawa
// Version     : 0.10
//============================================================================

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <memory>

#include "CWavFileReader.h"
#include "CWaveFileWriter.h"

class CArgvProc{
public:
	CArgvProc(){};
	virtual ~CArgvProc(){};

	virtual void Check(const char* line, bool bOption) = 0;

	void Parse(int argc, char**argv){
		/*from option*/
		for(int i=1; i<argc; i++){

			bool bOption = false;
			char topc = argv[i][0];
			if(topc == '-'){
				bOption = true;
			}
			std::string	line = argv[i];
			if(bOption){
				line = line.substr(1, line.length()-1);
			}
			Check(line.c_str(), bOption);

		}
	}
};

class CWavMergeOptionProc : public CArgvProc{
public:
	CWavMergeOptionProc(){
		m_mode = st_src;
	};
	virtual ~CWavMergeOptionProc(){};

	enum{
		st_unknown = -1,
		st_none = 0,
		st_src,
		st_dst,
		st_help,
		st_version,
	};
	int m_mode;
	std::vector<std::string> m_vcsrc;
	std::string		m_dst;

	virtual void Check(const char* line, bool bOption){
		std::string		s = line;
		if(bOption){
			if(s == "o"){
				m_mode = st_dst;
			}
			else if(s == "h"){
				m_mode = st_help;
			}
			else if(s == "v"){
				m_mode = st_version;
			}
			else{
				m_mode = st_unknown;
			}
		}
		else{
			if(m_mode == st_src){
				//std::cout << "push " << line << std::endl;
				m_vcsrc.push_back(line);
			}
			else if(m_mode == st_dst){
				m_dst = line;
				m_mode = st_src;
			}
		}
		//std::cout << line << std::endl;
	};

};

int mergeWavFiles(const char* outputfile, const std::vector<std::string> & vcsrcfiles )
{
	int ret = 0;

	CWavFileReader* paWF = NULL;
	char* pSrc = NULL;
	char* pDst = NULL;
	try{
		int nCnt = vcsrcfiles.size();
		if( nCnt < 2 ){
			ret = -2;
			throw std::logic_error("please select larger than 2 input files");
		}
		paWF = new CWavFileReader[nCnt];

		uint16_t nTotalCh = 0;
		uint32_t nSmpRate = 0;
		uint16_t nBitsPerSmp = 0;
		uint32_t nTotalSmp = 0;

		std::vector<int> anCh;
		anCh.resize(nCnt);

		/*check input file format and decide output format*/
		for(int i=0; i < nCnt; i++){
			CWavFileReader& wf = paWF[i];
			if(wf.open(vcsrcfiles[i].c_str()) != 0){
				ret = -1;
				throw std::logic_error("can not open file");
			}
			const sWaveFormatEx* pwfmt = &wf.m_wavfmt;
			anCh[i] = pwfmt->nChannels;
			nTotalCh += pwfmt->nChannels;

			if(i == 0){
				/*decision dst wave format*/
				nSmpRate = pwfmt->nSamplesPerSec;
				nBitsPerSmp = pwfmt->wBitsPerSample;
			}
			else{
				if(nSmpRate != pwfmt->nSamplesPerSec){
					/*sampling rate converter none*/
					ret = -3;
					throw std::logic_error("can not convert sampling rate");
				}
				if(pwfmt->wBitsPerSample != nBitsPerSmp){
					ret = -4;
					throw std::logic_error("can not convert sampling bits");
				}
			}
			nTotalSmp = std::max<uint32_t>(nTotalSmp, wf.getSamplesCount());
		}
		std::cout << "samplecount " << nTotalSmp << std::endl;

		/*open output file*/
		CWavFileWriter	dstWF;
		std::string sDstPath = outputfile;
		if(	dstWF.open(
				sDstPath.c_str(), nTotalCh, nSmpRate, nBitsPerSmp)
			!= 0)
		{
			ret = -5;
			throw std::logic_error("open error dst wav file");
		}

		unsigned int nBytesPerSmp = (nBitsPerSmp/8);
		int bufsize = nSmpRate * nTotalCh * nBytesPerSmp;
		pSrc = new char[bufsize];
		pDst = new char[bufsize];

		/*loop for getting samples from input files and puting samples to output file*/
		uint32_t nWrSmp = 0;
		unsigned int wrsmp = 0;
		while(nWrSmp < nTotalSmp){
			memset(pSrc, 0, bufsize);

			unsigned int smp = nSmpRate;
			unsigned int maxreadsmp = 0;
			int chOff = 0;

			for(int i=0; i < nCnt; i++){
				unsigned int nowreadsmp = paWF[i].read(pSrc, smp);
				if(nowreadsmp > 0){
					int ch = anCh[i];
					for(unsigned int n=0; n < nowreadsmp; n++){
						for(int c=0; c < ch; c++){
							memcpy( &pDst[ ( nTotalCh*n + (c+chOff) ) * nBytesPerSmp  ]
									, &pSrc[ (ch*n + c) * nBytesPerSmp ]
									, nBytesPerSmp);
						}
					}
				}
				maxreadsmp = std::max<unsigned int>(maxreadsmp, nowreadsmp);

				chOff += anCh[i];
			}

			if(maxreadsmp > 0){
				dstWF.write(pDst, maxreadsmp);
				wrsmp += maxreadsmp;
			}
			else{
				break;
			}
		}
		dstWF.close();

	}
	catch(std::exception & e){
		std::cerr << "error: "<< e.what() << std::endl;


	}
	delete [] pSrc;
	delete [] pDst;

	delete [] paWF;

	return ret;
}

int main(int argc, char**argv) {

	CWavMergeOptionProc	cmd;
	cmd.Parse(argc, argv);

	switch(cmd.m_mode){
	case CWavMergeOptionProc::st_help:
	{
		std::cout <<
"Usage: wavs2onewav [-o output file] [input file] [input file] ...\n"
"-o\toutput .wav file name\n"
;

	}
	return 0;
	case CWavMergeOptionProc::st_version:
	{
		std::cout << "wavs2onewav 0.10\n";
	}
	return 0;
	}

	int ret = mergeWavFiles(cmd.m_dst.c_str(), cmd.m_vcsrc);

	return ret;
}

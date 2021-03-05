#pragma once

#include <Prelude.h>

#define WAVE_FORMAT_UNKNOWN      0X0000;
#define WAVE_FORMAT_PCM          0X0001;
#define WAVE_FORMAT_MS_ADPCM     0X0002;
#define WAVE_FORMAT_IEEE_FLOAT   0X0003;
#define WAVE_FORMAT_ALAW         0X0006;
#define WAVE_FORMAT_MULAW        0X0007;
#define WAVE_FORMAT_IMA_ADPCM    0X0011;
#define WAVE_FORMAT_YAMAHA_ADPCM 0X0016;
#define WAVE_FORMAT_GSM          0X0031;
#define WAVE_FORMAT_ITU_ADPCM    0X0040;
#define WAVE_FORMAT_MPEG         0X0050;
#define WAVE_FORMAT_EXTENSIBLE   0XFFFE;

//#define SAMPLE_RATE 48000 // 48000Hz for sample rate
#define WAVE_LISTSIZE 48000//linked list size, 반드시 2의 배수일 것
#define WAVE_MULTYPLAY_PRESSURE 10000
//#define CHANNEL 1
//-------------------------------------------
// [Channel]
// - streo     : [left][right]
// - 3 channel : [left][right][center]
// - quad      : [front left][front right][rear left][reat right]
// - 4 channel : [left][center][right][surround]
// - 6 channel : [left center][left][center][right center][right][surround]
//-------------------------------------------


#define BYTE_RATE 2

class Wave {
private:
	struct RIFF
	{
		unsigned char ChunkID[4];
		unsigned int ChunkSize;
		unsigned char Format[4];
	};
	struct FMT {
		unsigned char  ChunkID[4];    // Contains the letters "fmt " in ASCII form
		unsigned int   ChunkSize;     // 16 for PCM.  This is the size of the rest of the Subchunk which follows this number.
		unsigned short AudioFormat;   // PCM = 1
		unsigned short NumChannels;   // Mono = 1, Stereo = 2, etc.
		unsigned int   SampleRate;    // 8000, 44100, etc.
		unsigned int   AvgByteRate;   // SampleRate * NumChannels * BitsPerSample/8
		unsigned short BlockAlign;    // NumChannels * BitsPerSample/8
		unsigned short BitPerSample;  // 8 bits = 8, 16 bits = 16, etc
	};
	struct DATA{
		char          ChunkID[4];    // Contains the letters "data" in ASCII form
		unsigned int  ChunkSize;     // NumSamples * NumChannels * BitsPerSample/8
	};
	struct LIST
	{
		char SoundData[WAVE_LISTSIZE];
		LIST* NextList;
	};

	//저장될 파일 및 녹음 관련
	char* Name;
	
	unsigned char Channel;
	unsigned int SampleRate;

	int StandardPressure; // StandardPressure * WAVE_MULTYPLAY_PRESSURE. (소수 네째자리까지 기록)
	//헤더 파일들
	RIFF riff;
	FMT fmt; 
	DATA data;

	//데이터
	int SoundDatalength; // Length of raw sound data chunk
	LIST* head;//파일 쓰기와 flush를 수행하기 위한 linked list의 시작점
	LIST* tail;//입력 속도를 높이기 위한 linked list의 끝점

	enum Mode
	{
		close,//no name, no data
		//recorded,//exist data, not noname. only available make new file
		create,//name exist, 
		//append,//if exist file open.
	};

	Mode state;//주기적 저장과 순서 꼬임을 방지하기 위한 wave class state memory

	void WriteHeader(std::ofstream& f_out);

public:

	Wave(); // Initialize
	~Wave();
	
	void Open(char* InName);
	void Flush();
	//Close();

	void Append(const float &SoundPressure); // Append data chunk
	//void SaveWave(); // Save WAV file

};


Wave::Wave() {
	Name = NULL;
	Channel = 1;
	SampleRate = 48000;
	// Initialize RIFF chunk
	memcpy(riff.ChunkID, "RIFF", 4);
	riff.ChunkSize = 36; // following size of byte
	memcpy(riff.Format, "WAVE", 4);

	// Initialize FMT chunk
	memcpy(fmt.ChunkID, "fmt ", 4);
	fmt.ChunkSize = 0x10;
	fmt.AudioFormat = WAVE_FORMAT_PCM;
	fmt.NumChannels = Channel;
	fmt.SampleRate = SampleRate; // number of samples per second
	fmt.AvgByteRate = SampleRate * Channel * BYTE_RATE; // The number of byte used in a second
	fmt.BlockAlign = Channel * BYTE_RATE; // size of sample frame
	fmt.BitPerSample = BYTE_RATE * 8; // The number of bit per sample. BIT_RATE / 8 = number of byte

	// Initialize DATA chunk
	memcpy(data.ChunkID, "data", 4);
	SoundDatalength = 0;
	head = tail = new LIST;
	head->NextList = NULL;
	//// data.ChunkSize is determined after recording is over.

	state = Mode::close;
}
Wave::~Wave()
{
	delete[] Name;
	while (0 < SoundDatalength)
	{
		if (SoundDatalength >= WAVE_LISTSIZE)
		{
			SoundDatalength -= WAVE_LISTSIZE;
			tail = head->NextList;
			delete head;
			head = tail;
		}
		else
		{
			SoundDatalength = 0;
			delete head;
		}
	}
}

void Wave::Open(char* name)
{
	if (name[0] == NULL)
	{
		printf("ERROR :: NULL can not be filename!");
	}
	else if (state == Mode::create)
	{
		printf("ERROR :: Wave file already created");
	}
	else
	{
		if (Name != NULL) { delete[] Name; }
		Name = new char[strlen(name)+5];
		Name[0] = '\0';
		strcpy_s(Name, strlen(name)+1, name);

		strcat_s(Name, strlen(name) + 5, ".wav");
		state = Mode::create;
	}
}
void Wave::Append(const float &SoundPressure) {
	/*
	Value is raw sound data is amplitude of sound pressure.
	reference from: http://www.billposer.org/Linguistics/Computation/LectureNotes/AudioData.html
	*/

	// Version 1: use pressure with scaling
    // Need to be modified with reliable reference
	int SoundOut = ((int)(WAVE_MULTYPLAY_PRESSURE*SoundPressure)) - StandardPressure;

	// cannot use strcat() due to 0x00
	tail->SoundData[SoundDatalength%WAVE_LISTSIZE] = ((char*)&SoundOut)[0];
	tail->SoundData[(SoundDatalength%WAVE_LISTSIZE)+1] = ((char*)&SoundOut)[1];
	
	if ((SoundDatalength%WAVE_LISTSIZE) + 2 >= WAVE_LISTSIZE)
	{
		tail->NextList = new LIST;
		tail = tail->NextList;
		tail->NextList = NULL;
		//링크드 리스트 연장
	}
	SoundDatalength += 2;
}

void Wave::Flush() {

	
	switch (state)
	{
	case Wave::close:
		printf("ERROR :: No name set");
		break;
	case Wave::create:
		{
			state = Mode::close;

			riff.ChunkSize = 36 + SoundDatalength;
			data.ChunkSize = SoundDatalength;

			// Open file
			std::ofstream f_out(Name, std::ios::binary | std::ios::out);

			// Write header on file
			WriteHeader(f_out);

			while (0 < SoundDatalength)
			{
				if (SoundDatalength >= WAVE_LISTSIZE)
				{
					f_out.write(head->SoundData, WAVE_LISTSIZE);
					SoundDatalength -= WAVE_LISTSIZE;
					tail = head->NextList;
					delete head;
					head = tail;
				}
				else
				{
					f_out.write(head->SoundData, SoundDatalength);
					SoundDatalength = 0;
					delete head;
				}
			}

			f_out.close();
		}
	default:
		printf("ERROR :: Unknown wave class state");
		break;
	}

	
}

void Wave::WriteHeader(std::ofstream& f_out)
{
	f_out.write((char*)&riff.ChunkID, sizeof(riff.ChunkID));
	f_out.write((char*)&riff.ChunkSize, sizeof(riff.ChunkSize));
	f_out.write((char*)&riff.Format, sizeof(riff.Format));

	f_out.write((char*)&fmt.ChunkID, sizeof(fmt.ChunkID));
	f_out.write((char*)&fmt.ChunkSize, sizeof(fmt.ChunkSize));
	f_out.write((char*)&fmt.AudioFormat, sizeof(fmt.AudioFormat));
	f_out.write((char*)&fmt.NumChannels, sizeof(fmt.NumChannels));
	f_out.write((char*)&fmt.SampleRate, sizeof(fmt.SampleRate));
	f_out.write((char*)&fmt.AvgByteRate, sizeof(fmt.AvgByteRate));
	f_out.write((char*)&fmt.BlockAlign, sizeof(fmt.BlockAlign));
	f_out.write((char*)&fmt.BitPerSample, sizeof(fmt.BitPerSample));

	f_out.write((char*)&data.ChunkID, sizeof(data.ChunkID));
	f_out.write((char*)&data.ChunkSize, sizeof(data.ChunkSize));;
}
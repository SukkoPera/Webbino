#ifndef CONTENT_H_INCLUDED
#define CONTENT_H_INCLUDED

#include <webbino_config.h>
#include <webbino_debug.h>

//20440 1357

class PageContent {
public:
	virtual char getNextByte () = 0;
};

/******************************************************************************/


class FlashContent: public PageContent {
private:
	const Page& page;
	PGM_P next;

public:
	FlashContent (const Page* p): page (*p), next (p -> getContent ()) {
	}

	char getNextByte () override {
		return pgm_read_byte (next++);
	}
};

/******************************************************************************/


#if defined (WEBBINO_ENABLE_SD) || defined (WEBBINO_ENABLE_SDFAT)

#if defined (WEBBINO_ENABLE_SD)
#include <SD.h>
#elif defined (WEBBINO_ENABLE_SDFAT)
#include <SdFat.h>

static SdFat SD;
#endif

struct SDContent: public PageContent {
private:
	File file;

public:
	SDContent (const char* filename) {
		file = SD.open (filename);
	}

	~SDContent () {
		file.close ();
	}

	char SDContent::getNextByte () override {
		if (file.available ()) {
			return file.read ();
		} else {
			return '\0';
		}
	}
};

#endif


#endif

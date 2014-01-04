#include "num2words.h"
#include "string.h"

static const char* const HOURS[] = {
	"minuit",
	"une",
	"deux",
	"trois",
	"quatre",
	"cinq",
	"six",
	"sept",
	"huit",
	"neuf",
	"dix",
	"onze",
	"midi"
};

static const char * STR_HEURES = "heures";
static const char * STR_HEURE = "heure";

static const char * STR_0 = "pile";
static const char * STR_1 = "juste passé";
static const char * STR_5 = "cinq";
static const char * STR_10 = "dix";
static const char * STR_15 = "et quart";
static const char * STR_20 = "vingt";
static const char * STR_30 = "et demi";
static const char * STR_40 = "moins vingt";
static const char * STR_45 = "moins le quart";
static const char * STR_50 = "moins dix";
static const char * STR_55 = "moins cinq";
static const char * STR_59 = "moins cinq";

static size_t append_string(char* buffer, const size_t length, const char* str) {
	strncat(buffer, str, length);

	size_t written = strlen(str);
	return (length > written) ? written : length;
}

void fuzzy_time_to_words(int hours, int minutes, char* words, size_t length)
{
	int fuzzy_hours = hours;
	size_t remaining = length;

	memset(words, 0, length);

	/* Adjust hours */
	if (fuzzy_hours > 12)
	{
		fuzzy_hours = fuzzy_hours -12;
	}

	if (minutes > 34) /* Why 34 ? See the switch/case later */
	{
		fuzzy_hours += 1;
		if (fuzzy_hours == 13)
		{
			fuzzy_hours = 0;
		}
	}

	/* Hours in words */
	remaining -= append_string(words, remaining, HOURS[hours<=12?hours:hours-12]);
	
	remaining -= append_string(words, remaining, " ");

	if (fuzzy_hours != 12 && fuzzy_hours != 0)
	{
		remaining -= append_string(words, remaining, fuzzy_hours==1?STR_HEURE:STR_HEURES);
		remaining -= append_string(words, remaining, " ");
	}

	/* Minutes in words. Enumeration is faster, yay ! */
	switch(minutes)
	{
		case 0:
			remaining -= append_string(words, remaining, STR_0);
			break;
		
		case 1:
		case 2:
			remaining -= append_string(words, remaining, STR_1);
			break;
			
		case 3:
		case 4:
		case 5:
		case 6:
			remaining -= append_string(words, remaining, STR_5);
			break;
			
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			remaining -= append_string(words, remaining, STR_10);
			break;
			
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			remaining -= append_string(words, remaining, STR_15);
			break;

		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			remaining -= append_string(words, remaining, STR_20);
			break;

		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
			remaining -= append_string(words, remaining, STR_30);
			break;

		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
			remaining -= append_string(words, remaining, STR_40);
			break;

		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			remaining -= append_string(words, remaining, STR_45);
			break;
			
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
			remaining -= append_string(words, remaining, STR_50);
			break;

		case 53:
		case 54:
		case 55:
		case 56:
			remaining -= append_string(words, remaining, STR_55);
			break;

		case 57:
		case 58:
		case 59:
			remaining -= append_string(words, remaining, STR_59);
			break;
	}
}

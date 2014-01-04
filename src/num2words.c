#include "num2words.h"
#include "string.h"

static const char * const HEURES[] =
{
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

static const char * const JOURS[] =
{
	"Dimanche",
	"Lundi",
	"Mardi",
	"Mercredi",
	"Jeudi",
	"Vendredi",
	"Samedi"
};

static const char * const MOIS[] =
{
	"janvier",
	"février",
	"mars",
	"avril",
	"mai",
	"juin",
	"juillet",
	"août",
	"septembre",
	"octobre",
	"novembre",
	"décembre"
};

static const char * const ITOA[] =
{
	"",
	"1er",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"11",
	"12",
	"13",
	"14",
	"15",
	"16",
	"17",
	"18",
	"19",
	"20",
	"21",
	"22",
	"23",
	"24",
	"25",
	"26",
	"27",
	"28",
	"29",
	"30",
	"31"
};

static const char * STR_H_HEURES = " heures";
static const char * STR_H_HEURE = " heure";

static const char * STR_H_0 = " pile";
static const char * STR_H_1 = " juste passé";
static const char * STR_H_5 = " cinq";
static const char * STR_H_10 = " dix";
static const char * STR_H_15 = " et quart";
static const char * STR_H_20 = " vingt";
static const char * STR_H_30 = " et demi";
static const char * STR_H_40 = " moins vingt";
static const char * STR_H_45 = " moins le quart";
static const char * STR_H_50 = " moins dix";
static const char * STR_H_55 = " moins cinq";
static const char * STR_H_59 = " moins cinq";

/*
 *
 */
static size_t append_string(char* buffer, const size_t length, const char* str)
{
	strncat(buffer, str, length);

	size_t written = strlen(str);
	return (length > written) ? written : length;
}

/*
 *
 */
void fuzzy_date_to_words(int wday, int mday, int mon, char * words, size_t length)
{
	size_t remaining = length;

	memset(words, 0, length);

	remaining -= append_string(words, remaining, JOURS[wday]);
	remaining -= append_string(words, remaining, " ");
	remaining -= append_string(words, remaining, ITOA[mday]);
	remaining -= append_string(words, remaining, " ");
	remaining -= append_string(words, remaining, MOIS[mon]);
}

/*
 *
 */
void fuzzy_time_to_words(int hours, int minutes, char * words, size_t length)
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
	remaining -= append_string(words, remaining, HEURES[fuzzy_hours]);
	
	if (fuzzy_hours != 12 && fuzzy_hours != 0)
	{
		remaining -= append_string(words, remaining, fuzzy_hours==1?STR_H_HEURE:STR_H_HEURES);
	}

	/* Minutes in words. Enumeration is faster, yay ! */
	switch(minutes)
	{
		case 0:
			remaining -= append_string(words, remaining, STR_H_0);
			break;
		
		case 1:
		case 2:
			remaining -= append_string(words, remaining, STR_H_1);
			break;
			
		case 3:
		case 4:
		case 5:
		case 6:
			remaining -= append_string(words, remaining, STR_H_5);
			break;
			
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			remaining -= append_string(words, remaining, STR_H_10);
			break;
			
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			remaining -= append_string(words, remaining, STR_H_15);
			break;

		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			remaining -= append_string(words, remaining, STR_H_20);
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
			remaining -= append_string(words, remaining, STR_H_30);
			break;

		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
			remaining -= append_string(words, remaining, STR_H_40);
			break;

		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			remaining -= append_string(words, remaining, STR_H_45);
			break;
			
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
			remaining -= append_string(words, remaining, STR_H_50);
			break;

		case 53:
		case 54:
		case 55:
		case 56:
			remaining -= append_string(words, remaining, STR_H_55);
			break;

		case 57:
		case 58:
		case 59:
			remaining -= append_string(words, remaining, STR_H_59);
			break;
	}
}

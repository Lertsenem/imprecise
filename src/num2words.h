#pragma once

#include "string.h"

void fuzzy_date_to_words(int wday, int mday, int mon, char * words, size_t length);
void fuzzy_time_to_words(int hours, int minutes, char* words, size_t length);

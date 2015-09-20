#pragma once

namespace stopwords {

bool isEnglishStopword(const std::string &word);
bool isPunctuation(const std::string &word);
bool isNumeric(const std::string &word);


} // stopwords

#include "libunicode/UnicodeBlock.h"
#include <utf8.h>
#include <string>
#include <glog/logging.h>

using namespace std;

namespace relevanced {
namespace libunicode {

uint32_t normalizeBasicLatinCodePoint(uint32_t codePoint) {
  // A = 65
  // Z = 90
  // a - A = 32
  // z - Z = 32, etc
  if (codePoint >= 65 && codePoint <= 90) {
    codePoint += 32;
  }
  return codePoint;
}

uint32_t normalizeLatin1SupplementCodePoint(uint32_t codePoint) {
  if ((codePoint >= 192 && codePoint <= 214) || (codePoint >= 216 && codePoint <= 221)) {
    codePoint += 32;
  }
  return codePoint;
}

uint32_t normalizeLatinExtendedACodePoint(uint32_t codePoint) {
  // 383 is the small long S,
  // the only one in latin extended A without a capital version
  if (codePoint == 383) {
    return codePoint;
  }
  if (codePoint < 311) {
    // up to 311, latin A alternates between
    // upper and lower case versions with
    // uppers on even numbers.
    if (!(codePoint % 2)) {
      codePoint += 1;
    }
  }
  return codePoint;
}

uint32_t normalizeCodePoint(uint32_t codePoint, UnicodeBlock block) {
  switch (block) {
    case UnicodeBlock::BASIC_LATIN : return normalizeBasicLatinCodePoint(codePoint);
    case UnicodeBlock::LATIN_1_SUPPLEMENT : return normalizeLatin1SupplementCodePoint(codePoint);
    case UnicodeBlock::LATIN_EXTENDED_A : return normalizeLatinExtendedACodePoint(codePoint);
    default: return codePoint;
  }
}

uint32_t normalizeCodePoint(uint32_t codePoint) {
  auto block = getUnicodeBlock(codePoint);
  return normalizeCodePoint(codePoint, block);
}

bool isBasicLatinLetter(uint32_t cp) {
  // 0-31: misc whitespace chars, hex tablet things

  // 33-64: misc punc, digits, more misc punc
  // 33 = '!''
  // 54 = '6'
  // 64 = '@'

  if (cp <= 64) {
    return false;
  }
  if (cp <= 96 && cp > 90) {
    // lbracket, backslash, etc
    return false;
  }

  // 122 = 'z'
  if (cp > 122) {
    DCHECK(cp <= 127);
    return false;
  }

  // remaining characters are letters
  return true;
}

bool isLatin1SupplementLetter(uint32_t cp) {
  // LATIN_1_SUPPLEMENT starts with a bunch of punctuation.
  // 191 is upside-down question mark
  if (cp <= 191) {
    return false;
  }

  // 215 = multiplication symbol
  // 247 = division symbol
  if (cp == 215 || cp == 247) {
    return false;
  }

  return true;
}

bool isMathematicalAlphanumericSymbolLetter(uint32_t cp) {
  // 120782 is a 0 character. everything before that in
  // this block is a letter (sort of)
  return cp < 120782;
}

bool isLetterPoint(uint32_t cp, UnicodeBlock uBlock) {
  switch (uBlock) {
    case UnicodeBlock::BASIC_LATIN : return isBasicLatinLetter(cp);
    case UnicodeBlock::LATIN_1_SUPPLEMENT : return isLatin1SupplementLetter(cp);
    case UnicodeBlock::LATIN_EXTENDED_A : return true;
    case UnicodeBlock::LATIN_EXTENDED_B : return true;
    case UnicodeBlock::LATIN_EXTENDED_C : return true;
    case UnicodeBlock::LATIN_EXTENDED_D : return true;
    case UnicodeBlock::LATIN_EXTENDED_E : return true;
    case UnicodeBlock::LATIN_EXTENDED_ADDITIONAL : return true;
    case UnicodeBlock::GENERAL_PUNCTUATION : return false;

    case UnicodeBlock::ARROWS : return false;
    case UnicodeBlock::BLOCK_ELEMENTS : return false;
    case UnicodeBlock::BOX_DRAWING : return false;
    case UnicodeBlock::BRAILLE_PATTERNS : return false;
    case UnicodeBlock::COMBINING_DIACRITICAL_MARKS : return false;
    case UnicodeBlock::COMBINING_DIACRITICAL_MARKS_EXTENDED : return false;
    case UnicodeBlock::CONTROL_PICTURES : return false;
    case UnicodeBlock::DINGBATS : return false;
    case UnicodeBlock::DOMINO_TILES : return false;
    case UnicodeBlock::EMOTICONS : return false;
    case UnicodeBlock::ENCLOSED_ALPHANUMERICS : return false;
    case UnicodeBlock::GEOMETRIC_SHAPES : return false;
    case UnicodeBlock::IPA_EXTENSIONS : return false;
    case UnicodeBlock::MATHEMATICAL_OPERATORS : return false;
    case UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A : return false;
    case UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B : return false;
    case UnicodeBlock::MISCELLANEOUS_SYMBOLS : return false;
    case UnicodeBlock::MISCELLANEOUS_SYMBOLS_AND_ARROWS : return false;
    case UnicodeBlock::MISCELLANEOUS_TECHNICAL : return false;
    case UnicodeBlock::MUSICAL_SYMBOLS : return false;
    case UnicodeBlock::NUMBER_FORMS : return false;
    case UnicodeBlock::SPACING_MODIFIER_LETTERS : return false;
    case UnicodeBlock::SUPERSCRIPTS_AND_SUBSCRIPTS : return false;
    case UnicodeBlock::SUPPLEMENTAL_MATHEMATICAL_OPERATORS : return false;
    case UnicodeBlock::SUPPLEMENTAL_PUNCTUATION : return false;

    default: return true;
  }
}

bool isLetterPoint(uint32_t cp) {
  auto uBlock = getUnicodeBlock(cp);
  return isLetterPoint(cp, uBlock);
}

}
}

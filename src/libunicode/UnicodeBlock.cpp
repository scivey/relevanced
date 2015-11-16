#include "UnicodeBlock.h"


namespace relevanced {
namespace libunicode {

UnicodeBlock getUnicodeBlock(uint32_t codePoint) {
  auto cp = codePoint;
  if (cp < 592) {
    if (cp < 256) {
      if (cp <= 127) {
        return UnicodeBlock::BASIC_LATIN;
      } else {
        return UnicodeBlock::LATIN_1_SUPPLEMENT;
      }
    } else {
      if (cp <= 383) {
        return UnicodeBlock::LATIN_EXTENDED_A;
      } else {
        return UnicodeBlock::LATIN_EXTENDED_B;
      }
    }
  }

  if (cp < 43392) {
    if (cp < 8192) {
      if (cp < 4608) {
        if (cp < 2304) {
          if (cp < 1536) {
            if (cp < 1024) {
              if (cp < 768) {
                if (cp <= 687) {
                  return UnicodeBlock::IPA_EXTENSIONS;
                } else {
                  return UnicodeBlock::SPACING_MODIFIER_LETTERS;
                }
              } else {
                if (cp <= 879) {
                  return UnicodeBlock::COMBINING_DIACRITICAL_MARKS;
                } else {
                  return UnicodeBlock::GREEK_AND_COPTIC;
                }
              }
            } else {
              if (cp < 1328) {
                if (cp <= 1279) {
                  return UnicodeBlock::CYRILLIC;
                } else {
                  return UnicodeBlock::CYRILLIC_SUPPLEMENT;
                }
              } else {
                if (cp <= 1423) {
                  return UnicodeBlock::ARMENIAN;
                } else {
                  return UnicodeBlock::HEBREW;
                }
              }
            }
          } else {
            if (cp < 1984) {
              if (cp < 1872) {
                if (cp <= 1791) {
                  return UnicodeBlock::ARABIC;
                } else {
                  return UnicodeBlock::SYRIAC;
                }
              } else {
                if (cp <= 1919) {
                  return UnicodeBlock::ARABIC_SUPPLEMENT;
                } else {
                  return UnicodeBlock::THAANA;
                }
              }
            } else {
              if (cp < 2112) {
                if (cp <= 2047) {
                  return UnicodeBlock::NKO;
                } else {
                  return UnicodeBlock::SAMARITAN;
                }
              } else {
                if (cp <= 2143) {
                  return UnicodeBlock::MANDAIC;
                } else {
                  return UnicodeBlock::ARABIC_EXTENDED_A;
                }
              }
            }
          }
        } else {
          if (cp < 3328) {
            if (cp < 2816) {
              if (cp < 2560) {
                if (cp <= 2431) {
                  return UnicodeBlock::DEVANAGARI;
                } else {
                  return UnicodeBlock::BENGALI;
                }
              } else {
                if (cp <= 2687) {
                  return UnicodeBlock::GURMUKHI;
                } else {
                  return UnicodeBlock::GUJARATI;
                }
              }
            } else {
              if (cp < 3072) {
                if (cp <= 2943) {
                  return UnicodeBlock::ORIYA;
                } else {
                  return UnicodeBlock::TAMIL;
                }
              } else {
                if (cp <= 3199) {
                  return UnicodeBlock::TELUGU;
                } else {
                  return UnicodeBlock::KANNADA;
                }
              }
            }
          } else {
            if (cp < 3840) {
              if (cp < 3584) {
                if (cp <= 3455) {
                  return UnicodeBlock::MALAYALAM;
                } else {
                  return UnicodeBlock::SINHALA;
                }
              } else {
                if (cp <= 3711) {
                  return UnicodeBlock::THAI;
                } else {
                  return UnicodeBlock::LAO;
                }
              }
            } else {
              if (cp < 4256) {
                if (cp <= 4095) {
                  return UnicodeBlock::TIBETAN;
                } else {
                  return UnicodeBlock::MYANMAR;
                }
              } else {
                if (cp <= 4351) {
                  return UnicodeBlock::GEORGIAN;
                } else {
                  return UnicodeBlock::HANGUL_JAMO;
                }
              }
            }
          }
        }
      } else {
        if (cp < 6624) {
          if (cp < 5952) {
            if (cp < 5760) {
              if (cp < 5024) {
                if (cp <= 4991) {
                  return UnicodeBlock::ETHIOPIC;
                } else {
                  return UnicodeBlock::ETHIOPIC_SUPPLEMENT;
                }
              } else {
                if (cp <= 5119) {
                  return UnicodeBlock::CHEROKEE;
                } else {
                  return UnicodeBlock::UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS;
                }
              }
            } else {
              if (cp < 5888) {
                if (cp <= 5791) {
                  return UnicodeBlock::OGHAM;
                } else {
                  return UnicodeBlock::RUNIC;
                }
              } else {
                if (cp <= 5919) {
                  return UnicodeBlock::TAGALOG;
                } else {
                  return UnicodeBlock::HANUNOO;
                }
              }
            }
          } else {
            if (cp < 6320) {
              if (cp < 6016) {
                if (cp <= 5983) {
                  return UnicodeBlock::BUHID;
                } else {
                  return UnicodeBlock::TAGBANWA;
                }
              } else {
                if (cp <= 6143) {
                  return UnicodeBlock::KHMER;
                } else {
                  return UnicodeBlock::MONGOLIAN;
                }
              }
            } else {
              if (cp < 6480) {
                if (cp <= 6399) {
                  return UnicodeBlock::UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED;
                } else {
                  return UnicodeBlock::LIMBU;
                }
              } else {
                if (cp <= 6527) {
                  return UnicodeBlock::TAI_LE;
                } else {
                  return UnicodeBlock::NEW_TAI_LUE;
                }
              }
            }
          }
        } else {
          if (cp < 7248) {
            if (cp < 6912) {
              if (cp < 6688) {
                if (cp <= 6655) {
                  return UnicodeBlock::KHMER_SYMBOLS;
                } else {
                  return UnicodeBlock::BUGINESE;
                }
              } else {
                if (cp <= 6831) {
                  return UnicodeBlock::TAI_THAM;
                } else {
                  return UnicodeBlock::COMBINING_DIACRITICAL_MARKS_EXTENDED;
                }
              }
            } else {
              if (cp < 7104) {
                if (cp <= 7039) {
                  return UnicodeBlock::BALINESE;
                } else {
                  return UnicodeBlock::SUNDANESE;
                }
              } else {
                if (cp <= 7167) {
                  return UnicodeBlock::BATAK;
                } else {
                  return UnicodeBlock::LEPCHA;
                }
              }
            }
          } else {
            if (cp < 7552) {
              if (cp < 7376) {
                if (cp <= 7295) {
                  return UnicodeBlock::OL_CHIKI;
                } else {
                  return UnicodeBlock::SUNDANESE_SUPPLEMENT;
                }
              } else {
                if (cp <= 7423) {
                  return UnicodeBlock::VEDIC_EXTENSIONS;
                } else {
                  return UnicodeBlock::PHONETIC_EXTENSIONS;
                }
              }
            } else {
              if (cp < 7680) {
                if (cp <= 7615) {
                  return UnicodeBlock::PHONETIC_EXTENSIONS_SUPPLEMENT;
                } else {
                  return UnicodeBlock::COMBINING_DIACRITICAL_MARKS_SUPPLEMENT;
                }
              } else {
                if (cp <= 7935) {
                  return UnicodeBlock::LATIN_EXTENDED_ADDITIONAL;
                } else {
                  return UnicodeBlock::GREEK_EXTENDED;
                }
              }
            }
          }
        }
      }
    } else {
      if (cp < 11904) {
        if (cp < 9984) {
          if (cp < 8960) {
            if (cp < 8448) {
              if (cp < 8352) {
                if (cp <= 8303) {
                  return UnicodeBlock::GENERAL_PUNCTUATION;
                } else {
                  return UnicodeBlock::SUPERSCRIPTS_AND_SUBSCRIPTS;
                }
              } else {
                if (cp <= 8399) {
                  return UnicodeBlock::CURRENCY_SYMBOLS;
                } else {
                  return UnicodeBlock::COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS;
                }
              }
            } else {
              if (cp < 8592) {
                if (cp <= 8527) {
                  return UnicodeBlock::LETTERLIKE_SYMBOLS;
                } else {
                  return UnicodeBlock::NUMBER_FORMS;
                }
              } else {
                if (cp <= 8703) {
                  return UnicodeBlock::ARROWS;
                } else {
                  return UnicodeBlock::MATHEMATICAL_OPERATORS;
                }
              }
            }
          } else {
            if (cp < 9472) {
              if (cp < 9280) {
                if (cp <= 9215) {
                  return UnicodeBlock::MISCELLANEOUS_TECHNICAL;
                } else {
                  return UnicodeBlock::CONTROL_PICTURES;
                }
              } else {
                if (cp <= 9311) {
                  return UnicodeBlock::OPTICAL_CHARACTER_RECOGNITION;
                } else {
                  return UnicodeBlock::ENCLOSED_ALPHANUMERICS;
                }
              }
            } else {
              if (cp < 9632) {
                if (cp <= 9599) {
                  return UnicodeBlock::BOX_DRAWING;
                } else {
                  return UnicodeBlock::BLOCK_ELEMENTS;
                }
              } else {
                if (cp <= 9727) {
                  return UnicodeBlock::GEOMETRIC_SHAPES;
                } else {
                  return UnicodeBlock::MISCELLANEOUS_SYMBOLS;
                }
              }
            }
          }
        } else {
          if (cp < 11264) {
            if (cp < 10496) {
              if (cp < 10224) {
                if (cp <= 10175) {
                  return UnicodeBlock::DINGBATS;
                } else {
                  return UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A;
                }
              } else {
                if (cp <= 10239) {
                  return UnicodeBlock::SUPPLEMENTAL_ARROWS_A;
                } else {
                  return UnicodeBlock::BRAILLE_PATTERNS;
                }
              }
            } else {
              if (cp < 10752) {
                if (cp <= 10623) {
                  return UnicodeBlock::SUPPLEMENTAL_ARROWS_B;
                } else {
                  return UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B;
                }
              } else {
                if (cp <= 11007) {
                  return UnicodeBlock::SUPPLEMENTAL_MATHEMATICAL_OPERATORS;
                } else {
                  return UnicodeBlock::MISCELLANEOUS_SYMBOLS_AND_ARROWS;
                }
              }
            }
          } else {
            if (cp < 11568) {
              if (cp < 11392) {
                if (cp <= 11359) {
                  return UnicodeBlock::GLAGOLITIC;
                } else {
                  return UnicodeBlock::LATIN_EXTENDED_C;
                }
              } else {
                if (cp <= 11519) {
                  return UnicodeBlock::COPTIC;
                } else {
                  return UnicodeBlock::GEORGIAN_SUPPLEMENT;
                }
              }
            } else {
              if (cp < 11744) {
                if (cp <= 11647) {
                  return UnicodeBlock::TIFINAGH;
                } else {
                  return UnicodeBlock::ETHIOPIC_EXTENDED;
                }
              } else {
                if (cp <= 11775) {
                  return UnicodeBlock::CYRILLIC_EXTENDED_A;
                } else {
                  return UnicodeBlock::SUPPLEMENTAL_PUNCTUATION;
                }
              }
            }
          }
        }
      } else {
        if (cp < 19968) {
          if (cp < 12688) {
            if (cp < 12352) {
              if (cp < 12272) {
                if (cp <= 12031) {
                  return UnicodeBlock::CJK_RADICALS_SUPPLEMENT;
                } else {
                  return UnicodeBlock::KANGXI_RADICALS;
                }
              } else {
                if (cp <= 12287) {
                  return UnicodeBlock::IDEOGRAPHIC_DESCRIPTION_CHARACTERS;
                } else {
                  return UnicodeBlock::CJK_SYMBOLS_AND_PUNCTUATION;
                }
              }
            } else {
              if (cp < 12544) {
                if (cp <= 12447) {
                  return UnicodeBlock::HIRAGANA;
                } else {
                  return UnicodeBlock::KATAKANA;
                }
              } else {
                if (cp <= 12591) {
                  return UnicodeBlock::BOPOMOFO;
                } else {
                  return UnicodeBlock::HANGUL_COMPATIBILITY_JAMO;
                }
              }
            }
          } else {
            if (cp < 12800) {
              if (cp < 12736) {
                if (cp <= 12703) {
                  return UnicodeBlock::KANBUN;
                } else {
                  return UnicodeBlock::BOPOMOFO_EXTENDED;
                }
              } else {
                if (cp <= 12783) {
                  return UnicodeBlock::CJK_STROKES;
                } else {
                  return UnicodeBlock::KATAKANA_PHONETIC_EXTENSIONS;
                }
              }
            } else {
              if (cp < 13312) {
                if (cp <= 13055) {
                  return UnicodeBlock::ENCLOSED_CJK_LETTERS_AND_MONTHS;
                } else {
                  return UnicodeBlock::CJK_COMPATIBILITY;
                }
              } else {
                if (cp <= 19903) {
                  return UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A;
                } else {
                  return UnicodeBlock::YIJING_HEXAGRAM_SYMBOLS;
                }
              }
            }
          }
        } else {
          if (cp < 42784) {
            if (cp < 42240) {
              if (cp < 42128) {
                if (cp <= 40959) {
                  return UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS;
                } else {
                  return UnicodeBlock::YI_SYLLABLES;
                }
              } else {
                if (cp <= 42191) {
                  return UnicodeBlock::YI_RADICALS;
                } else {
                  return UnicodeBlock::LISU;
                }
              }
            } else {
              if (cp < 42656) {
                if (cp <= 42559) {
                  return UnicodeBlock::VAI;
                } else {
                  return UnicodeBlock::CYRILLIC_EXTENDED_B;
                }
              } else {
                if (cp <= 42751) {
                  return UnicodeBlock::BAMUM;
                } else {
                  return UnicodeBlock::MODIFIER_TONE_LETTERS;
                }
              }
            }
          } else {
            if (cp < 43136) {
              if (cp < 43056) {
                if (cp <= 43007) {
                  return UnicodeBlock::LATIN_EXTENDED_D;
                } else {
                  return UnicodeBlock::SYLOTI_NAGRI;
                }
              } else {
                if (cp <= 43071) {
                  return UnicodeBlock::COMMON_INDIC_NUMBER_FORMS;
                } else {
                  return UnicodeBlock::PHAGS_PA;
                }
              }
            } else {
              if (cp < 43264) {
                if (cp <= 43231) {
                  return UnicodeBlock::SAURASHTRA;
                } else {
                  return UnicodeBlock::DEVANAGARI_EXTENDED;
                }
              } else {
                if (cp <= 43311) {
                  return UnicodeBlock::KAYAH_LI;
                } else if (cp <= 43359) {
                  return UnicodeBlock::REJANG;
                } else {
                  return UnicodeBlock::HANGUL_JAMO_EXTENDED_A;
                }
              }
            }
          }
        }
      }
    }
  } else {
    if (cp < 68608) {
      if (cp < 66000) {
        if (cp < 63744) {
          if (cp < 43888) {
            if (cp < 43648) {
              if (cp < 43520) {
                if (cp <= 43487) {
                  return UnicodeBlock::JAVANESE;
                } else {
                  return UnicodeBlock::MYANMAR_EXTENDED_B;
                }
              } else {
                if (cp <= 43615) {
                  return UnicodeBlock::CHAM;
                } else {
                  return UnicodeBlock::MYANMAR_EXTENDED_A;
                }
              }
            } else {
              if (cp < 43776) {
                if (cp <= 43743) {
                  return UnicodeBlock::TAI_VIET;
                } else {
                  return UnicodeBlock::MEETEI_MAYEK_EXTENSIONS;
                }
              } else {
                if (cp <= 43823) {
                  return UnicodeBlock::ETHIOPIC_EXTENDED_A;
                } else {
                  return UnicodeBlock::LATIN_EXTENDED_E;
                }
              }
            }
          } else {
            if (cp < 55296) {
              if (cp < 44032) {
                if (cp <= 44031) {
                  return UnicodeBlock::CHEROKEE_SUPPLEMENT;
                } else {
                  return UnicodeBlock::MEETEI_MAYEK;
                }
              } else {
                if (cp <= 55215) {
                  return UnicodeBlock::HANGUL_SYLLABLES;
                } else {
                  return UnicodeBlock::HANGUL_JAMO_EXTENDED_B;
                }
              }
            } else {
              if (cp < 56320) {
                if (cp <= 56191) {
                  return UnicodeBlock::HIGH_SURROGATES;
                } else {
                  return UnicodeBlock::HIGH_PRIVATE_USE_SURROGATES;
                }
              } else {
                if (cp <= 57343) {
                  return UnicodeBlock::LOW_SURROGATES;
                } else {
                  return UnicodeBlock::PRIVATE_USE_AREA;
                }
              }
            }
          }
        } else {
          if (cp < 65136) {
            if (cp < 65040) {
              if (cp < 64336) {
                if (cp <= 64255) {
                  return UnicodeBlock::CJK_COMPATIBILITY_IDEOGRAPHS;
                } else {
                  return UnicodeBlock::ALPHABETIC_PRESENTATION_FORMS;
                }
              } else {
                if (cp <= 65023) {
                  return UnicodeBlock::ARABIC_PRESENTATION_FORMS_A;
                } else {
                  return UnicodeBlock::VARIATION_SELECTORS;
                }
              }
            } else {
              if (cp < 65072) {
                if (cp <= 65055) {
                  return UnicodeBlock::VERTICAL_FORMS;
                } else {
                  return UnicodeBlock::COMBINING_HALF_MARKS;
                }
              } else {
                if (cp <= 65103) {
                  return UnicodeBlock::CJK_COMPATIBILITY_FORMS;
                } else {
                  return UnicodeBlock::SMALL_FORM_VARIANTS;
                }
              }
            }
          } else {
            if (cp < 65664) {
              if (cp < 65520) {
                if (cp <= 65279) {
                  return UnicodeBlock::ARABIC_PRESENTATION_FORMS_B;
                } else {
                  return UnicodeBlock::HALFWIDTH_AND_FULLWIDTH_FORMS;
                }
              } else {
                if (cp <= 65535) {
                  return UnicodeBlock::SPECIALS;
                } else {
                  return UnicodeBlock::LINEAR_B_SYLLABARY;
                }
              }
            } else {
              if (cp < 65856) {
                if (cp <= 65791) {
                  return UnicodeBlock::LINEAR_B_IDEOGRAMS;
                } else {
                  return UnicodeBlock::AEGEAN_NUMBERS;
                }
              } else {
                if (cp <= 65935) {
                  return UnicodeBlock::ANCIENT_GREEK_NUMBERS;
                } else {
                  return UnicodeBlock::ANCIENT_SYMBOLS;
                }
              }
            }
          }
        }
      } else {
        if (cp < 67648) {
          if (cp < 66464) {
            if (cp < 66304) {
              if (cp < 66208) {
                if (cp <= 66047) {
                  return UnicodeBlock::PHAISTOS_DISC;
                } else {
                  return UnicodeBlock::LYCIAN;
                }
              } else {
                if (cp <= 66271) {
                  return UnicodeBlock::CARIAN;
                } else {
                  return UnicodeBlock::COPTIC_EPACT_NUMBERS;
                }
              }
            } else {
              if (cp < 66384) {
                if (cp <= 66351) {
                  return UnicodeBlock::OLD_ITALIC;
                } else {
                  return UnicodeBlock::GOTHIC;
                }
              } else {
                if (cp <= 66431) {
                  return UnicodeBlock::OLD_PERMIC;
                } else {
                  return UnicodeBlock::UGARITIC;
                }
              }
            }
          } else {
            if (cp < 66816) {
              if (cp < 66640) {
                if (cp <= 66527) {
                  return UnicodeBlock::OLD_PERSIAN;
                } else {
                  return UnicodeBlock::DESERET;
                }
              } else {
                if (cp <= 66687) {
                  return UnicodeBlock::SHAVIAN;
                } else {
                  return UnicodeBlock::OSMANYA;
                }
              }
            } else {
              if (cp < 67072) {
                if (cp <= 66863) {
                  return UnicodeBlock::ELBASAN;
                } else {
                  return UnicodeBlock::CAUCASIAN_ALBANIAN;
                }
              } else {
                if (cp <= 67455) {
                  return UnicodeBlock::LINEAR_A;
                } else {
                  return UnicodeBlock::CYPRIOT_SYLLABARY;
                }
              }
            }
          }
        } else {
          if (cp < 68096) {
            if (cp < 67840) {
              if (cp < 67712) {
                if (cp <= 67679) {
                  return UnicodeBlock::IMPERIAL_ARAMAIC;
                } else {
                  return UnicodeBlock::PALMYRENE;
                }
              } else {
                if (cp <= 67759) {
                  return UnicodeBlock::NABATAEAN;
                } else {
                  return UnicodeBlock::HATRAN;
                }
              }
            } else {
              if (cp < 67968) {
                if (cp <= 67871) {
                  return UnicodeBlock::PHOENICIAN;
                } else {
                  return UnicodeBlock::LYDIAN;
                }
              } else {
                if (cp <= 67999) {
                  return UnicodeBlock::MEROITIC_HIEROGLYPHS;
                } else {
                  return UnicodeBlock::MEROITIC_CURSIVE;
                }
              }
            }
          } else {
            if (cp < 68352) {
              if (cp < 68224) {
                if (cp <= 68191) {
                  return UnicodeBlock::KHAROSHTHI;
                } else {
                  return UnicodeBlock::OLD_SOUTH_ARABIAN;
                }
              } else {
                if (cp <= 68255) {
                  return UnicodeBlock::OLD_NORTH_ARABIAN;
                } else {
                  return UnicodeBlock::MANICHAEAN;
                }
              }
            } else {
              if (cp < 68448) {
                if (cp <= 68415) {
                  return UnicodeBlock::AVESTAN;
                } else {
                  return UnicodeBlock::INSCRIPTIONAL_PARTHIAN;
                }
              } else {
                if (cp <= 68479) {
                  return UnicodeBlock::INSCRIPTIONAL_PAHLAVI;
                } else {
                  return UnicodeBlock::PSALTER_PAHLAVI;
                }
              }
            }
          }
        }
      }
    } else {
      if (cp < 113664) {
        if (cp < 71168) {
          if (cp < 70016) {
            if (cp < 69760) {
              if (cp < 69216) {
                if (cp <= 68687) {
                  return UnicodeBlock::OLD_TURKIC;
                } else {
                  return UnicodeBlock::OLD_HUNGARIAN;
                }
              } else {
                if (cp <= 69247) {
                  return UnicodeBlock::RUMI_NUMERAL_SYMBOLS;
                } else {
                  return UnicodeBlock::BRAHMI;
                }
              }
            } else {
              if (cp < 69888) {
                if (cp <= 69839) {
                  return UnicodeBlock::KAITHI;
                } else {
                  return UnicodeBlock::SORA_SOMPENG;
                }
              } else {
                if (cp <= 69967) {
                  return UnicodeBlock::CHAKMA;
                } else {
                  return UnicodeBlock::MAHAJANI;
                }
              }
            }
          } else {
            if (cp < 70320) {
              if (cp < 70144) {
                if (cp <= 70111) {
                  return UnicodeBlock::SHARADA;
                } else {
                  return UnicodeBlock::SINHALA_ARCHAIC_NUMBERS;
                }
              } else {
                if (cp <= 70223) {
                  return UnicodeBlock::KHOJKI;
                } else {
                  return UnicodeBlock::MULTANI;
                }
              }
            } else {
              if (cp < 70784) {
                if (cp <= 70399) {
                  return UnicodeBlock::KHUDAWADI;
                } else {
                  return UnicodeBlock::GRANTHA;
                }
              } else {
                if (cp <= 70879) {
                  return UnicodeBlock::TIRHUTA;
                } else {
                  return UnicodeBlock::SIDDHAM;
                }
              }
            }
          }
        } else {
          if (cp < 77824) {
            if (cp < 72384) {
              if (cp < 71424) {
                if (cp <= 71263) {
                  return UnicodeBlock::MODI;
                } else {
                  return UnicodeBlock::TAKRI;
                }
              } else {
                if (cp <= 71487) {
                  return UnicodeBlock::AHOM;
                } else {
                  return UnicodeBlock::WARANG_CITI;
                }
              }
            } else {
              if (cp < 74752) {
                if (cp <= 72447) {
                  return UnicodeBlock::PAU_CIN_HAU;
                } else {
                  return UnicodeBlock::CUNEIFORM;
                }
              } else {
                if (cp <= 74879) {
                  return UnicodeBlock::CUNEIFORM_NUMBERS_AND_PUNCTUATION;
                } else {
                  return UnicodeBlock::EARLY_DYNASTIC_CUNEIFORM;
                }
              }
            }
          } else {
            if (cp < 92880) {
              if (cp < 92160) {
                if (cp <= 78895) {
                  return UnicodeBlock::EGYPTIAN_HIEROGLYPHS;
                } else {
                  return UnicodeBlock::ANATOLIAN_HIEROGLYPHS;
                }
              } else {
                if (cp <= 92735) {
                  return UnicodeBlock::BAMUM_SUPPLEMENT;
                } else {
                  return UnicodeBlock::MRO;
                }
              }
            } else {
              if (cp < 93952) {
                if (cp <= 92927) {
                  return UnicodeBlock::BASSA_VAH;
                } else {
                  return UnicodeBlock::PAHAWH_HMONG;
                }
              } else {
                if (cp <= 94111) {
                  return UnicodeBlock::MIAO;
                } else {
                  return UnicodeBlock::KANA_SUPPLEMENT;
                }
              }
            }
          }
        }
      } else {
        if (cp < 127744) {
          if (cp < 120832) {
            if (cp < 119296) {
              if (cp < 118784) {
                if (cp <= 113823) {
                  return UnicodeBlock::DUPLOYAN;
                } else {
                  return UnicodeBlock::SHORTHAND_FORMAT_CONTROLS;
                }
              } else {
                if (cp <= 119039) {
                  return UnicodeBlock::BYZANTINE_MUSICAL_SYMBOLS;
                } else {
                  return UnicodeBlock::MUSICAL_SYMBOLS;
                }
              }
            } else {
              if (cp < 119648) {
                if (cp <= 119375) {
                  return UnicodeBlock::ANCIENT_GREEK_MUSICAL_NOTATION;
                } else {
                  return UnicodeBlock::TAI_XUAN_JING_SYMBOLS;
                }
              } else {
                if (cp <= 119679) {
                  return UnicodeBlock::COUNTING_ROD_NUMERALS;
                } else {
                  return UnicodeBlock::MATHEMATICAL_ALPHANUMERIC_SYMBOLS;
                }
              }
            }
          } else {
            if (cp < 127024) {
              if (cp < 126464) {
                if (cp <= 121519) {
                  return UnicodeBlock::SUTTON_SIGNWRITING;
                } else {
                  return UnicodeBlock::MENDE_KIKAKUI;
                }
              } else {
                if (cp <= 126719) {
                  return UnicodeBlock::ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS;
                } else {
                  return UnicodeBlock::MAHJONG_TILES;
                }
              }
            } else {
              if (cp < 127232) {
                if (cp <= 127135) {
                  return UnicodeBlock::DOMINO_TILES;
                } else {
                  return UnicodeBlock::PLAYING_CARDS;
                }
              } else {
                if (cp <= 127487) {
                  return UnicodeBlock::ENCLOSED_ALPHANUMERIC_SUPPLEMENT;
                } else {
                  return UnicodeBlock::ENCLOSED_IDEOGRAPHIC_SUPPLEMENT;
                }
              }
            }
          }
        } else {
          if (cp < 131072) {
            if (cp < 128768) {
              if (cp < 128592) {
                if (cp <= 128511) {
                  return UnicodeBlock::MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS;
                } else {
                  return UnicodeBlock::EMOTICONS;
                }
              } else {
                if (cp <= 128639) {
                  return UnicodeBlock::ORNAMENTAL_DINGBATS;
                } else {
                  return UnicodeBlock::TRANSPORT_AND_MAP_SYMBOLS;
                }
              }
            } else {
              if (cp < 129024) {
                if (cp <= 128895) {
                  return UnicodeBlock::ALCHEMICAL_SYMBOLS;
                } else {
                  return UnicodeBlock::GEOMETRIC_SHAPES_EXTENDED;
                }
              } else {
                if (cp <= 129279) {
                  return UnicodeBlock::SUPPLEMENTAL_ARROWS_C;
                } else {
                  return UnicodeBlock::SUPPLEMENTAL_SYMBOLS_AND_PICTOGRAPHS;
                }
              }
            }
          } else {
            if (cp < 194560) {
              if (cp < 177984) {
                if (cp <= 173791) {
                  return UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B;
                } else {
                  return UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C;
                }
              } else {
                if (cp <= 178207) {
                  return UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D;
                } else {
                  return UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_E;
                }
              }
            } else {
              if (cp < 917760) {
                if (cp <= 195103) {
                  return UnicodeBlock::CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT;
                } else {
                  return UnicodeBlock::TAGS;
                }
              } else {
                if (cp <= 917999) {
                  return UnicodeBlock::VARIATION_SELECTORS_SUPPLEMENT;
                } else if (cp <= 1048575) {
                  return UnicodeBlock::SUPPLEMENTARY_PRIVATE_USE_AREA_A;
                } else {
                  return UnicodeBlock::SUPPLEMENTARY_PRIVATE_USE_AREA_B;
                }
              }
            }
          }
        }
      }
    }
  }
  return UnicodeBlock::UNKNOWN;
}


std::string stringOfUnicodeBlock(UnicodeBlock block) {
  switch(block) {
    case UnicodeBlock::AEGEAN_NUMBERS : return "AEGEAN_NUMBERS";
    case UnicodeBlock::AHOM : return "AHOM";
    case UnicodeBlock::ALCHEMICAL_SYMBOLS : return "ALCHEMICAL_SYMBOLS";
    case UnicodeBlock::ALPHABETIC_PRESENTATION_FORMS : return "ALPHABETIC_PRESENTATION_FORMS";
    case UnicodeBlock::ANATOLIAN_HIEROGLYPHS : return "ANATOLIAN_HIEROGLYPHS";
    case UnicodeBlock::ANCIENT_GREEK_MUSICAL_NOTATION : return "ANCIENT_GREEK_MUSICAL_NOTATION";
    case UnicodeBlock::ANCIENT_GREEK_NUMBERS : return "ANCIENT_GREEK_NUMBERS";
    case UnicodeBlock::ANCIENT_SYMBOLS : return "ANCIENT_SYMBOLS";
    case UnicodeBlock::ARABIC : return "ARABIC";
    case UnicodeBlock::ARABIC_EXTENDED_A : return "ARABIC_EXTENDED_A";
    case UnicodeBlock::ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS : return "ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS";
    case UnicodeBlock::ARABIC_PRESENTATION_FORMS_A : return "ARABIC_PRESENTATION_FORMS_A";
    case UnicodeBlock::ARABIC_PRESENTATION_FORMS_B : return "ARABIC_PRESENTATION_FORMS_B";
    case UnicodeBlock::ARABIC_SUPPLEMENT : return "ARABIC_SUPPLEMENT";
    case UnicodeBlock::ARMENIAN : return "ARMENIAN";
    case UnicodeBlock::ARROWS : return "ARROWS";
    case UnicodeBlock::AVESTAN : return "AVESTAN";
    case UnicodeBlock::BALINESE : return "BALINESE";
    case UnicodeBlock::BAMUM : return "BAMUM";
    case UnicodeBlock::BAMUM_SUPPLEMENT : return "BAMUM_SUPPLEMENT";
    case UnicodeBlock::BASIC_LATIN : return "BASIC_LATIN";
    case UnicodeBlock::BASSA_VAH : return "BASSA_VAH";
    case UnicodeBlock::BATAK : return "BATAK";
    case UnicodeBlock::BENGALI : return "BENGALI";
    case UnicodeBlock::BLOCK_ELEMENTS : return "BLOCK_ELEMENTS";
    case UnicodeBlock::BOPOMOFO : return "BOPOMOFO";
    case UnicodeBlock::BOPOMOFO_EXTENDED : return "BOPOMOFO_EXTENDED";
    case UnicodeBlock::BOX_DRAWING : return "BOX_DRAWING";
    case UnicodeBlock::BRAHMI : return "BRAHMI";
    case UnicodeBlock::BRAILLE_PATTERNS : return "BRAILLE_PATTERNS";
    case UnicodeBlock::BUGINESE : return "BUGINESE";
    case UnicodeBlock::BUHID : return "BUHID";
    case UnicodeBlock::BYZANTINE_MUSICAL_SYMBOLS : return "BYZANTINE_MUSICAL_SYMBOLS";
    case UnicodeBlock::CJK_COMPATIBILITY : return "CJK_COMPATIBILITY";
    case UnicodeBlock::CJK_COMPATIBILITY_FORMS : return "CJK_COMPATIBILITY_FORMS";
    case UnicodeBlock::CJK_COMPATIBILITY_IDEOGRAPHS : return "CJK_COMPATIBILITY_IDEOGRAPHS";
    case UnicodeBlock::CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT : return "CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT";
    case UnicodeBlock::CJK_RADICALS_SUPPLEMENT : return "CJK_RADICALS_SUPPLEMENT";
    case UnicodeBlock::CJK_STROKES : return "CJK_STROKES";
    case UnicodeBlock::CJK_SYMBOLS_AND_PUNCTUATION : return "CJK_SYMBOLS_AND_PUNCTUATION";
    case UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS : return "CJK_UNIFIED_IDEOGRAPHS";
    case UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A : return "CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A";
    case UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B : return "CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B";
    case UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C : return "CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C";
    case UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D : return "CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D";
    case UnicodeBlock::CJK_UNIFIED_IDEOGRAPHS_EXTENSION_E : return "CJK_UNIFIED_IDEOGRAPHS_EXTENSION_E";
    case UnicodeBlock::CARIAN : return "CARIAN";
    case UnicodeBlock::CAUCASIAN_ALBANIAN : return "CAUCASIAN_ALBANIAN";
    case UnicodeBlock::CHAKMA : return "CHAKMA";
    case UnicodeBlock::CHAM : return "CHAM";
    case UnicodeBlock::CHEROKEE : return "CHEROKEE";
    case UnicodeBlock::CHEROKEE_SUPPLEMENT : return "CHEROKEE_SUPPLEMENT";
    case UnicodeBlock::COMBINING_DIACRITICAL_MARKS : return "COMBINING_DIACRITICAL_MARKS";
    case UnicodeBlock::COMBINING_DIACRITICAL_MARKS_EXTENDED : return "COMBINING_DIACRITICAL_MARKS_EXTENDED";
    case UnicodeBlock::COMBINING_DIACRITICAL_MARKS_SUPPLEMENT : return "COMBINING_DIACRITICAL_MARKS_SUPPLEMENT";
    case UnicodeBlock::COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS : return "COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS";
    case UnicodeBlock::COMBINING_HALF_MARKS : return "COMBINING_HALF_MARKS";
    case UnicodeBlock::COMMON_INDIC_NUMBER_FORMS : return "COMMON_INDIC_NUMBER_FORMS";
    case UnicodeBlock::CONTROL_PICTURES : return "CONTROL_PICTURES";
    case UnicodeBlock::COPTIC : return "COPTIC";
    case UnicodeBlock::COPTIC_EPACT_NUMBERS : return "COPTIC_EPACT_NUMBERS";
    case UnicodeBlock::COUNTING_ROD_NUMERALS : return "COUNTING_ROD_NUMERALS";
    case UnicodeBlock::CUNEIFORM : return "CUNEIFORM";
    case UnicodeBlock::CUNEIFORM_NUMBERS_AND_PUNCTUATION : return "CUNEIFORM_NUMBERS_AND_PUNCTUATION";
    case UnicodeBlock::CURRENCY_SYMBOLS : return "CURRENCY_SYMBOLS";
    case UnicodeBlock::CYPRIOT_SYLLABARY : return "CYPRIOT_SYLLABARY";
    case UnicodeBlock::CYRILLIC : return "CYRILLIC";
    case UnicodeBlock::CYRILLIC_EXTENDED_A : return "CYRILLIC_EXTENDED_A";
    case UnicodeBlock::CYRILLIC_EXTENDED_B : return "CYRILLIC_EXTENDED_B";
    case UnicodeBlock::CYRILLIC_SUPPLEMENT : return "CYRILLIC_SUPPLEMENT";
    case UnicodeBlock::DESERET : return "DESERET";
    case UnicodeBlock::DEVANAGARI : return "DEVANAGARI";
    case UnicodeBlock::DEVANAGARI_EXTENDED : return "DEVANAGARI_EXTENDED";
    case UnicodeBlock::DINGBATS : return "DINGBATS";
    case UnicodeBlock::DOMINO_TILES : return "DOMINO_TILES";
    case UnicodeBlock::DUPLOYAN : return "DUPLOYAN";
    case UnicodeBlock::EARLY_DYNASTIC_CUNEIFORM : return "EARLY_DYNASTIC_CUNEIFORM";
    case UnicodeBlock::EGYPTIAN_HIEROGLYPHS : return "EGYPTIAN_HIEROGLYPHS";
    case UnicodeBlock::ELBASAN : return "ELBASAN";
    case UnicodeBlock::EMOTICONS : return "EMOTICONS";
    case UnicodeBlock::ENCLOSED_ALPHANUMERIC_SUPPLEMENT : return "ENCLOSED_ALPHANUMERIC_SUPPLEMENT";
    case UnicodeBlock::ENCLOSED_ALPHANUMERICS : return "ENCLOSED_ALPHANUMERICS";
    case UnicodeBlock::ENCLOSED_CJK_LETTERS_AND_MONTHS : return "ENCLOSED_CJK_LETTERS_AND_MONTHS";
    case UnicodeBlock::ENCLOSED_IDEOGRAPHIC_SUPPLEMENT : return "ENCLOSED_IDEOGRAPHIC_SUPPLEMENT";
    case UnicodeBlock::ETHIOPIC : return "ETHIOPIC";
    case UnicodeBlock::ETHIOPIC_EXTENDED : return "ETHIOPIC_EXTENDED";
    case UnicodeBlock::ETHIOPIC_EXTENDED_A : return "ETHIOPIC_EXTENDED_A";
    case UnicodeBlock::ETHIOPIC_SUPPLEMENT : return "ETHIOPIC_SUPPLEMENT";
    case UnicodeBlock::GENERAL_PUNCTUATION : return "GENERAL_PUNCTUATION";
    case UnicodeBlock::GEOMETRIC_SHAPES : return "GEOMETRIC_SHAPES";
    case UnicodeBlock::GEOMETRIC_SHAPES_EXTENDED : return "GEOMETRIC_SHAPES_EXTENDED";
    case UnicodeBlock::GEORGIAN : return "GEORGIAN";
    case UnicodeBlock::GEORGIAN_SUPPLEMENT : return "GEORGIAN_SUPPLEMENT";
    case UnicodeBlock::GLAGOLITIC : return "GLAGOLITIC";
    case UnicodeBlock::GOTHIC : return "GOTHIC";
    case UnicodeBlock::GRANTHA : return "GRANTHA";
    case UnicodeBlock::GREEK_EXTENDED : return "GREEK_EXTENDED";
    case UnicodeBlock::GREEK_AND_COPTIC : return "GREEK_AND_COPTIC";
    case UnicodeBlock::GUJARATI : return "GUJARATI";
    case UnicodeBlock::GURMUKHI : return "GURMUKHI";
    case UnicodeBlock::HALFWIDTH_AND_FULLWIDTH_FORMS : return "HALFWIDTH_AND_FULLWIDTH_FORMS";
    case UnicodeBlock::HANGUL_COMPATIBILITY_JAMO : return "HANGUL_COMPATIBILITY_JAMO";
    case UnicodeBlock::HANGUL_JAMO : return "HANGUL_JAMO";
    case UnicodeBlock::HANGUL_JAMO_EXTENDED_A : return "HANGUL_JAMO_EXTENDED_A";
    case UnicodeBlock::HANGUL_JAMO_EXTENDED_B : return "HANGUL_JAMO_EXTENDED_B";
    case UnicodeBlock::HANGUL_SYLLABLES : return "HANGUL_SYLLABLES";
    case UnicodeBlock::HANUNOO : return "HANUNOO";
    case UnicodeBlock::HATRAN : return "HATRAN";
    case UnicodeBlock::HEBREW : return "HEBREW";
    case UnicodeBlock::HIGH_PRIVATE_USE_SURROGATES : return "HIGH_PRIVATE_USE_SURROGATES";
    case UnicodeBlock::HIGH_SURROGATES : return "HIGH_SURROGATES";
    case UnicodeBlock::HIRAGANA : return "HIRAGANA";
    case UnicodeBlock::IPA_EXTENSIONS : return "IPA_EXTENSIONS";
    case UnicodeBlock::IDEOGRAPHIC_DESCRIPTION_CHARACTERS : return "IDEOGRAPHIC_DESCRIPTION_CHARACTERS";
    case UnicodeBlock::IMPERIAL_ARAMAIC : return "IMPERIAL_ARAMAIC";
    case UnicodeBlock::INSCRIPTIONAL_PAHLAVI : return "INSCRIPTIONAL_PAHLAVI";
    case UnicodeBlock::INSCRIPTIONAL_PARTHIAN : return "INSCRIPTIONAL_PARTHIAN";
    case UnicodeBlock::JAVANESE : return "JAVANESE";
    case UnicodeBlock::KAITHI : return "KAITHI";
    case UnicodeBlock::KANA_SUPPLEMENT : return "KANA_SUPPLEMENT";
    case UnicodeBlock::KANBUN : return "KANBUN";
    case UnicodeBlock::KANGXI_RADICALS : return "KANGXI_RADICALS";
    case UnicodeBlock::KANNADA : return "KANNADA";
    case UnicodeBlock::KATAKANA : return "KATAKANA";
    case UnicodeBlock::KATAKANA_PHONETIC_EXTENSIONS : return "KATAKANA_PHONETIC_EXTENSIONS";
    case UnicodeBlock::KAYAH_LI : return "KAYAH_LI";
    case UnicodeBlock::KHAROSHTHI : return "KHAROSHTHI";
    case UnicodeBlock::KHMER : return "KHMER";
    case UnicodeBlock::KHMER_SYMBOLS : return "KHMER_SYMBOLS";
    case UnicodeBlock::KHOJKI : return "KHOJKI";
    case UnicodeBlock::KHUDAWADI : return "KHUDAWADI";
    case UnicodeBlock::LAO : return "LAO";
    case UnicodeBlock::LATIN_EXTENDED_ADDITIONAL : return "LATIN_EXTENDED_ADDITIONAL";
    case UnicodeBlock::LATIN_EXTENDED_A : return "LATIN_EXTENDED_A";
    case UnicodeBlock::LATIN_EXTENDED_B : return "LATIN_EXTENDED_B";
    case UnicodeBlock::LATIN_EXTENDED_C : return "LATIN_EXTENDED_C";
    case UnicodeBlock::LATIN_EXTENDED_D : return "LATIN_EXTENDED_D";
    case UnicodeBlock::LATIN_EXTENDED_E : return "LATIN_EXTENDED_E";
    case UnicodeBlock::LATIN_1_SUPPLEMENT : return "LATIN_1_SUPPLEMENT";
    case UnicodeBlock::LEPCHA : return "LEPCHA";
    case UnicodeBlock::LETTERLIKE_SYMBOLS : return "LETTERLIKE_SYMBOLS";
    case UnicodeBlock::LIMBU : return "LIMBU";
    case UnicodeBlock::LINEAR_A : return "LINEAR_A";
    case UnicodeBlock::LINEAR_B_IDEOGRAMS : return "LINEAR_B_IDEOGRAMS";
    case UnicodeBlock::LINEAR_B_SYLLABARY : return "LINEAR_B_SYLLABARY";
    case UnicodeBlock::LISU : return "LISU";
    case UnicodeBlock::LOW_SURROGATES : return "LOW_SURROGATES";
    case UnicodeBlock::LYCIAN : return "LYCIAN";
    case UnicodeBlock::LYDIAN : return "LYDIAN";
    case UnicodeBlock::MAHAJANI : return "MAHAJANI";
    case UnicodeBlock::MAHJONG_TILES : return "MAHJONG_TILES";
    case UnicodeBlock::MALAYALAM : return "MALAYALAM";
    case UnicodeBlock::MANDAIC : return "MANDAIC";
    case UnicodeBlock::MANICHAEAN : return "MANICHAEAN";
    case UnicodeBlock::MATHEMATICAL_ALPHANUMERIC_SYMBOLS : return "MATHEMATICAL_ALPHANUMERIC_SYMBOLS";
    case UnicodeBlock::MATHEMATICAL_OPERATORS : return "MATHEMATICAL_OPERATORS";
    case UnicodeBlock::MEETEI_MAYEK : return "MEETEI_MAYEK";
    case UnicodeBlock::MEETEI_MAYEK_EXTENSIONS : return "MEETEI_MAYEK_EXTENSIONS";
    case UnicodeBlock::MENDE_KIKAKUI : return "MENDE_KIKAKUI";
    case UnicodeBlock::MEROITIC_CURSIVE : return "MEROITIC_CURSIVE";
    case UnicodeBlock::MEROITIC_HIEROGLYPHS : return "MEROITIC_HIEROGLYPHS";
    case UnicodeBlock::MIAO : return "MIAO";
    case UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A : return "MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A";
    case UnicodeBlock::MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B : return "MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B";
    case UnicodeBlock::MISCELLANEOUS_SYMBOLS : return "MISCELLANEOUS_SYMBOLS";
    case UnicodeBlock::MISCELLANEOUS_SYMBOLS_AND_ARROWS : return "MISCELLANEOUS_SYMBOLS_AND_ARROWS";
    case UnicodeBlock::MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS : return "MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS";
    case UnicodeBlock::MISCELLANEOUS_TECHNICAL : return "MISCELLANEOUS_TECHNICAL";
    case UnicodeBlock::MODI : return "MODI";
    case UnicodeBlock::MODIFIER_TONE_LETTERS : return "MODIFIER_TONE_LETTERS";
    case UnicodeBlock::MONGOLIAN : return "MONGOLIAN";
    case UnicodeBlock::MRO : return "MRO";
    case UnicodeBlock::MULTANI : return "MULTANI";
    case UnicodeBlock::MUSICAL_SYMBOLS : return "MUSICAL_SYMBOLS";
    case UnicodeBlock::MYANMAR : return "MYANMAR";
    case UnicodeBlock::MYANMAR_EXTENDED_A : return "MYANMAR_EXTENDED_A";
    case UnicodeBlock::MYANMAR_EXTENDED_B : return "MYANMAR_EXTENDED_B";
    case UnicodeBlock::NKO : return "NKO";
    case UnicodeBlock::NABATAEAN : return "NABATAEAN";
    case UnicodeBlock::NEW_TAI_LUE : return "NEW_TAI_LUE";
    case UnicodeBlock::NUMBER_FORMS : return "NUMBER_FORMS";
    case UnicodeBlock::OGHAM : return "OGHAM";
    case UnicodeBlock::OL_CHIKI : return "OL_CHIKI";
    case UnicodeBlock::OLD_HUNGARIAN : return "OLD_HUNGARIAN";
    case UnicodeBlock::OLD_ITALIC : return "OLD_ITALIC";
    case UnicodeBlock::OLD_NORTH_ARABIAN : return "OLD_NORTH_ARABIAN";
    case UnicodeBlock::OLD_PERMIC : return "OLD_PERMIC";
    case UnicodeBlock::OLD_PERSIAN : return "OLD_PERSIAN";
    case UnicodeBlock::OLD_SOUTH_ARABIAN : return "OLD_SOUTH_ARABIAN";
    case UnicodeBlock::OLD_TURKIC : return "OLD_TURKIC";
    case UnicodeBlock::OPTICAL_CHARACTER_RECOGNITION : return "OPTICAL_CHARACTER_RECOGNITION";
    case UnicodeBlock::ORIYA : return "ORIYA";
    case UnicodeBlock::ORNAMENTAL_DINGBATS : return "ORNAMENTAL_DINGBATS";
    case UnicodeBlock::OSMANYA : return "OSMANYA";
    case UnicodeBlock::PAHAWH_HMONG : return "PAHAWH_HMONG";
    case UnicodeBlock::PALMYRENE : return "PALMYRENE";
    case UnicodeBlock::PAU_CIN_HAU : return "PAU_CIN_HAU";
    case UnicodeBlock::PHAGS_PA : return "PHAGS_PA";
    case UnicodeBlock::PHAISTOS_DISC : return "PHAISTOS_DISC";
    case UnicodeBlock::PHOENICIAN : return "PHOENICIAN";
    case UnicodeBlock::PHONETIC_EXTENSIONS : return "PHONETIC_EXTENSIONS";
    case UnicodeBlock::PHONETIC_EXTENSIONS_SUPPLEMENT : return "PHONETIC_EXTENSIONS_SUPPLEMENT";
    case UnicodeBlock::PLAYING_CARDS : return "PLAYING_CARDS";
    case UnicodeBlock::PRIVATE_USE_AREA : return "PRIVATE_USE_AREA";
    case UnicodeBlock::PSALTER_PAHLAVI : return "PSALTER_PAHLAVI";
    case UnicodeBlock::REJANG : return "REJANG";
    case UnicodeBlock::RUMI_NUMERAL_SYMBOLS : return "RUMI_NUMERAL_SYMBOLS";
    case UnicodeBlock::RUNIC : return "RUNIC";
    case UnicodeBlock::SAMARITAN : return "SAMARITAN";
    case UnicodeBlock::SAURASHTRA : return "SAURASHTRA";
    case UnicodeBlock::SHARADA : return "SHARADA";
    case UnicodeBlock::SHAVIAN : return "SHAVIAN";
    case UnicodeBlock::SHORTHAND_FORMAT_CONTROLS : return "SHORTHAND_FORMAT_CONTROLS";
    case UnicodeBlock::SIDDHAM : return "SIDDHAM";
    case UnicodeBlock::SINHALA : return "SINHALA";
    case UnicodeBlock::SINHALA_ARCHAIC_NUMBERS : return "SINHALA_ARCHAIC_NUMBERS";
    case UnicodeBlock::SMALL_FORM_VARIANTS : return "SMALL_FORM_VARIANTS";
    case UnicodeBlock::SORA_SOMPENG : return "SORA_SOMPENG";
    case UnicodeBlock::SPACING_MODIFIER_LETTERS : return "SPACING_MODIFIER_LETTERS";
    case UnicodeBlock::SPECIALS : return "SPECIALS";
    case UnicodeBlock::SUNDANESE : return "SUNDANESE";
    case UnicodeBlock::SUNDANESE_SUPPLEMENT : return "SUNDANESE_SUPPLEMENT";
    case UnicodeBlock::SUPERSCRIPTS_AND_SUBSCRIPTS : return "SUPERSCRIPTS_AND_SUBSCRIPTS";
    case UnicodeBlock::SUPPLEMENTAL_ARROWS_A : return "SUPPLEMENTAL_ARROWS_A";
    case UnicodeBlock::SUPPLEMENTAL_ARROWS_B : return "SUPPLEMENTAL_ARROWS_B";
    case UnicodeBlock::SUPPLEMENTAL_ARROWS_C : return "SUPPLEMENTAL_ARROWS_C";
    case UnicodeBlock::SUPPLEMENTAL_MATHEMATICAL_OPERATORS : return "SUPPLEMENTAL_MATHEMATICAL_OPERATORS";
    case UnicodeBlock::SUPPLEMENTAL_PUNCTUATION : return "SUPPLEMENTAL_PUNCTUATION";
    case UnicodeBlock::SUPPLEMENTAL_SYMBOLS_AND_PICTOGRAPHS : return "SUPPLEMENTAL_SYMBOLS_AND_PICTOGRAPHS";
    case UnicodeBlock::SUPPLEMENTARY_PRIVATE_USE_AREA_A : return "SUPPLEMENTARY_PRIVATE_USE_AREA_A";
    case UnicodeBlock::SUPPLEMENTARY_PRIVATE_USE_AREA_B : return "SUPPLEMENTARY_PRIVATE_USE_AREA_B";
    case UnicodeBlock::SUTTON_SIGNWRITING : return "SUTTON_SIGNWRITING";
    case UnicodeBlock::SYLOTI_NAGRI : return "SYLOTI_NAGRI";
    case UnicodeBlock::SYRIAC : return "SYRIAC";
    case UnicodeBlock::TAGALOG : return "TAGALOG";
    case UnicodeBlock::TAGBANWA : return "TAGBANWA";
    case UnicodeBlock::TAGS : return "TAGS";
    case UnicodeBlock::TAI_LE : return "TAI_LE";
    case UnicodeBlock::TAI_THAM : return "TAI_THAM";
    case UnicodeBlock::TAI_VIET : return "TAI_VIET";
    case UnicodeBlock::TAI_XUAN_JING_SYMBOLS : return "TAI_XUAN_JING_SYMBOLS";
    case UnicodeBlock::TAKRI : return "TAKRI";
    case UnicodeBlock::TAMIL : return "TAMIL";
    case UnicodeBlock::TELUGU : return "TELUGU";
    case UnicodeBlock::THAANA : return "THAANA";
    case UnicodeBlock::THAI : return "THAI";
    case UnicodeBlock::TIBETAN : return "TIBETAN";
    case UnicodeBlock::TIFINAGH : return "TIFINAGH";
    case UnicodeBlock::TIRHUTA : return "TIRHUTA";
    case UnicodeBlock::TRANSPORT_AND_MAP_SYMBOLS : return "TRANSPORT_AND_MAP_SYMBOLS";
    case UnicodeBlock::UGARITIC : return "UGARITIC";
    case UnicodeBlock::UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS : return "UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS";
    case UnicodeBlock::UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED : return "UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED";
    case UnicodeBlock::VAI : return "VAI";
    case UnicodeBlock::VARIATION_SELECTORS : return "VARIATION_SELECTORS";
    case UnicodeBlock::VARIATION_SELECTORS_SUPPLEMENT : return "VARIATION_SELECTORS_SUPPLEMENT";
    case UnicodeBlock::VEDIC_EXTENSIONS : return "VEDIC_EXTENSIONS";
    case UnicodeBlock::VERTICAL_FORMS : return "VERTICAL_FORMS";
    case UnicodeBlock::WARANG_CITI : return "WARANG_CITI";
    case UnicodeBlock::YI_RADICALS : return "YI_RADICALS";
    case UnicodeBlock::YI_SYLLABLES : return "YI_SYLLABLES";
    case UnicodeBlock::YIJING_HEXAGRAM_SYMBOLS : return "YIJING_HEXAGRAM_SYMBOLS";
    case UnicodeBlock::UNKNOWN : return "UNKNOWN";
  }
  return "UNKNOWN";
}

} // text_util
} // relevanced

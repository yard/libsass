#include "prelexer.hpp"
#include "constants.hpp"
#include <cctype>

namespace Sass {
  namespace Prelexer {

    const char* newline(const char* src) {
      return
      alternatives<
        exactly<'\n'>,
        sequence< exactly<'\r'>, exactly<'\n'> >,
        exactly<'\r'>,
        exactly<'\f'>
      >(src);
    }

    const char* whitespace(const char* src) {
      return
      alternatives<
        newline,
        exactly<' '>,
        exactly<'\t'>
      >(src);
    }

    const char* ws(const char* src) {
      return zero_plus< whitespace >(src);
    }

    const char* letter(const char* src) {
      return std::isalpha(*src) ? src + 1 : 0;
    }

    const char* digit(const char* src) {
      return std::isdigit(*src) ? src + 1 : 0;
    }

    const char* hex_digit(const char* src) {
      return alternatives< digit, class_char<hex_letter_chars> >(src);
    }

    const char* escape(const char* src) {
      return
      sequence<
        exactly<'\\'>,
        alternatives<
          negate< alternatives< newline, hex_digit > >,
          sequence< between<1, 6, hex_digit>, optional<whitespace> >
        >
      >(src);
    }

    const char* non_ascii(const char* src) {
      return static_cast<unsigned char>(*src) > 127 ? src + 1 : 0;
    }

    const char* name_start(const char* src) {
      return
      alternatives<
        letter,
        exactly<'_'>,
        non_ascii
      >(src);
    }

    const char* name(const char* src) {
      return
      alternatives<
        name_start,
        digit,
        exactly<'-'>
      >(src);
    }

    const char* identifier(const char* src) {
      return
      sequence<
        optional< exactly<'-'> >,
        alternatives< name_start, escape >,
        zero_plus< alternatives< name, escape > >
      >(src);
    }

    const char* function(const char* src) {
      return sequence< identifier, exactly<'('> >(src);
    }

    const char* number(const char* src) {
      return
      sequence<
        // [+|-]
        optional< alternatives< exactly<'+'>, exactly<'-'> > >,
        // [[digits].]digits
        sequence<
          optional< sequence< zero_plus<digit>, exactly<'.'> > >,
          one_plus<digit>
        >,
        // [e|E[+|-]digits]
        optional<
          sequence<
            alternatives< exactly<'e'>, exactly<'E'> >,
            optional< alternatives< exactly<'+'>, exactly<'-'> > >,
            one_plus<digits>
          >
        >
      >(src);
    }

    const char* hash(const char* src) {
      return
      sequence<
        exactly<'#'>,
        one_plus< alternatives< name, escape > >
      >(src);
    }

    


  }
}

#include "lexer_combinators.hpp"
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

    const char* non_printable(const char* src) {
      const unsigned char c = static_cast<unsigned char>(*src);
      // Should we guard against 0x00 if we're assuming that that's the EOI?
      if (c <= 0x08 || c == 0x0b || (c >= 0x0e && c <= 0x1f) || c == 0x7f)
        return src + 1;
      else
        return 0;
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
      return std::isxdigit(*src) ? src + 1 : 0;
    }

    const char* escape(const char* src) {
      return
      sequence<
        exactly<'\\'>,
        alternatives<
          sequence< between<1, 6, hex_digit>, optional<whitespace> >,
          negate< newline >
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

    // check for special functions (e.g., `url`, `calc`) in the parser
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

    const char* dimension(const char* src) {
      return sequence< number, identifier >(src);
    }

    const char* percentage(const char* src) {
      return sequence< number, exactly<'%'> >(src);
    }

    const char* numeric(const char* src) {
      return
      sequence<
        number,
        optional< alternatives< identifier, exactly<'%'> > >
      >(src);
    }

    const char* at_keyword(const char* src) {
      return sequence< exactly<'@'>, identifier >(src);
    }

    const char* hash(const char* src) {
      return
      sequence<
        exactly<'#'>,
        one_plus< alternatives< name, escape > >
      >(src);
    }

    // may contain interpolants
    const char* string(const char* src) {
      return
      alternatives<

        sequence<
          exactly<'"'>,
          zero_plus<
            alternatives<
              any_char_except<
                alternatives<
                  exactly<'"'>,
                  exactly<'\\'>,
                  newline
                >
              >,
              sequence< exactly<'\\'>, newline >,
              escape
            >
          >
          exactly<'"'>
        >,

        sequence<
          exactly<'\''>,
          zero_plus<
            alternatives<
              any_char_except<
                alternatives<
                  exactly<'\''>,
                  exactly<'\\'>,
                  newline
                >
              >,
              sequence< exactly<'\\'>, newline >,
              escape
            >
          >
          exactly<'\''>
        >

      >(src);
    }

    // disallow interpolants
    const char* static_string(const char* src) {
      return
      alternatives<

        sequence<
          exactly<'"'>,
          zero_plus<
            alternatives<
              any_char_except<
                alternatives<
                  exactly<'"'>,
                  exactly<'\\'>,
                  newline,
                  interpolant_start
                >
              >,
              sequence< exactly<'\\'>, newline >,
              escape
            >
          >
          exactly<'"'>
        >,

        sequence<
          exactly<'\''>,
          zero_plus<
            alternatives<
              any_char_except<
                alternatives<
                  exactly<'\''>,
                  exactly<'\\'>,
                  newline,
                  interpolant_start
                >
              >,
              sequence< exactly<'\\'>, newline >,
              escape
            >
          >
          exactly<'\''>
        >

      >(src);
    }

    const char* unquoted_url(const char* src) {
      return
      one_plus<
        alternatives<
          any_char_except<
            alternatives<
              exactly<'"'>,
              exactly<'\''>,
              exactly<'('>,
              exactly<')'>,
              exactly<'\\'>,
              whitespace,
              non_printable
            >
          >,
          escape
        >
      >(src);
    }

    const char* unicode_range(const char* src) {
      // match the "U+"
      src =
      sequence<
        alternatives< exactly<'u'>, exactly<'U'> >,
        exactly<'+'>
      >(src);
      if (!src) return 0;
      // consume up to 6 hex digits
      size_t i;
      for (i = 0; i <6; ++i) {
        if (!hex_digit(src)) break;
        ++src;
      }
      // consume up to (6 - #digits) question marks
      size_t j;
      for(j = i; j < 6; ++j) {
        if (!exactly<'?'>(src)) break;
        ++src;
      }
      // if we didn't find any digits or question marks, fail
      if (i == 0 && j == 0) return 0;
      // if we matched any question marks, succeed
      if (j > 0) return src;
      // see if the range has a second half; if not, succed with what we have
      if (!sequence< exactly<'-'>, hex_digit >(src)) return src;
      // otherwise consume the hyphen and remaining hex digits
      return sequence< exactly<'-'>, between<1, 6, hex_digit> >(src);
    }

    extern const char include_match_sym[]   = "~=";
    extern const char dash_match_sym[]      = "|=";
    extern const char prefix_match_sym[]    = "^=";
    extern const char suffix_match_sym[]    = "$=";
    extern const char substring_match_sym[] = "*=";
    extern const char column_sym[]          = "||";
    extern const char CDO_sym[]             = "<!--";
    extern const char CDC_sym[]             = "-->";

    const char* include_match(const char* src) {
      return exactly<include_match_sym>(src);
    }

    const char* dash_match(const char* src) {
      return exactly<dash_match_sym>(src);
    }

    const char* prefix_match(const char* src) {
      return exactly<prefix_match_sym>(src);
    }

    const char* suffix_match(const char* src) {
      return exactly<suffix_match_sym>(src);
    }

    const char* substring_match(const char* src) {
      return exactly<substring_match_sym>(src);
    }

    const char* match_operator(const char* src) {
      return
      alternatives<
        include_match,
        dash_match,
        prefix_match,
        suffix_match,
        substring_match
      >(src);
    }

    const char* column(const char* src) {
      return exactly<column_sym>(src);
    }

    const char* CDO(const char* src) {
      return exactly<CDO_sym>(src);
    }

    const char* CDC(const char* src) {
      return exactly<CDC_sym>(src);
    }

    const char* hex_color(const char* src) {
      return
      sequence<
        exactly<'#'>,
        between<3, 3, hex_digit>,
        optional< between<3, 3, hex_digit> >,
        negate<hex_digit> // disallow extra digits
      >(src);
    }

    extern const char important_kwd[] = "important";

    const char* important(const char* src) {
      return sequence< exactly<'!'>, ws, exactly<important_kwd> >(src);
    }

    extern const char interpolant_start_sym[] = "#{";

    const char* interpolant_start(const char* src) {
      return exactly<interpolant_start_sym>(src);
    }

    const char* variable(const char* src) {
      return sequence< exactly<'$'>, identifier >();
    }

    const char* static_component(const char* src) {
      return
      alternatives<
        identifier,
        static_string,
        hex_color,
        numeric,
        important
      >(src);
    }

    extern const char statement_terminator_chars[] = ";}";

    const char* static_value(const char* src) {
      return
      sequence<
        static_component,
        zero_plus<
          sequence<
            ws,
            alternatives< exactly<','>, exactly<'/'> >,
            ws,
            static_component
          >
        >,
        ws,
        in<statement_terminator_chars>
      >(src);
    }

    extern const char block_comment_start_sym[] = "/*";
    extern const char block_comment_end_sym[]   = "*/";
    extern const char line_comment_start_sym[]  = "//";

    const char* block_comment(const char* src) {
      return
      sequence<
        exactly<comment_start_sym>,
        zero_plus< any_char_except<comment_end_sym> >,
        exactly<comment_end_sym>
      >(src);
    }

    const char* line_comment(const char* src) {
      return
      sequence<
        exactly<line_comment_start_sym>,
        zero_plus< any_char_except<newline> >,
        alternatives< eoi, newline >
      >(src);
    }


    // const char* static_selector(const char* src) {
    //   return
    //   sequence<
    //     between<0, 50, // limit the lookahead
    //       alternatives<
    //         name,
    //         escape,
    //         exactly<' '>,
    //         exactly<'\t'>,


    // }

  }
}

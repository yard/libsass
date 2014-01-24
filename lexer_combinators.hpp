namespace Sass {
  namespace Prelexer {

    typedef int (*ctype_predicate)(int);
    typedef const char* (*prelexer)(const char*);

    // Match a single character literal.
    template <char pre>
    const char* exactly(const char* src) {
      return *src == pre ? src + 1 : 0;
    }

    // Match a string constant.
    template <const char* prefix>
    const char* exactly(const char* src) {
      const char* pre = prefix;
      while (*pre && *src == *pre) ++src, ++pre;
      return *pre ? 0 : src;
    }

    // Match a single character that is a member of the supplied class.
    template <const char* char_class>
    const char* class_char(const char* src) {
      const char* cc = char_class;
      while (*cc && *src != *cc) ++cc;
      return *cc ? src + 1 : 0;
    }

    // Tries to match a certain number of times (between the supplied interval).
    template<size_t lo, size_t hi, prelexer mx>
    const char* between(const char* src) {
      for (size_t i = 0; i < lo; ++i) {
        src = mx(src);
        if (!src) return 0;
      }
      for (size_t i = lo; i <= hi; ++i) {
        const char* new_src = mx(src);
        if (!new_src) return src;
        src = new_src;
      }
      return src;
    }

    // TODO: use variadic templates when we upgrade to C++ 11. Also, see if
    // we can't use expression templates and macros to provide better syntax
    // without losing all the compile-time expansion.

    // Tries the matchers in sequence and returns the first match (or none)
    template <prelexer mx1, prelexer mx2>
    const char* alternatives(const char* src) {
      const char* rslt;
      (rslt = mx1(src)) || (rslt = mx2(src));
      return rslt;
    }

    // Same as above, but with 3 arguments.
    template <prelexer mx1, prelexer mx2, prelexer mx3>
    const char* alternatives(const char* src) {
      const char* rslt;
      (rslt = mx1(src)) || (rslt = mx2(src)) || (rslt = mx3(src));
      return rslt;
    }

    // Same as above, but with 4 arguments.
    template <prelexer mx1, prelexer mx2, prelexer mx3, prelexer mx4>
    const char* alternatives(const char* src) {
      const char* rslt;
      (rslt = mx1(src)) || (rslt = mx2(src)) || (rslt = mx3(src)) ||
      (rslt = mx4(src));
      return rslt;
    }

    // Same as above, but with 5 arguments.
    template <prelexer mx1, prelexer mx2, prelexer mx3,
              prelexer mx4, prelexer mx5>
    const char* alternatives(const char* src) {
      const char* rslt;
      (rslt = mx1(src)) || (rslt = mx2(src)) || (rslt = mx3(src)) ||
      (rslt = mx4(src)) || (rslt = mx5(src));
      return rslt;
    }

    // Same as above, but with 6 arguments.
    template <prelexer mx1, prelexer mx2, prelexer mx3,
              prelexer mx4, prelexer mx5, prelexer mx6>
    const char* alternatives(const char* src) {
      const char* rslt;
      (rslt = mx1(src)) || (rslt = mx2(src)) || (rslt = mx3(src)) ||
      (rslt = mx4(src)) || (rslt = mx5(src)) || (rslt = mx6(src));
      return rslt;
    }

    // Same as above, but with 7 arguments.
    template <prelexer mx1, prelexer mx2,
              prelexer mx3, prelexer mx4,
              prelexer mx5, prelexer mx6,
              prelexer mx7>
    const char* alternatives(const char* src) {
      const char* rslt = src;
      (rslt = mx1(rslt)) || (rslt = mx2(rslt)) || (rslt = mx3(rslt)) ||
      (rslt = mx4(rslt)) || (rslt = mx5(rslt)) || (rslt = mx6(rslt)) ||
      (rslt = mx7(rslt));
      return rslt;
    }

    // Same as above, but with 8 arguments.
    template <prelexer mx1, prelexer mx2,
              prelexer mx3, prelexer mx4,
              prelexer mx5, prelexer mx6,
              prelexer mx7, prelexer mx8>
    const char* alternatives(const char* src) {
      const char* rslt = src;
      (rslt = mx1(rslt)) || (rslt = mx2(rslt)) || (rslt = mx3(rslt)) ||
      (rslt = mx4(rslt)) || (rslt = mx5(rslt)) || (rslt = mx6(rslt)) ||
      (rslt = mx7(rslt)) || (rslt = mx8(rslt));
      return rslt;
    }

    // Tries the matchers in sequence and succeeds if they all succeed.
    template <prelexer mx1, prelexer mx2>
    const char* sequence(const char* src) {
      const char* rslt = src;
      (rslt = mx1(rslt)) && (rslt = mx2(rslt));
      return rslt;
    }

    // Same as above, but with 3 arguments.
    template <prelexer mx1, prelexer mx2, prelexer mx3>
    const char* sequence(const char* src) {
      const char* rslt = src;
      (rslt = mx1(rslt)) && (rslt = mx2(rslt)) && (rslt = mx3(rslt));
      return rslt;
    }

    // Same as above, but with 4 arguments.
    template <prelexer mx1, prelexer mx2, prelexer mx3, prelexer mx4>
    const char* sequence(const char* src) {
      const char* rslt = src;
      (rslt = mx1(rslt)) && (rslt = mx2(rslt)) && (rslt = mx3(rslt)) &&
      (rslt = mx4(rslt));
      return rslt;
    }

    // Same as above, but with 5 arguments.
    template <prelexer mx1, prelexer mx2,
              prelexer mx3, prelexer mx4,
              prelexer mx5>
    const char* sequence(const char* src) {
      const char* rslt = src;
      (rslt = mx1(rslt)) && (rslt = mx2(rslt)) && (rslt = mx3(rslt)) &&
      (rslt = mx4(rslt)) && (rslt = mx5(rslt));
      return rslt;
    }

    // Same as above, but with 6 arguments.
    template <prelexer mx1, prelexer mx2,
              prelexer mx3, prelexer mx4,
              prelexer mx5, prelexer mx6>
    const char* sequence(const char* src) {
      const char* rslt = src;
      (rslt = mx1(rslt)) && (rslt = mx2(rslt)) && (rslt = mx3(rslt)) &&
      (rslt = mx4(rslt)) && (rslt = mx5(rslt)) && (rslt = mx6(rslt));
      return rslt;
    }

    // Same as above, but with 7 arguments.
    template <prelexer mx1, prelexer mx2,
              prelexer mx3, prelexer mx4,
              prelexer mx5, prelexer mx6,
              prelexer mx7>
    const char* sequence(const char* src) {
      const char* rslt = src;
      (rslt = mx1(rslt)) && (rslt = mx2(rslt)) && (rslt = mx3(rslt)) &&
      (rslt = mx4(rslt)) && (rslt = mx5(rslt)) && (rslt = mx6(rslt)) &&
      (rslt = mx7(rslt));
      return rslt;
    }

    // Same as above, but with 8 arguments.
    template <prelexer mx1, prelexer mx2,
              prelexer mx3, prelexer mx4,
              prelexer mx5, prelexer mx6,
              prelexer mx7>
    const char* sequence(const char* src) {
      const char* rslt = src;
      (rslt = mx1(rslt)) && (rslt = mx2(rslt)) && (rslt = mx3(rslt)) &&
      (rslt = mx4(rslt)) && (rslt = mx5(rslt)) && (rslt = mx6(rslt)) &&
      (rslt = mx7(rslt)) && (rslt = mx8(rslt));
      return rslt;
    }


    // Match a pattern or not. Always succeeds.
    template <prelexer mx>
    const char* optional(const char* src) {
      const char* p = mx(src);
      return p ? p : src;
    }

    // Match zero or more of the supplied pattern
    template <prelexer mx>
    const char* zero_plus(const char* src) {
      const char* p = mx(src);
      while (p) src = p, p = mx(src);
      return src;
    }

    // Match one or more of the supplied pattern
    template <prelexer mx>
    const char* one_plus(const char* src) {
      const char* p = mx(src);
      if (!p) return 0;
      while (p) src = p, p = mx(src);
      return src;
    }

    // Succeeds of the supplied matcher fails, and vice versa. Consumes no
    // input.
    template <prelexer mx>
    const char* negate(const char* src) {
      return mx(src) ? 0 : src;
    }

    // Lookahead -- tries the pattern, but consumes no input.
    template <prelexer mx>
    const char* look(const char* src) {
      return mx(src) ? src : 0;
    }

    // Negative lookahead -- if the first pattern matches, then fail. Otherwise
    // match the second pattern.
    template <prelexer pre, prelexer mx>
    const char* without(const char* src) {
      return sequence< negate<pre>, mx >(src);
    }

    // Positive lookahead -- make sure the first pattern matches without
    // consuming any input, then match the second pattern.
    template <prelexer pre, prelexer mx>
    const char* with(const char* src) {
      return sequence< look<pre>, mx >(src);
    }

  }
}
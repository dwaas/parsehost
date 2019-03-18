#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <optional>
#include <string.h>

#include <iterator>

#include "named_type.hpp"

constexpr size_t kMaxHostnameLen = 255;
constexpr size_t kMinHostnameLen = 63;

//===================================================================
//namespace gsl{

template<typename T>
struct span {
        T* _begin = nullptr;
        T* _end = nullptr;

        constexpr T* begin() const { return _begin;};
        constexpr T* end() const { return _end;};
        constexpr bool empty() const { return _begin == _end; };
        constexpr size_t size() const { return _end - _begin; };
};

using str_view = span<char>;

using std::begin;
using std::end;
using std::empty;
using std::size;

//}
//===================================================================

using namespace fluent;
using Hostname = fluent::NamedType<const char*, struct HostnameParameter, Printable, Comparable>;

constexpr char kASCII_ZERO = 48;
constexpr char kASCII_NINE = 57;
constexpr char kASCII_A = 65;
constexpr char kASCII_Z = 90;
constexpr char kASCII_a = 97;
constexpr char kASCII_z = 122;

constexpr
bool IsNumeric(char c) {
        return kASCII_ZERO <= c && c <= kASCII_NINE;
}

constexpr
bool IsAlphabetic(char c) {
        return (kASCII_A <= c && c <= kASCII_Z)
        || (kASCII_a <= c && c <= kASCII_z);
}

constexpr
bool IsDelimiter(char c) {
        return c == '-';
}

constexpr
bool IsAlphanumeric(char c) {
        return IsNumeric(c) || IsAlphabetic(c);
}

constexpr
bool IsBeginning(char c) {
        if(IsDelimiter(c) ) return false;
        return IsAlphanumeric(c);
}

constexpr
bool IsValidChar(char c) {
        return IsAlphanumeric(c) || IsDelimiter(c);
}

/*detail Returns non-null delimited tokens */
constexpr
str_view Tokenize(const str_view str, char sep = ' ') {
//NOTE: don't call with empty cstring!
        for(char* c = begin(str); c != end(str); ++c) {
                if ( *c == sep ) { return {begin(str), c};}
                if ( *c == '\0') { return {begin(str), --c};}
        }
        return {};
}

constexpr
bool IsValidToken(const str_view token) {
        if(! size(token) < kMaxHostnameLen) {return false;}
        if(! IsBeginning( *begin(token)) ) {return false;}

        for(char c : token) {
                if(! IsValidChar(c) ) {return false;}
        }
        return true;
}

constexpr
bool IsHostnameNameValid(char *name) {
// 1 <= hostname.size <= 255 (no trailing dot)
// no initial hypen
// no final hypen
// tld is alphabetic only!
// a-z case insensitive; 0-9; -
// RFC 1123: https://tools.ietf.org/html/rfc1123
// https://tools.ietf.org/html/rfc1123#page-13

   const auto len = strlen(name);
   if(!(0 < len && len < kMaxHostnameLen)) { return false; }

   for(str_view token = Tokenize({name, name+len}, '.');
        empty(token); token = Tokenize(token, '.') ) {

        if(! IsValidToken(token) ) { return false; }
   }
   return true;
}

constexpr
std::optional<Hostname> makeHostname(char* hostname){
        if (IsHostnameNameValid(hostname)) {
                return Hostname{hostname};
        }
        return {};
}
//============================================================== TEST

TEST_CASE("compile time eval") {
        constexpr auto h = makeHostname("blabla.com");
        REQUIRE( *h  == Hostname{"blabla.com"} );
}
TEST_CASE("parent domain") {
        REQUIRE(*makeHostname("blu.blabla.com")  == Hostname{"blu.blabla.com"} );
}

TEST_CASE("empty input") {
        REQUIRE(*makeHostname("")  != Hostname{""} );
        REQUIRE(*makeHostname("\0")  != Hostname{"\0"} );
}

TEST_CASE("out of bounds input") {
//        REQUIRE(*makeHostname(char[200] = {0})  != Hostname{char[200] = {0}} );
}
TEST_CASE("invalid starting char") {
//        REQUIRE(*makeHostname("-blabla.com")  != Hostname{"-blabla.com"} );
}

TEST_CASE("invalid middle char") {
        REQUIRE(*makeHostname("bla*bla.com")  != Hostname{"bla*bla.com"} );
}

TEST_CASE("invalid ending char") {
//        REQUIRE(*makeHostname("blabla-.com")  != Hostname{"blabla-.com"} );
}

TEST_CASE("numbers only") {
//        REQUIRE(*makeHostname("123.com")  == Hostname{"123.com"} );
}

TEST_CASE("non alphabetic top level domain") {
//        REQUIRE(*makeHostname("blabla.c0m")  != Hostname{"blabla.c0m"} );
}

TEST_CASE("case insensitive"){
//        REQUIRE(*makeHostname("blabla.com")  != Hostname{"BLABLA.com"} );
}

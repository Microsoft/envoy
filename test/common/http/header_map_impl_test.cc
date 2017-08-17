#include <string>

#include "common/http/header_map_impl.h"

#include "test/test_common/printers.h"
#include "test/test_common/utility.h"

#include "gtest/gtest.h"

namespace Envoy {
namespace Http {

TEST(HeaderStringTest, All) {
  // Static LowerCaseString constructor
  {
    LowerCaseString static_string("hello");
    HeaderString string(static_string);
    EXPECT_STREQ("hello", string.c_str());
    EXPECT_EQ(static_string.get().c_str(), string.c_str());
    EXPECT_EQ(5U, string.size());
  }

  // Static std::string constructor
  {
    std::string static_string("HELLO");
    HeaderString string(static_string);
    EXPECT_STREQ("HELLO", string.c_str());
    EXPECT_EQ(static_string.c_str(), string.c_str());
    EXPECT_EQ(5U, string.size());
  }

  // Static move contructor
  {
    std::string static_string("HELLO");
    HeaderString string1(static_string);
    HeaderString string2(std::move(string1));
    EXPECT_STREQ("HELLO", string2.c_str());
    EXPECT_EQ(static_string.c_str(), string1.c_str());
    EXPECT_EQ(static_string.c_str(), string2.c_str());
    EXPECT_EQ(5U, string1.size());
    EXPECT_EQ(5U, string2.size());
  }

  // Inline move constructor
  {
    HeaderString string;
    string.setCopy("hello", 5);
    EXPECT_EQ(HeaderString::Type::Inline, string.type());
    HeaderString string2(std::move(string));
    EXPECT_TRUE(string.empty());
    EXPECT_EQ(HeaderString::Type::Inline, string.type());
    EXPECT_EQ(HeaderString::Type::Inline, string2.type());
    string.append("world", 5);
    EXPECT_STREQ("world", string.c_str());
    EXPECT_EQ(5UL, string.size());
    EXPECT_STREQ("hello", string2.c_str());
    EXPECT_EQ(5UL, string2.size());
  }

  // Dynamic move constructor
  {
    std::string large(4096, 'a');
    HeaderString string;
    string.setCopy(large.c_str(), large.size());
    EXPECT_EQ(HeaderString::Type::Dynamic, string.type());
    HeaderString string2(std::move(string));
    EXPECT_TRUE(string.empty());
    EXPECT_EQ(HeaderString::Type::Inline, string.type());
    EXPECT_EQ(HeaderString::Type::Dynamic, string2.type());
    string.append("b", 1);
    EXPECT_STREQ("b", string.c_str());
    EXPECT_EQ(1UL, string.size());
    EXPECT_STREQ(large.c_str(), string2.c_str());
    EXPECT_EQ(4096UL, string2.size());
  }

  // Static to inline number.
  {
    std::string static_string("HELLO");
    HeaderString string(static_string);
    string.setInteger(5);
    EXPECT_EQ(HeaderString::Type::Inline, string.type());
    EXPECT_STREQ("5", string.c_str());
  }

  // Static to inline string.
  {
    std::string static_string("HELLO");
    HeaderString string(static_string);
    string.setCopy(static_string.c_str(), static_string.size());
    EXPECT_EQ(HeaderString::Type::Inline, string.type());
    EXPECT_STREQ("HELLO", string.c_str());
  }

  // Static clear() does nothing.
  {
    std::string static_string("HELLO");
    HeaderString string(static_string);
    EXPECT_EQ(HeaderString::Type::Reference, string.type());
    string.clear();
    EXPECT_EQ(HeaderString::Type::Reference, string.type());
    EXPECT_STREQ("HELLO", string.c_str());
  }

  // Static to append.
  {
    std::string static_string("HELLO");
    HeaderString string(static_string);
    EXPECT_EQ(HeaderString::Type::Reference, string.type());
    string.append("a", 1);
    EXPECT_STREQ("a", string.c_str());
  }

  // Copy inline
  {
    HeaderString string;
    string.setCopy("hello", 5);
    EXPECT_STREQ("hello", string.c_str());
    EXPECT_EQ(5U, string.size());
  }

  // Copy dynamic
  {
    HeaderString string;
    std::string large_value(4096, 'a');
    string.setCopy(large_value.c_str(), large_value.size());
    EXPECT_STREQ(large_value.c_str(), string.c_str());
    EXPECT_NE(large_value.c_str(), string.c_str());
    EXPECT_EQ(4096U, string.size());
  }

  // Copy twice dynamic
  {
    HeaderString string;
    std::string large_value1(4096, 'a');
    string.setCopy(large_value1.c_str(), large_value1.size());
    std::string large_value2(2048, 'b');
    string.setCopy(large_value2.c_str(), large_value2.size());
    EXPECT_STREQ(large_value2.c_str(), string.c_str());
    EXPECT_NE(large_value2.c_str(), string.c_str());
    EXPECT_EQ(2048U, string.size());
  }

  // Copy twice dynamic with reallocate
  {
    HeaderString string;
    std::string large_value1(4096, 'a');
    string.setCopy(large_value1.c_str(), large_value1.size());
    std::string large_value2(16384, 'b');
    string.setCopy(large_value2.c_str(), large_value2.size());
    EXPECT_STREQ(large_value2.c_str(), string.c_str());
    EXPECT_NE(large_value2.c_str(), string.c_str());
    EXPECT_EQ(16384U, string.size());
  }

  // Copy twice inline to dynamic
  {
    HeaderString string;
    std::string large_value1(16, 'a');
    string.setCopy(large_value1.c_str(), large_value1.size());
    std::string large_value2(16384, 'b');
    string.setCopy(large_value2.c_str(), large_value2.size());
    EXPECT_STREQ(large_value2.c_str(), string.c_str());
    EXPECT_NE(large_value2.c_str(), string.c_str());
    EXPECT_EQ(16384U, string.size());
  }

  // Append, small buffer to dynamic
  {
    HeaderString string;
    std::string test(127, 'a');
    string.append(test.c_str(), test.size());
    EXPECT_EQ(HeaderString::Type::Inline, string.type());
    string.append("a", 1);
    EXPECT_EQ(HeaderString::Type::Dynamic, string.type());
    test += 'a';
    EXPECT_STREQ(test.c_str(), string.c_str());
  }

  // Append into inline twice, then shift to dynamic.
  {
    HeaderString string;
    string.append("hello", 5);
    EXPECT_STREQ("hello", string.c_str());
    EXPECT_EQ(5U, string.size());
    string.append("world", 5);
    EXPECT_STREQ("helloworld", string.c_str());
    EXPECT_EQ(10U, string.size());
    std::string large(4096, 'a');
    string.append(large.c_str(), large.size());
    large = "helloworld" + large;
    EXPECT_STREQ(large.c_str(), string.c_str());
    EXPECT_EQ(4106U, string.size());
  }

  // Append, realloc dynamic.
  {
    HeaderString string;
    std::string large(128, 'a');
    string.append(large.c_str(), large.size());
    EXPECT_EQ(HeaderString::Type::Dynamic, string.type());
    std::string large2 = large + large;
    string.append(large2.c_str(), large2.size());
    large += large2;
    EXPECT_STREQ(large.c_str(), string.c_str());
    EXPECT_EQ(384U, string.size());
  }

  // Append, realloc close to limit with small buffer.
  {
    HeaderString string;
    std::string large(128, 'a');
    string.append(large.c_str(), large.size());
    EXPECT_EQ(HeaderString::Type::Dynamic, string.type());
    std::string large2(120, 'b');
    string.append(large2.c_str(), large2.size());
    std::string large3(32, 'c');
    string.append(large3.c_str(), large3.size());
    EXPECT_STREQ((large + large2 + large3).c_str(), string.c_str());
    EXPECT_EQ(280U, string.size());
  }

  // Set integer, inline
  {
    HeaderString string;
    string.setInteger(123456789);
    EXPECT_STREQ("123456789", string.c_str());
    EXPECT_EQ(9U, string.size());
  }

  // Set integer, dynamic
  {
    HeaderString string;
    std::string large(128, 'a');
    string.append(large.c_str(), large.size());
    string.setInteger(123456789);
    EXPECT_STREQ("123456789", string.c_str());
    EXPECT_EQ(9U, string.size());
    EXPECT_EQ(HeaderString::Type::Dynamic, string.type());
  }

  // Set static, switch to dynamic, back to static.
  {
    const std::string static_string = "hello world";
    HeaderString string;
    string.setReference(static_string);
    EXPECT_EQ(string.c_str(), static_string.c_str());
    EXPECT_EQ(11U, string.size());
    EXPECT_EQ(HeaderString::Type::Reference, string.type());

    const std::string large(128, 'a');
    string.setCopy(large.c_str(), large.size());
    EXPECT_NE(string.c_str(), large.c_str());
    EXPECT_EQ(HeaderString::Type::Dynamic, string.type());

    string.setReference(static_string);
    EXPECT_EQ(string.c_str(), static_string.c_str());
    EXPECT_EQ(11U, string.size());
    EXPECT_EQ(HeaderString::Type::Reference, string.type());
  }
}

TEST(HeaderMapImplTest, InlineInsert) {
  HeaderMapImpl headers;
  EXPECT_EQ(nullptr, headers.Host());
  headers.insertHost().value(std::string("hello"));
  EXPECT_STREQ(":authority", headers.Host()->key().c_str());
  EXPECT_STREQ("hello", headers.Host()->value().c_str());
  EXPECT_STREQ("hello", headers.get(Headers::get().Host)->value().c_str());
}

TEST(HeaderMapImplTest, MoveIntoInline) {
  HeaderMapImpl headers;
  HeaderString key;
  key.setCopy(Headers::get().Host.get().c_str(), Headers::get().Host.get().size());
  HeaderString value;
  value.setCopy("hello", 5);
  headers.addViaMove(std::move(key), std::move(value));
  EXPECT_STREQ(":authority", headers.Host()->key().c_str());
  EXPECT_STREQ("hello", headers.Host()->value().c_str());
}

TEST(HeaderMapImplTest, Remove) {
  HeaderMapImpl headers;

  // Add random header and then remove by name.
  LowerCaseString static_key("hello");
  std::string ref_value("value");
  headers.addReference(static_key, ref_value);
  EXPECT_STREQ("value", headers.get(static_key)->value().c_str());
  EXPECT_EQ(HeaderString::Type::Reference, headers.get(static_key)->value().type());
  EXPECT_EQ(1UL, headers.size());
  headers.remove(static_key);
  EXPECT_EQ(nullptr, headers.get(static_key));
  EXPECT_EQ(0UL, headers.size());

  // Add and remove by inline.
  headers.insertContentLength().value(5);
  EXPECT_STREQ("5", headers.ContentLength()->value().c_str());
  EXPECT_EQ(1UL, headers.size());
  headers.removeContentLength();
  EXPECT_EQ(nullptr, headers.ContentLength());
  EXPECT_EQ(0UL, headers.size());

  // Add inline and remove by name.
  headers.insertContentLength().value(5);
  EXPECT_STREQ("5", headers.ContentLength()->value().c_str());
  EXPECT_EQ(1UL, headers.size());
  headers.remove(Headers::get().ContentLength);
  EXPECT_EQ(nullptr, headers.ContentLength());
  EXPECT_EQ(0UL, headers.size());
}

TEST(HeaderMapImplTest, DoubleInlineAdd) {
  HeaderMapImpl headers;
  headers.addReferenceKey(Headers::get().ContentLength, 5);
  headers.addReferenceKey(Headers::get().ContentLength, 6);
  EXPECT_STREQ("5", headers.ContentLength()->value().c_str());
  EXPECT_EQ(1UL, headers.size());
}

TEST(HeaderMapImplTest, AddCopy) {
  HeaderMapImpl headers;

  // Start with a string value.
  std::unique_ptr<LowerCaseString> lcKeyPtr(new LowerCaseString("hello"));
  headers.addCopy(*lcKeyPtr, "world");

  const HeaderString& value = headers.get(*lcKeyPtr)->value();

  EXPECT_STREQ("world", value.c_str());
  EXPECT_EQ(5UL, value.size());

  lcKeyPtr.reset();

  const HeaderString& value2 = headers.get(LowerCaseString("hello"))->value();

  EXPECT_STREQ("world", value2.c_str());
  EXPECT_EQ(5UL, value2.size());
  EXPECT_EQ(value.c_str(), value2.c_str());
  EXPECT_EQ(1UL, headers.size());

  // Repeat with an int value.
  //
  // addReferenceKey and addCopy can both add multiple instances of a
  // given header, so we need to delete the old "hello" header.
  headers.remove(LowerCaseString("hello"));

  // Build "hello" with string concatenation to make it unlikely that the
  // compiler is just reusing the same string constant for everything.
  lcKeyPtr.reset(new LowerCaseString(std::string("he") + "llo"));
  EXPECT_STREQ("hello", lcKeyPtr->get().c_str());

  headers.addCopy(*lcKeyPtr, 42);

  const HeaderString& value3 = headers.get(*lcKeyPtr)->value();

  EXPECT_STREQ("42", value3.c_str());
  EXPECT_EQ(2UL, value3.size());

  lcKeyPtr.reset();

  const HeaderString& value4 = headers.get(LowerCaseString("hello"))->value();

  EXPECT_STREQ("42", value4.c_str());
  EXPECT_EQ(2UL, value4.size());
  EXPECT_EQ(1UL, headers.size());

  // Here, again, we'll build yet another key string.
  LowerCaseString lcKey3(std::string("he") + "ll" + "o");
  EXPECT_STREQ("hello", lcKey3.get().c_str());

  EXPECT_STREQ("42", headers.get(lcKey3)->value().c_str());
  EXPECT_EQ(2UL, headers.get(lcKey3)->value().size());
}

TEST(HeaderMapImplTest, Equality) {
  TestHeaderMapImpl headers1;
  TestHeaderMapImpl headers2;
  EXPECT_EQ(headers1, headers2);

  headers1.addCopy("hello", "world");
  EXPECT_FALSE(headers1 == headers2);

  headers2.addCopy("foo", "bar");
  EXPECT_FALSE(headers1 == headers2);
}

TEST(HeaderMapImplTest, LargeCharInHeader) {
  HeaderMapImpl headers;
  LowerCaseString static_key("\x90hello");
  std::string ref_value("value");
  headers.addReference(static_key, ref_value);
  EXPECT_STREQ("value", headers.get(static_key)->value().c_str());
}

} // namespace Http
} // namespace Envoy
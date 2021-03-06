#include <iod/metajson/metajson.hh>
#include <cassert>

namespace s
{
  IOD_SYMBOL(test1)
  IOD_SYMBOL(test2)
}

int main()
{
  { // Simple deserializer.
    std::string input = R"json({"test1":12,"test2":"John"})json";

    auto obj = iod::make_metamap(s::_test1 = int(),
                                 s::_test2 = std::string());

    iod::json_decode(input, obj);
    assert(obj.test1 == 12);
    assert(obj.test2 == "John");
  }

  { // C-struct with getter and members
    std::string input = R"json({"test1":12,"test2":42})json";

    struct {
      int& test1() { return tmp; }
      int test2;
    private:
      int tmp;
    } a;

    auto err = iod::json_object(s::_test1, s::_test2).decode(input, a);
    assert(!err);
    assert(a.test1() == 12);
    assert(a.test2 == 42);
  }

  { // json key.
    std::string input = R"json({"test1":12,"name":"John"})json";

    auto obj = iod::make_metamap(s::_test1 = int(),
                                 s::_test2 = std::string());

    json_object(s::_test1, s::_test2(iod::json_key("name"))).decode(input, obj);

    assert(obj.test1 == 12);
    assert(obj.test2 == "John");
  }
  
  {
    // plain vectors.
    std::string input = R"json([1,2,3,4])json";
    
    std::vector<int> v;
    auto err = iod::json_decode(input, v);
    assert(!err);
    assert(v.size() == 4);
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 3);
    assert(v[3] == 4);
  }

  {
    // plain vectors.
    std::string input = R"json([{"test1":12}])json";

    struct A { int test1; };

    std::vector<A> v;
    auto err = iod::json_vector(s::_test1).decode(input, v);
    assert(!err);
    assert(v.size() == 1);
    assert(v[0].test1 == 12);
  }

  {
    // tuples.
    std::string input = R"json( [  42  ,  "foo" , 0 , 4 ] )json";
    
    std::tuple<int, std::string, bool, int> tu;
    auto err = iod::json_decode(input, tu);
    assert(!err);
    assert(std::get<0>(tu) == 42);
    assert(std::get<2>(tu) == 0);
    assert(std::get<3>(tu) == 4);
  }


  {
    // optional.
    auto obj = iod::make_metamap(s::_test1 = std::optional<std::string>());
    auto err = iod::json_decode("{}", obj);
    assert(err.good());
    assert(!obj.test1.has_value());
    err = iod::json_decode("{\"test1\": \"Hooh\"}", obj);
    assert(err.good());
    assert(obj.test1.has_value());
    assert(obj.test1.value() == "Hooh");
  }


  {
    // Variant.
    auto obj = iod::make_metamap(s::_test1 = std::variant<int, std::string>("abc"));

    assert(iod::json_decode(R"json({"test1":{"idx":1,"value":"abc"}})json", obj).good());
    assert(std::get<std::string>(obj.test1) == "abc");

    assert(iod::json_decode(R"json({"test1":{"idx":0,"value":42}})json", obj).good());
    assert(std::get<int>(obj.test1) == 42);
  }
  
}

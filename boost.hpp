
#ifndef pmt_boost_hpp
#define pmt_boost_hpp

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/array.hpp>
using boost::property_tree::ptree,
        boost::property_tree::write_json,
        boost::property_tree::read_json;

#endif //pmt_boost_hpp
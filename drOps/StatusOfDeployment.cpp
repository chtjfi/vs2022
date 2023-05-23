#include "stdafx.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "StatusOfDeployment.h"

using std::string;

namespace drOps {
StatusOfDeployment::StatusOfDeployment()
{
}

StatusOfDeployment::StatusOfDeployment(const std::string& json) : _json(json)
{
	char* properties[] = {"message", "status", "exception", "version"};
	int numberOfProperties = sizeof(properties) / sizeof(properties[0]);
	for (int i = 0; i < numberOfProperties; ++i) {
		string property = properties[i];
		boost::property_tree::ptree pt2;
		std::istringstream is (json);
		boost::property_tree::read_json (is, pt2);
		string value = pt2.get<std::string> (property); 
		_propertyMap[property] = value;
	}
}

StatusOfDeployment::~StatusOfDeployment()
{
}

}
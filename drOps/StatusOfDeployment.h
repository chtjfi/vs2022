#pragma once

#include <string>
#include <map>

namespace drOps {
class StatusOfDeployment
{
public:
	StatusOfDeployment();
	StatusOfDeployment(const std::string& json);
	~StatusOfDeployment();

	void setMessage(const std::string& s) { _propertyMap["message"] = s; }
	std::string getMessage(void) const { return _propertyMap.at("message"); }
	void setException(const std::string& s) { _propertyMap["exception"] = s; }
	std::string getException(void) const { return _propertyMap.at("exception"); }
	void setVersion(const std::string& s) { _propertyMap["version"] = s; }
	std::string getVersion(void) const { return _propertyMap.at("version"); }
	void setStatus(const std::string& s) { _propertyMap["status"] = s; }
	std::string getStatus(void) const { return _propertyMap.at("status"); }
	void getJSON(std::string& json) const { json.assign(_json); }
private:
	std::string _json;
	std::map<std::string, std::string> _propertyMap;
};

}
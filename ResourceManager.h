#pragma once
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>

template <typename Derived, typename T>
class ResourceManager {
public:
	ResourceManager(const std::string& l_path) { loadPaths(l_path); }
	~ResourceManager() { purgeResources(); }

	std::string getPath(const std::string& l_id) {
		auto path = m_paths.find(l_id);
		return(path != m_paths.end() ? path->second : "");
	}

	T* getResource(const std::string& l_id) {
		auto res = m_resources.find(l_id);
		return(res != m_resources.end() ? res->second.first : nullptr);
	}

	bool requireResource(const std::string& l_name) {
		auto itr = m_resources.find(l_name);
		if (itr != m_resources.end()) {
			++(itr->second.second);
			return true;
		}
		auto path = m_paths.find(l_name);
		if (path == m_paths.end()) { return false; }
		T* resource = load(path->second);
		if (!resource) { return false; }
		m_resources.emplace(l_name, std::make_pair(resource, 1));
		return true;
	}

	void purgeResources() {
		while (m_resources.begin() != m_resources.end()) {
			delete(m_resources.begin()->second.first);
			m_resources.erase(m_resources.begin());
		}
	}

	bool releaseResource(const std::string& l_name) {
		auto itr = m_resources.find(l_name);
		if (itr == m_resources.end()) { return false; }
		--(itr->second.second);
		if (itr->second.second == 0) { unload(l_name); }
		return true;
	}

protected:

	T* load(const std::string& l_path) {
		return static_cast<Derived*>(this)->load(l_path);
	}

private:
	void loadPaths(const std::string& l_path) {
		std::ifstream file;
		file.open(l_path);
		if (!file.is_open()) { std::cout << "Failed loading" << l_path << std::endl; return; }
		std::string buffer;
		while (std::getline(file, buffer)) {
			std::stringstream sstream(buffer);
			std::string resourceName;
			std::string resourcePath;
			sstream >> resourceName;
			sstream >> resourcePath;
			m_paths.emplace(resourceName, resourcePath);
		}
		file.close();
	}

	bool unload(const std::string& l_name) {
		auto itr = m_resources.find(l_name);
		if (itr == m_resources.end()) { return false; }
		delete(itr->second.first);
		m_resources.erase(itr);
	}

	std::unordered_map<std::string, std::pair<T*, unsigned int>> m_resources;
	std::unordered_map<std::string, std::string> m_paths;
};
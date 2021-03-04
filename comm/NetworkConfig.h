/*
 * NetworkConfig.h
 *
 *  Created on: Apr 26, 2016
 *      Author: ricardodias
 */

#ifndef COMM_NETWORKCONFIG_H_
#define COMM_NETWORKCONFIG_H_

#include <string>

class NetworkConfig {
public:
	NetworkConfig(const std::string& configFile);
	virtual ~NetworkConfig();

	char 			multicast_ip[16];
	char*			iface;
	unsigned int	port;
	int				frequency;

	int set_network_iface(const char * name);
	int get_network_config(char **network_name);
	void print_config();

private:
	std::string configFileName;
	std::string networkName;
	void reset();
};

#endif /* COMM_NETWORKCONFIG_H_ */

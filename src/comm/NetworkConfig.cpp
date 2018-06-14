/*
 * NetworkConfig.cpp
 *
 *  Created on: Apr 26, 2016
 *      Author: ricardodias
 */

#include "NetworkConfig.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PERR(txt, par...) \
	printf("ERROR: (%s / %s): " txt "\n", __FILE__, __FUNCTION__, ## par)

NetworkConfig::NetworkConfig(const std::string& configFile) {
	this->configFileName = configFile;
	this->iface = NULL;
	reset();
}

NetworkConfig::~NetworkConfig() {
	// TODO Auto-generated destructor stub
}

void NetworkConfig::reset() {
	memset(multicast_ip, 0, 16*sizeof(char));
	this->port = 0;
	this->frequency = 0;
}

int NetworkConfig::set_network_iface(const char * name) {
	this->iface = strdup(name);
	return 0;
}

int NetworkConfig::get_network_config(char **network_name) {
	/* if network_name == NULL, the default network will be returned */
	FILE * fp;
	void* rc;
	char tline[256];
	char str[256];
	char internal_network_name[256];
	char *ptr;
	int found = 0;
	int use_default = 0;
	fp = fopen(this->configFileName.c_str(),"r");

	if(fp == NULL)
	{
		PERR("Could not open %s",this->configFileName.c_str());
		return -1;
	}

	/* reset network_config */
	reset();

	/* if no network name is provided, use the default network */
	if(*network_name == NULL)
	{
		use_default = 1;
	}

	do{
		rc = fgets(tline, 200, fp);

		if(rc == 0)
			break;

		if(tline[0] == '#')
			continue;

		if(strstr(tline,"NETWORK") == tline)
		{
			/* check if the network was already found, then we can directly stop */
			if(found){
				break;
			}
			/* obtain name of the network */
			sscanf(tline,"%*s %s",internal_network_name);

			if(!use_default)
			{
				if(strcmp(*network_name,internal_network_name) == 0){
					/* use this network if the network_name is correct */
					found = 1;
				}
			}
		}

		if(found || use_default)
		{
			if((ptr = strstr(tline,"multicast_ip")) != NULL)
			{
				if( sscanf(ptr, "%*[^=]=%*[ ]%127[^;]", str) > 0){
					memcpy(this->multicast_ip,str,16*sizeof(char));
				}
			}
			if((ptr = strstr(tline,"multicast_port")) != NULL)
			{
				if( sscanf(ptr, "%*[^=]=%*[ ]%127[^;]", str) > 0){
					this->port = atoi(str);
				}
			}
			if((ptr = strstr(tline,"frequency")) != NULL)
			{
				if( sscanf(ptr, "%*[^=]=%*[ ]%127[^;]", str) > 0){
					this->frequency = atoi(str);
				}
			}
			if((ptr = strstr(tline,"default")) != NULL)
			{
				if( sscanf(ptr, "%*[^=]=%*[ ]%127[^;]", str) > 0){
					if(strncasecmp(str,"true", 4) == 0)
					{
						found = 1;
						*network_name = strdup(internal_network_name);
					}
				}
			}
		}
	}while(rc);

	fclose(fp);
	if(!found && use_default){
		PERR("Could not find 'default' network");
		return -1;
	}
	if(!found){
		PERR("Could not find correct network");
		return -1;
	}
	if(this->multicast_ip[0] == 0){
		PERR("No multicast_ip specified in network.conf");
		return -1;
	}
	if(this->port == 0){
		PERR("No port specified in network.conf");
		return -1;
	}
	if(this->frequency == 0){
		PERR("No frequency specified in network.conf");
		return -1;
	}

	networkName = *network_name;

	return 0;
}

void NetworkConfig::print_config() {
	fprintf(stderr, "Network      : %s\n", networkName.c_str());
	fprintf(stderr, "Multicast IP : %s\n", multicast_ip);
	fprintf(stderr, "Interface    : %s\n", iface);
	fprintf(stderr, "Port         : %d\n", port);
	fprintf(stderr, "Frequency    : %d\n", frequency);

}


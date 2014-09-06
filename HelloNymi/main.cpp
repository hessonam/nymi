#include "ncl.h"

#include <string>
#include <iostream>
#include <vector>

bool gNclInitialized=false;
int gHandle=-1;
int validatedUser=-1;
std::vector<NclProvision> gProvisions;

void callback(NclEvent event, void* userData){
	switch(event.type){
		case NCL_EVENT_INIT:
			if(event.init.success) gNclInitialized=true;
			else exit(-1);
			break;
		case NCL_EVENT_DISCOVERY:
			std::cout<<"log: Nymi discovered\n";
			std::cout<<"log: stopping scan\n";
			nclStopScan();
			gHandle=event.discovery.nymiHandle;
			std::cout<<"log: agreeing\n";
			nclAgree(event.discovery.nymiHandle);
			break;
		case NCL_EVENT_FIND:
			std::cout<<"log: Nymi found\n";
			std::cout<<"log: stopping scan\n";
			nclStopScan();
			gHandle=event.find.nymiHandle;
			std::cout<<"log: validating\n";
			nclValidate(event.find.nymiHandle);
			break;
		case NCL_EVENT_DISCONNECTION:
			std::cout<<"log: disconnected\n";
			gHandle=-1;
			break;
		case NCL_EVENT_AGREEMENT:
			gHandle=event.agreement.nymiHandle;
			std::cout<<"Is this:\n";
			for(unsigned i=0; i<NCL_AGREEMENT_PATTERNS; ++i){
				for(unsigned j=0; j<NCL_LEDS; ++j)
					std::cout<<event.agreement.leds[i][j];
				std::cout<<"\n";
			}
			std::cout<<"the correct LED pattern (agree/reject)?\n";
			break;
		case NCL_EVENT_PROVISION:
			gProvisions.push_back(event.provision.provision);
			std::cout<<"log: provisioned\n";
			break;
		case NCL_EVENT_VALIDATION:
			std::cout<<"Nymi validated! Now trusted user stuff can happen.\n";
			validatedUser=gHandle;
			break;
		default: break;
	}
}

int main(int argc, const char* argv[]){
	if(!nclInit(callback, NULL, "HelloNymi", NCL_MODE_DEV, stderr)) return -1;
	std::cout<<"Welcome to Hello Nymi!\n";
	std::cout<<"Enter \"provision\" if you want to start trusting a new Nymi.\n";
	std::cout<<"Enter \"validate\" if you want to find trusted Nymis and validate the first one found.\n";
	std::cout<<"Enter \"(un)lock\" to lock/unlock the screen.\n";
	std::cout<<"Enter \"quit\" to quit.\n";
	while(true){
		std::string input;
		std::cin>>input;
		if(!gNclInitialized){
			std::cout<<"error: NCL didn't finished initializing yet!\n";
			continue;
		}
		else if(input=="provision"){
			std::cout<<"log: starting discovery\n";
			nclStartDiscovery();
		}
		else if(input=="agree"){
			std::cout<<"log: provisioning\n";
			nclProvision(gHandle);
		}
		else if(input=="reject"){
			std::cout<<"log: disconnecting\n";
			nclDisconnect(gHandle);
		}
		else if(input=="validate"){
			std::cout<<"log: starting finding\n";
			nclStartFinding(gProvisions.data(), gProvisions.size(), NCL_FALSE);
		}
		else if(input=="lock"){
			if(gHandle!=-1 && gHandle==validatedUser){
				std::cout<<"log: locking\n";
				///AMER PLS
			}
			else{
				std::cout<<"log: not validated\n";
			}
		}
		else if(input=="unlock"){
			if(gHandle!=-1 && gHandle==validatedUser){
				std::cout<<"log: locking\n";
				///AMER PLS
			}
			else{
				std::cout<<"log: not validated\n";
			}
		}
		else if(input=="disconnect"){
			std::cout<<"log: disconnecting\n";
			nclDisconnect(gHandle);
		}
		else if(input=="quit") break;
	}
	if(gHandle!=-1) nclDisconnect(gHandle);
	nclFinish();
	return 0;
}


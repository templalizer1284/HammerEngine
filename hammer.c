#include "hammer.h"

int main(void){
	
	printf("Hammer Engine %d.%d Battlecruiser operational...\n", VERSION_MAJOR, VERSION_MINOR);
	
	if(h_MainLoop()) {
		perror("Main loop eror, aborting.\n");
		return 1;
	}
	
	return 0;
}

//#define DEBUG_SOCKET
#define DEBUG_IP "192.168.1.218"
#define DEBUG_PORT 19023

#include "ps4.h"

int libRegMgrHandle;

/* https://github.com/oct0xor/ps4_registry_editor/blob/master/Ps4EditLib/PsRegistry/Preferences.cs#L133 */
#define SECURITY_PARENTAL_PASSCODE (0x03800800)

int(*sceRegMgrGetInt)(int optionId, int* outInt);
int(*sceRegMgrSetInt)(int optionId, int inInt);
int(*sceRegMgrGetStr)(int optionId, char* outString, size_t stringLength);
int(*sceRegMgrSetStr)(int optionId, const char* inStirng, size_t stringLength);
int(*sceRegMgrGetBin)(int optionId, void* outData, size_t dataLength);
int(*sceRegMgrSetBin)(int optionId, const void* inData, size_t dataLength);

/* for some reason there are no RegMgr definitions in the Scene Collective payload SDK */
/* luckily, we can take the headers from the orbisdev repo and use typical RESOLVE stuff */
int initRegMgr() {
  if (libRegMgrHandle) {
    return 1;
  }
  
  libRegMgrHandle = sceKernelLoadStartModule("libSceRegMgr.sprx", 0, 0, 0, NULL, NULL);
  if (libRegMgrHandle < 0) {
    return libRegMgrHandle;
  }
  
  RESOLVE(libRegMgrHandle, sceRegMgrGetInt);
  RESOLVE(libRegMgrHandle, sceRegMgrSetInt);
  RESOLVE(libRegMgrHandle, sceRegMgrGetStr);
  RESOLVE(libRegMgrHandle, sceRegMgrSetStr);
  RESOLVE(libRegMgrHandle, sceRegMgrGetBin);
  RESOLVE(libRegMgrHandle, sceRegMgrSetBin);
  
  return 1;
}

int _main(struct thread *td) {
  char buff[32] = { '\0' };
  UNUSED(td);

  initKernel();
  initLibc();

#ifdef DEBUG_SOCKET
  initNetwork();
  DEBUG_SOCK = SckConnect(DEBUG_IP, DEBUG_PORT);
#endif

  jailbreak();

  initSysUtil();
  initModule();

  // the fun happens here:
  if (initRegMgr() < 0) {
    printf_notification("Failed to resolve RegMgr=%X", libRegMgrHandle);
  }
  else {
    sceRegMgrGetStr(SECURITY_PARENTAL_PASSCODE, buff, sizeof(buff) - 1);
	printf_notification("nya, passcode='%s'.", buff);
  }

#ifdef DEBUG_SOCKET
  printf_debug("Closing socket...\n");
  SckClose(DEBUG_SOCK);
#endif

  return 0;
}

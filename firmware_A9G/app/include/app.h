#ifndef __MAIN__
#include <stdint.h>
#include <api_inc_os.h>
#include <api_event.h>

void app(void *vp);
void waitSystemReady(void);
void EventDispatch(API_Event_t *pEvent);
HANDLE getOsTaskHandle(void);
HANDLE getMainTaskHandle(void);


#define MainTaskHandle getMainTaskHandle
#define OSTaskHandle getOsTaskHandle
#endif // !__MAIN__
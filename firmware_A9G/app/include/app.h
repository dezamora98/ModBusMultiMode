#ifndef __MAIN__
#include <stdint.h>
#include <api_event.h>

#define MAX_TASK_PR 1

void app(void *vp);
void EventDispatch(API_Event_t *pEvent);

#endif // !__MAIN__
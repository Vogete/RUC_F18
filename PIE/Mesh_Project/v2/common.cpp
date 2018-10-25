#include "common.h"
#include <string>


Common::Common(){
}

void Common::sendMessage(std::string message, uint32_t destination)
{
    // String msg = "Hello from node ";
    // msg += mesh.getNodeId();
    // taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));

    if (destination == 0)
    {
        // mesh.sendBroadcast(message);
    }
    else
    {
        // mesh.sendSingle(destination, message);
    }

}


// Common common = Common();

#ifndef _MESHCONTROL_H_
#define _MESHCONTROL_H_

#include <easyMesh.h>
#include <list>

class MeshControl
{
private:
    static std::list<uint32_t> meshNodes;
    static easyMesh mesh;

    static void receivedCallback(uint32_t from, String &msg);
    static void newConnectionCallback(bool adopt);
public:
    MeshControl();

    static void setupMesh();
    static void updateMesh();
    static void sendMeshMessage(String message, uint32_t destination);
    static String getNodesInMesh();

};



#endif // !_MESHCONTROL_H_

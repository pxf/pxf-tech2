#include <SampleModule.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <SOIL.h>
#include <angelscript.h>
#include <scriptbuilder.h>
#include <Box2D/Box2D.h>
#include <enet/enet.h>
#include <lua.h>
#include <RtAudio.h>
#include <s7.h>

#include <Pxf/Base/SharedLibrary.h>

int main()
{
    glfwInit();
    glewInit();
    asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    b2World box2d_body = b2World(b2Vec2(0, 0), false);
    ENetPacket *ae = enet_packet_create(NULL, 0, 0);
    const char* aoeu = LUA_VERSION;
    RtAudio* audio = new RtAudio(RtAudio::WINDOWS_DS);
    s7_scheme *s7;
    int bajs = SOIL_HDR_RGBE;
    return 0;
    //return lulz();
}
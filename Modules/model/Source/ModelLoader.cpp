#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/model/ModelLoader.h>

using namespace Pxf;
using namespace Modules;

GenericModelLoader::GenericModelLoader(Pxf::Kernel* _Kernel)
	: ResourceLoader(_Kernel, "Generic Model Loader")
{ }

Resource::Model* GenericModelLoader::Load(const char* _FilePath)
{
	return 0;
}

Resource::Model* GenericModelLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	return 0;
}

GenericModelLoader::~GenericModelLoader()
{

}
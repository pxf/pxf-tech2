#include "RenderBlocks.h"
#include "Renderer.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Resource/Image.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Json.h>

#include <string.h>

using namespace Derp;
using namespace Pxf;

static Json::Value CreateJson(const char* data)
{
	Kernel* k = Kernel::GetInstance();
	Resource::ResourceManager* res = k->GetResourceManager();
	Resource::JsonLoader* jsonloader = res->FindResourceLoader<Pxf::Resource::JsonLoader>("json");
	Resource::Json* doc = jsonloader->CreateFrom(data, StringLength(data));
	Json::Value root;
	if (doc)
	{
		root = doc->GetRoot();
	} else {
    Message("JSON", "Taak a daaamp?");
	}
	jsonloader->Destroy(doc);
	return root;
}

//////////////////////////////////////////////////////////////////////////
// Auxiliary block
//

AuxiliaryBlock::~AuxiliaryBlock()
{
	if (L)
		lua_close(L);
}

bool AuxiliaryBlock::Initialize(Json::Value *node)
{
	
	// First set blockname
	m_BlockName = (*node)["blockName"].asCString();
	
	// Take care of what type of aux block this is, and set AuxData
	if ((*node)["blockData"]["auxType"].asString() == "texture")
	{
		m_AuxType = AUXILIARY_TEXTURE;
		m_AuxData = (*node)["blockData"]["filepath"].asCString();
	}
	else if ((*node)["blockData"]["auxType"].asString() == "script")
	{
		m_AuxType = AUXILIARY_SCRIPT;
		m_AuxData = (*node)["blockData"]["src"].asCString();
		
		// Setup lua
		if (!L)
		{
			L = luaL_newstate();
			luaL_openlibs(L);
		}
	}
	else if ((*node)["blockData"]["auxType"].asString() == "model")
	{
		m_AuxType = AUXILIARY_MODEL;
		m_AuxData = (*node)["blockData"]["filepath"].asCString();
		
	} else {
		Message("AuxiliaryBlock", "Unknown auxiliary block type: %s", (*node)["blockData"]["auxType"].asCString());
		return false;
	}
	
	// Add outputs
	for(int i = 0; i < (*node)["blockOutput"].size(); i++)
	{
		m_OutputTypes.insert( std::make_pair((*node)["blockOutput"][i]["name"].asCString(), (*node)["blockOutput"][i]["type"].asCString()) );
	}
	
	
	return true;
}

void AuxiliaryBlock::BuildGraph()
{
	// I have no inputs just do nothing!
	//Message("AUXBLOCK", "My name is: %s", m_BlockName);
	//exit(1);
	if (!m_HasBeenBuilt)
	{
		for (Util::Map<Util::String, Util::String>::iterator iter = m_OutputTypes.begin(); iter != m_OutputTypes.end(); ++iter)
		{
			Util::String inputtype = (*iter).second;
			
			if (inputtype == "texture")
			{
				char filepath[256] = {0};
				Format(filepath, "datacache/%s", m_AuxData);
				Graphics::Texture* toutputtex = m_gfx->CreateTexture(filepath);
				m_Outputs.insert( std::make_pair((*iter).first, (void*)toutputtex) );
				
			} else if (inputtype == "float")
			{
				float* toutresult = new float(0.0f);
				m_Outputs.insert( std::make_pair((*iter).first, (void*)toutresult) );
			} else if (inputtype == "int")
			{
				int* toutresult = new int(0.0f);
				m_Outputs.insert( std::make_pair((*iter).first, (void*)toutresult) );
			} else if (inputtype == "vec2")
			{
				Math::Vec2f* toutresult = new Math::Vec2f(0.0f);
				m_Outputs.insert( std::make_pair((*iter).first, (void*)toutresult) );
				
			} else if (inputtype == "vec3")
			{
				Math::Vec3f* toutresult = new Math::Vec3f(0.0f);
				m_Outputs.insert( std::make_pair((*iter).first, (void*)toutresult) );

			} else if (inputtype == "geometry")
			{
				if (m_AuxType == AUXILIARY_MODEL)
				{
					char filepath[256] = {0};
					Format(filepath, "datacache/%s", m_AuxData);
					Graphics::Model* model = m_gfx->CreateModel(filepath);
					m_Outputs.insert( std::make_pair((*iter).first, (void*)model) );
				} else {
					Message("AuxiliaryBlock",  "Incompatable auxiliary type and output type.");
				}
				
			}
			
		}
		m_HasBeenBuilt = true;
	}
}

bool AuxiliaryBlock::Execute(bool _SendPreviews)
{
	if (!m_IsPerformed)
	{
		if (m_AuxType == AUXILIARY_SCRIPT)
		{
			int s = luaL_dostring(L, m_AuxData);
			if (s) {
				Message("AuxiliaryBlock::Execute", "Error while running supplied script: %s", lua_tostring(L, -1));
			} else {
			
				// Take care of result
				for (Util::Map<Util::String, Util::String>::iterator iter = m_OutputTypes.begin(); iter != m_OutputTypes.end(); ++iter)
				{
					Util::String inputtype = (*iter).second;
				
					if (inputtype == "float")
					{
						float* val = (float*)m_Outputs[(*iter).first];
						*val = lua_tonumber(L, -1);
					
					} else if (inputtype == "int")
					{
						int* val = (int*)m_Outputs[(*iter).first];
						*val = lua_tointeger(L, -1);
					
					} else if (inputtype == "vec2")
					{
						Math::Vec2f* val = (Math::Vec2f*)m_Outputs[(*iter).first];
						(*val).x = lua_tonumber(L, -2);
						(*val).y = lua_tonumber(L, -1);

					} else if (inputtype == "vec3")
					{
						Math::Vec3f* val = (Math::Vec3f*)m_Outputs[(*iter).first];
						(*val).x = lua_tonumber(L, -3);
						(*val).y = lua_tonumber(L, -2);
						(*val).z = lua_tonumber(L, -1);

					} else {
						Message("AuxiliaryBlock::Execute", "Don't know what to do!");
					}
				}
			
			}
		}
	}
	
	return Block::Execute(_SendPreviews);
}

//////////////////////////////////////////////////////////////////////////
// Render/geometry block
//

RenderBlock::~RenderBlock()
{
	if (m_Shader->IsValid())
		m_gfx->DestroyShader(m_Shader);
	if (m_DepthBuffer)
		delete m_DepthBuffer;
}

bool RenderBlock::Initialize(Json::Value *node)
{
	// First set blockname
	m_BlockName = (*node)["blockName"].asCString();

	// Setup texture properties
	m_Width = (*node)["blockData"]["width"].asInt();
	m_Height = (*node)["blockData"]["height"].asInt();

	// Store shader code
	m_VertShader = (*node)["blockData"]["shaderVert"].asCString();
	m_FragShader = (*node)["blockData"]["shaderFrag"].asCString();

	// Store our camera input (we need it in BuildGraph)
	m_CameraPosInputName = (*node)["blockData"]["cameraPosition"].asString();
	m_CameraLookInputName = (*node)["blockData"]["cameraLookAt"].asString();
	m_CameraFov = (float)(*node)["blockData"]["cameraFov"].asDouble();

	m_DrawMode = (*node)["blockData"]["drawMode"].asInt();

	// Add inputs
	for(int i = 0; i < (*node)["blockInput"].size(); i++)
	{
		m_Inputs.insert( m_Inputs.end(), OutputStruct((*node)["blockInput"][i]["block"].asCString(), (*node)["blockInput"][i]["output"].asCString()) );
	}

	// Add outputs
	for(int i = 0; i < (*node)["blockOutput"].size(); i++)
	{
		m_OutputTypes.insert( std::make_pair((*node)["blockOutput"][i]["name"].asCString(), (*node)["blockOutput"][i]["type"].asCString()) );
	}

	return true;
}


void RenderBlock::BuildGraph()
{
	if (!m_HasBeenBuilt)
	{
		
		// Build childs in trees and setup input pointers
		for (Util::Array<OutputStruct>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
		{
			Block *child = m_Renderer->m_Blocks[(*iter).block_name.c_str()];
			child->BuildGraph();
			m_InputBlocks.insert( std::make_pair((*iter).block_name.c_str(), child) );
			
			// Is this our camera position input?
			if (m_CameraPosInputName == (*iter).block_output)
			{
				m_CameraPosInput = child;
			} else if (m_CameraLookInputName == (*iter).block_output)
			{
				m_CameraLookInput = child;
			}
		}
		
		// Setup output textures
		for (Util::Map<Util::String, Util::String>::iterator iter = m_OutputTypes.begin(); iter != m_OutputTypes.end(); ++iter)
		{
			Graphics::Texture* toutputtex = m_gfx->CreateEmptyTexture(m_Width, m_Height);
			m_Outputs.insert( std::make_pair((*iter).first, (void*)toutputtex) );
		}
		
		// Setup internal block stuff
		m_Shader = m_gfx->CreateShader(m_BlockName, m_VertShader, m_FragShader);
		
		if (!m_Shader)
			Message("blah", "Failed to compile shader");
		
		//m_Cam.SetPerspective(m_CameraFov, (float)m_Width / (float)m_Height, 1.0f, 10000.0f);
		m_DepthBuffer = m_gfx->CreateRenderBuffer(GL_DEPTH_COMPONENT, m_Width, m_Height);
		
		m_HasBeenBuilt = true;
	}
}

void RenderBlock::ResetPerformed()
{
	// Reset inputs
	for (Util::Map<Util::String, Block*>::iterator iter = m_InputBlocks.begin(); iter != m_InputBlocks.end(); ++iter)
	{
		(*iter).second->ResetPerformed();
	}
	
	// Reset myself
	Block::ResetPerformed();
}

bool RenderBlock::Execute(bool _SendPreviews)
{
	if (!m_IsPerformed) {
		// Execute prereqs
		for (Util::Map<Util::String, Block*>::iterator iter = m_InputBlocks.begin(); iter != m_InputBlocks.end(); ++iter)
		{
			(*iter).second->Execute(_SendPreviews);
		}
		
		m_ProfileTimer.Start();
	
		// Setup OGL context etc
		m_gfx->SetViewport(0, 0, m_Width, m_Height);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(m_CameraFov, (float)m_Width / (float)m_Height, 1.0f, 10000.0f);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		Math::Vec3f campos, camlook;
		
		// Setup camera
		if (m_CameraPosInput && m_CameraPosInput->GetOutputType(m_CameraPosInputName) == "vec3")
		{
			Math::Vec3f* _campos = (Math::Vec3f*)m_CameraPosInput->GetOutput(m_CameraPosInputName);
			campos = Math::Vec3f(_campos->x, _campos->y, _campos->z);
			//m_Cam.SetPosition(_campos->x, _campos->y, _campos->z);
		} else {
			
			// Wrong output type of camera position
			Message("RenderBlock::Execute", "Wrong type of camera position input.");
			campos = Math::Vec3f(0.0f, 0.0f, 0.0f);
			//m_Cam.SetPosition(0.0f, 0.0f, 0.0f);
		}
		
		if (m_CameraLookInput && m_CameraLookInput->GetOutputType(m_CameraLookInputName) == "vec3")
		{
			Math::Vec3f* _camlook = (Math::Vec3f*)m_CameraLookInput->GetOutput(m_CameraLookInputName);
			//m_Cam.SetLookAt(_camlook->x, _camlook->y, _camlook->z);
			camlook = Math::Vec3f(_camlook->x, _camlook->y, _camlook->z);
		} else {
			
			// Wrong output type of camera lookat
			Message("RenderBlock::Execute", "Wrong type of camera look at input.");
			camlook = Math::Vec3f(0.0f, 0.0f, 0.0f);
			//m_Cam.SetLookAt(0.0f, 0.0f, 0.0f);
		}
		
		gluLookAt(campos.x, campos.y, campos.z, camlook.x, camlook.y, camlook.z, 0.0f, -1.0f, 0.0f);
		
		//m_gfx->SetProjection(m_Cam.GetProjectionView());
		//m_gfx->SetModelView(m_Cam.GetModelView());
	
		// Attach and bind all render texture outputs to FBO
		int attach_lut[] = {GL_COLOR_ATTACHMENT0_EXT,
			                  GL_COLOR_ATTACHMENT1_EXT,
			                  GL_COLOR_ATTACHMENT2_EXT,
			                  GL_COLOR_ATTACHMENT3_EXT};
		int num_attach = 0;
		for (Util::Map<Util::String, void*>::iterator iter = m_Outputs.begin(); iter != m_Outputs.end(); ++iter)
		{
			m_Renderer->m_FBO->Attach((Graphics::Texture*)((*iter).second), attach_lut[num_attach], false);
			num_attach += 1;
		}
		
		// Bind depth buffer
		m_Renderer->m_FBO->Attach(m_DepthBuffer, GL_DEPTH_ATTACHMENT_EXT);
		
		// Bind framebuffer
		m_gfx->BindFrameBufferObject(m_Renderer->m_FBO);
		
		// Enable depth
		glEnable(GL_DEPTH_TEST);
		
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		// Bind shaders
		if (m_Shader->IsValid())
			m_gfx->BindShader(m_Shader);
	
		// Gather and bind all our inputs
		int ttexunit = 0;
		for (Util::Array<OutputStruct>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
		{
			// Get pointer to input block
			Block* inputblock = m_InputBlocks[(*iter).block_name.c_str()];
		
			// Get texture pointer
			Pxf::Util::String inputtype = inputblock->GetOutputType((*iter).block_output);
			if (inputtype == "texture")
			{
				// Input is a texture, bind and set uniform
				Graphics::Texture* inputtex = (Graphics::Texture*)inputblock->GetOutput((*iter).block_output);
				if (inputtex == 0) {
					return false;
				}
		
				m_gfx->BindTexture(inputtex, ttexunit);
				if (m_Shader->IsValid())
					m_gfx->SetUniformi(m_Shader, (*iter).block_output.c_str(), ttexunit);
				ttexunit += 1;
			
			} else if (inputtype == "float")
			{
				// Input is a script with float result
				float* scriptres = (float*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformf(m_Shader, (*iter).block_output.c_str(), *scriptres);
			
			} else if (inputtype == "int")
			{
				// Input is a script with int result
				int* scriptres = (int*)inputblock->GetOutput((*iter).block_output);
				if (m_Shader->IsValid())
					m_gfx->SetUniformi(m_Shader, (*iter).block_output.c_str(), *scriptres);

			} else if (inputtype == "vec2")
			{
				// Input is a script with vec2 result
				Math::Vec2f* scriptres = (Math::Vec2f*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformVec2(m_Shader, (*iter).block_output.c_str(), scriptres);

			} else if (inputtype == "vec3")
			{
				// Input is a script with vec3 result
				Math::Vec3f* scriptres = (Math::Vec3f*)inputblock->GetOutput((*iter).block_output);
				if (m_Shader->IsValid())
					m_gfx->SetUniformVec3(m_Shader, (*iter).block_output.c_str(), scriptres);

			}
			
		}
	
		// Render only inputs that deliver geometry
		for (Util::Array<OutputStruct>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
		{
			// Get pointer to input block
			Block* inputblock = m_InputBlocks[(*iter).block_name.c_str()];
			
			Pxf::Util::String inputtype = inputblock->GetOutputType((*iter).block_output);
			if (inputtype == "geometry")
			{
				Graphics::Model* _Mdl = ((Graphics::Model*)inputblock->GetOutput((*iter).block_output));
				if (m_DrawMode == 1)
				{
					Graphics::VertexBuffer* _VB = _Mdl->GetVertexBuffer();
					m_OldPrimitiveType = _VB->SetPrimitive(Pxf::Graphics::VB_PRIMITIVE_LINES);
					
					Pxf::Graphics::Texture* _Tex = m_gfx->BindTexture(0);
					_Mdl->Draw();
					m_gfx->BindTexture(_Tex);
				}
				else if (!m_DrawMode)
				{
					Graphics::VertexBuffer* _VB = _Mdl->GetVertexBuffer();
					m_OldPrimitiveType = _VB->SetPrimitive(Pxf::Graphics::VB_PRIMITIVE_TRIANGLES);
					_Mdl->Draw();
				}
				else
					_Mdl->Draw();
			}
		}
			
		// Unbind shader
		m_gfx->BindShader(0);
	
		// Unbind FBO
		m_gfx->UnbindFrameBufferObject();
	
		// Detach texture
		for(int i = num_attach-1; i >= 0; i--)
		{
			m_Renderer->m_FBO->Detach(attach_lut[i]);
		}
		
		// Detach depth buffer
		m_Renderer->m_FBO->Detach(GL_DEPTH_ATTACHMENT_EXT);
		
		// Disable depth again
		glDisable(GL_DEPTH_TEST);
		
		glLoadIdentity();
		
		// End block timer
		m_ProfileTimer.Stop();
		// do something with result m_ProfileTimer.Interval()
		
		// Send preview
		if (_SendPreviews)//m_Renderer->m_Net->NumClients() > 0)
		{
			for (Util::Map<Util::String, void*>::iterator iter = m_Outputs.begin(); iter != m_Outputs.end(); ++iter)
			{
				//m_Renderer->m_FBO->Attach((Graphics::Texture*)((*iter).second), attach_lut[num_attach], false);
				//num_attach += 1;
				Resource::Image* img = m_gfx->CreateImageFromTexture((Graphics::Texture*)((*iter).second));//GetResult());
				Network::Packet* imgpacket = m_Renderer->m_NetDevice->CreateEmptyPacket("imgdata", m_Renderer->m_NetTag_Preview);
				imgpacket->PushString(m_BlockName, strlen(m_BlockName)); // block name
				//imgpacket->PushString(((Util::String)((*iter).first)).c_str(), ((Util::String)((*iter).first)).size()); // output name
				if (m_Outputs.size() > 1)
				{
					imgpacket->PushString(((Util::String)((*iter).first)).c_str(), ((Util::String)((*iter).first)).size()); // output name
				} else {
					imgpacket->PushString("", 1); // output name
				}
				imgpacket->PushInt(img->Width());
				imgpacket->PushInt(img->Height());
				imgpacket->PushInt(img->Channels());
				imgpacket->PushString((const char*)img->Ptr(), img->Height()*img->Width()*img->Channels());

				//Kernel::GetInstance()->Log(m_LogTag | Logger::IS_INFORMATION, "Sending image to client '%d'", packet->GetSender());
				m_Renderer->m_Net->SendAllPacket(imgpacket);
				
				delete imgpacket;
				delete img;
			}
		}
		
	}
	
	// Return
	return Block::Execute(_SendPreviews);
}



//////////////////////////////////////////////////////////////////////////
// Post-Process
//

PostProcessBlock::~PostProcessBlock()
{
	if (m_Shader->IsValid())
		m_gfx->DestroyShader(m_Shader);
	if (m_OutputQuad)
		delete m_OutputQuad;

	for (Pxf::Util::Map<Pxf::Util::String, void*>::iterator iter = m_Outputs.begin(); iter != m_Outputs.end(); ++iter)
		m_gfx->DestroyTexture((Pxf::Graphics::Texture*)(*iter).second);
}

bool PostProcessBlock::Initialize(Json::Value *node)
{

	// First set blockname
	m_BlockName = (*node)["blockName"].asCString();

	// Setup texture properties
	m_Width = (*node)["blockData"]["width"].asInt();
	m_Height = (*node)["blockData"]["height"].asInt();

	// Store shader code
	m_VertShader = (*node)["blockData"]["shaderVert"].asCString();
	m_FragShader = (*node)["blockData"]["shaderFrag"].asCString();

	// Add inputs
	for(int i = 0; i < (*node)["blockInput"].size(); i++)
	{
		m_Inputs.insert( m_Inputs.end(), OutputStruct((*node)["blockInput"][i]["block"].asCString(), (*node)["blockInput"][i]["output"].asCString()) );
	}
	
	// Add outputs
	for(int i = 0; i < (*node)["blockOutput"].size(); i++)
	{
		m_OutputTypes.insert( std::make_pair((*node)["blockOutput"][i]["name"].asCString(), (*node)["blockOutput"][i]["type"].asCString()) );
	}

	// TODO: Add other post-process properties

	return true;
}

void PostProcessBlock::BuildGraph()
{
	if (!m_HasBeenBuilt)
	{
		
		// Build childs in trees and setup input pointers
		for (Util::Array<OutputStruct>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
		{
			Block *child = m_Renderer->m_Blocks[(*iter).block_name.c_str()];
			child->BuildGraph();
			m_InputBlocks.insert( std::make_pair((*iter).block_name.c_str(), child) );
		}
		
		// Setup output textures
		for (Util::Map<Util::String, Util::String>::iterator iter = m_OutputTypes.begin(); iter != m_OutputTypes.end(); ++iter)
		{
			Graphics::Texture* toutputtex = m_gfx->CreateEmptyTexture(m_Width, m_Height);
			m_Outputs.insert( std::make_pair((*iter).first, (void*)toutputtex) );
		}
		
		// Setup internal block stuff
		m_OutputQuad = new SimpleQuad(0, 0, m_Width, m_Height);
		m_Shader = m_gfx->CreateShader(m_BlockName, m_VertShader, m_FragShader);
		
		m_HasBeenBuilt = true;
	}
}

void PostProcessBlock::ResetPerformed()
{
	// Reset inputs
	for (Util::Map<Util::String, Block*>::iterator iter = m_InputBlocks.begin(); iter != m_InputBlocks.end(); ++iter)
	{
		(*iter).second->ResetPerformed();
	}
	
	// Reset myself
	Block::ResetPerformed();
}

bool PostProcessBlock::Execute(bool _SendPreviews)
{
	if (!m_IsPerformed) {
		// Execute prereqs
		for (Util::Map<Util::String, Block*>::iterator iter = m_InputBlocks.begin(); iter != m_InputBlocks.end(); ++iter)
		{
			(*iter).second->Execute(_SendPreviews);
		}
		
		m_ProfileTimer.Start();
	
		// Setup OGL context etc
		m_gfx->SetViewport(0, 0, m_Width, m_Height);
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, m_Width, 0, m_Height, -1.0f, 10000.0f);
	  m_gfx->SetProjection(&prjmat);
		glDisable(GL_DEPTH_TEST);
	  glClear(GL_COLOR_BUFFER_BIT);
	
		// Attach and bind all render texture outputs to FBO
		int attach_lut[] = {GL_COLOR_ATTACHMENT0_EXT,
			                  GL_COLOR_ATTACHMENT1_EXT,
			                  GL_COLOR_ATTACHMENT2_EXT,
			                  GL_COLOR_ATTACHMENT3_EXT};
		int num_attach = 0;
		for (Util::Map<Util::String, void*>::iterator iter = m_Outputs.begin(); iter != m_Outputs.end(); ++iter)
		{
			m_Renderer->m_FBO->Attach((Graphics::Texture*)((*iter).second), attach_lut[num_attach], false);
			num_attach += 1;
		}
		m_gfx->BindFrameBufferObject(m_Renderer->m_FBO);
	
		// Bind shaders
		if (m_Shader->IsValid())
			m_gfx->BindShader(m_Shader);
	
		// Gather and bind all our inputs
		int ttexunit = 0;
		for (Util::Array<OutputStruct>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
		{
			// Get pointer to input block
			Block* inputblock = m_InputBlocks[(*iter).block_name.c_str()];
		
			// Get texture pointer
			Pxf::Util::String inputtype = inputblock->GetOutputType((*iter).block_output);
			if (inputtype == "texture")
			{
				// Input is a texture, bind and set uniform
				Graphics::Texture* inputtex = (Graphics::Texture*)inputblock->GetOutput((*iter).block_output);
				if (inputtex == 0) {
					return false;
				}
		
				m_gfx->BindTexture(inputtex, ttexunit);
				if (m_Shader->IsValid())
					m_gfx->SetUniformi(m_Shader, (*iter).block_output.c_str(), ttexunit);
				ttexunit += 1;
			
			} else if (inputtype == "float")
			{
				// Input is a script with float result
				float* scriptres = (float*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformf(m_Shader, (*iter).block_output.c_str(), *scriptres);
			
			} else if (inputtype == "int")
			{
				// Input is a script with int result
				int* scriptres = (int*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformi(m_Shader, (*iter).block_output.c_str(), *scriptres);

			} else if (inputtype == "vec2")
			{
				// Input is a script with vec2 result
				Math::Vec2f* scriptres = (Math::Vec2f*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformVec2(m_Shader, (*iter).block_output.c_str(), scriptres);

			} else if (inputtype == "vec3")
			{
				// Input is a script with vec3 result
				Math::Vec3f* scriptres = (Math::Vec3f*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformVec3(m_Shader, (*iter).block_output.c_str(), scriptres);

			}
		}
	
		// Render
		m_OutputQuad->Draw();
	
		// Unbind shader
		m_gfx->BindShader(0);
	
		// Unbind FBO
		m_gfx->UnbindFrameBufferObject();
	
		// Detach texture
		for(int i = num_attach-1; i >= 0; i--)
		{
			m_Renderer->m_FBO->Detach(attach_lut[i]);
		}
		
		// End block timer
		m_ProfileTimer.Stop();
		// do something with result m_ProfileTimer.Interval()
		
		// Send preview
		if (_SendPreviews)//m_Renderer->m_Net->NumClients() > 0)
		{
			for (Util::Map<Util::String, void*>::iterator iter = m_Outputs.begin(); iter != m_Outputs.end(); ++iter)
			{
				//m_Renderer->m_FBO->Attach((Graphics::Texture*)((*iter).second), attach_lut[num_attach], false);
				//num_attach += 1;
				Resource::Image* img = m_gfx->CreateImageFromTexture((Graphics::Texture*)((*iter).second));
				Network::Packet* imgpacket = m_Renderer->m_NetDevice->CreateEmptyPacket("imgdata", m_Renderer->m_NetTag_Preview);
				imgpacket->PushString(m_BlockName, strlen(m_BlockName)); // block name
				if (m_Outputs.size() > 1)
				{
					imgpacket->PushString(((Util::String)((*iter).first)).c_str(), ((Util::String)((*iter).first)).size()); // output name
				} else {
					imgpacket->PushString("", 1); // output name
				}
				imgpacket->PushInt(img->Width());
				imgpacket->PushInt(img->Height());
				imgpacket->PushInt(img->Channels());
				imgpacket->PushString((const char*)img->Ptr(), img->Height()*img->Width()*img->Channels());

				//Kernel::GetInstance()->Log(m_LogTag | Logger::IS_INFORMATION, "Sending image to client '%d'", packet->GetSender());
				m_Renderer->m_Net->SendAllPacket(imgpacket);
				
				delete imgpacket;
				delete img;
			}
		}
	}
	
	// Return
	return Block::Execute(_SendPreviews);
}



//////////////////////////////////////////////////////////////////////////
// RootBlock
//

RootBlock::~RootBlock()
{
	delete m_OutputQuad;
	m_gfx->DestroyShader(m_Shader);
}

bool RootBlock::Initialize(Json::Value *node)
{
	
	// First set blockname
	m_BlockName = (*node)["blockName"].asCString();
	
	// Setup texture properties
	m_Width = (*node)["blockData"]["width"].asInt();
	m_Height = (*node)["blockData"]["height"].asInt();
	
	// Store shader code
	m_VertShader = (*node)["blockData"]["shaderVert"].asCString();
	m_FragShader = (*node)["blockData"]["shaderFrag"].asCString();
	
	// Add inputs
	for(int i = 0; i < (*node)["blockInput"].size(); i++)
	{
		m_Inputs.insert( m_Inputs.end(), OutputStruct((*node)["blockInput"][i]["block"].asCString(), (*node)["blockInput"][i]["output"].asCString()) );
	}
	
	// TODO: Add other root properties
	
	return true;
}

void RootBlock::BuildGraph()
{
	if (!m_HasBeenBuilt)
	{
		
		// Build childs in trees
		for (Util::Array<OutputStruct>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
		{
			Block *child = m_Renderer->m_Blocks[(*iter).block_name.c_str()];
			child->BuildGraph();
			m_InputBlocks.insert( std::make_pair((*iter).block_name.c_str(), child) );
		}	
		
		// Setup internal block stuff
		m_OutputQuad = new SimpleQuad(0, 0, m_Width, m_Height);
		m_OutputTexture = m_gfx->CreateEmptyTexture(m_Width, m_Height);
		m_Shader = m_gfx->CreateShader(m_BlockName, m_VertShader, m_FragShader);
		
		m_HasBeenBuilt = true;
	}
}

void RootBlock::ResetPerformed()
{
	// Reset inputs
	for (Util::Map<Util::String, Block*>::iterator iter = m_InputBlocks.begin(); iter != m_InputBlocks.end(); ++iter)
	{
		(*iter).second->ResetPerformed();
	}
	
	// Reset myself
	Block::ResetPerformed();
}

bool RootBlock::Execute(bool _SendPreviews)
{
	if (!m_IsPerformed) {
			// Execute prereqs
		for (Util::Map<Util::String, Block*>::iterator iter = m_InputBlocks.begin(); iter != m_InputBlocks.end(); ++iter)
		{
			(*iter).second->Execute(_SendPreviews);
		}

		// Setup OGL context etc
		m_gfx->SetViewport(0, 0, m_Width, m_Height);
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, m_Width, 0, m_Height, -1.0f, 10000.0f);
		 m_gfx->SetProjection(&prjmat);
		glDisable(GL_DEPTH_TEST);
		 glClear(GL_COLOR_BUFFER_BIT);

		// Attach and bind render texture to FBO
		m_Renderer->m_FBO->Attach(m_OutputTexture, GL_COLOR_ATTACHMENT0_EXT, false);
		m_gfx->BindFrameBufferObject(m_Renderer->m_FBO);

		// Bind shaders
		m_gfx->BindShader(m_Shader);

		// Gather and bind all our inputs
		int ttexunit = 0;
		for (Util::Array<OutputStruct>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
		{
			// Get pointer to input block
			Block* inputblock = m_InputBlocks[(*iter).block_name.c_str()];
	
			// Get texture pointer
			Pxf::Util::String inputtype = inputblock->GetOutputType((*iter).block_output.c_str());
			if (inputtype == "texture")
			{
				// Input is a texture, bind and set uniform
				Graphics::Texture* inputtex = (Graphics::Texture*)inputblock->GetOutput((*iter).block_output);
				if (inputtex == 0) {
					return false;
				}
				
				// Unbind textures
				m_gfx->BindTexture(inputtex, ttexunit);
				m_gfx->SetUniformi(m_Shader, (*iter).block_output.c_str(), ttexunit);//inputtex->GetTextureID());
				ttexunit += 1;
		
			} else if (inputtype == "float")
			{
				// Input is a script with float result
				float* scriptres = (float*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformf(m_Shader, (*iter).block_output.c_str(), *scriptres);
		
			} else if (inputtype == "int")
			{
				// Input is a script with int result
				int* scriptres = (int*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformi(m_Shader, (*iter).block_output.c_str(), *scriptres);

			} else if (inputtype == "vec2")
			{
				// Input is a script with vec2 result
				Math::Vec2f* scriptres = (Math::Vec2f*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformVec2(m_Shader, (*iter).block_output.c_str(), scriptres);

			} else if (inputtype == "vec3")
			{
				// Input is a script with vec3 result
				Math::Vec3f* scriptres = (Math::Vec3f*)inputblock->GetOutput((*iter).block_output);
				m_gfx->SetUniformVec3(m_Shader, (*iter).block_output.c_str(), scriptres);

			}
		}

		// Render
		m_OutputQuad->Draw();

		// Unbind shader
		m_gfx->BindShader(0);
		
		// Unbind textures
		for(int i = ttexunit-1; i >= 0; --i)
		{
			m_gfx->BindTexture(0, i);
		}

		// Unbind FBO
		m_gfx->UnbindFrameBufferObject();

		// Detach texture
		m_Renderer->m_FBO->Detach(GL_COLOR_ATTACHMENT0_EXT);
		
		// Send preview
		if (_SendPreviews)//m_Renderer->m_Net->NumClients() > 0)
		{
			/*for (Util::Map<Util::String, void*>::iterator iter = m_Outputs.begin(); iter != m_Outputs.end(); ++iter)
			{*/
				//m_Renderer->m_FBO->Attach((Graphics::Texture*)((*iter).second), attach_lut[num_attach], false);
				//num_attach += 1;
				Resource::Image* img = m_gfx->CreateImageFromTexture(m_OutputTexture);
				Network::Packet* imgpacket = m_Renderer->m_NetDevice->CreateEmptyPacket("imgdata", m_Renderer->m_NetTag_Preview);
				imgpacket->PushString(m_BlockName, strlen(m_BlockName)); // block name
				imgpacket->PushString("",1);//((String)((*iter).first)).c_str(), ((String)((*iter).first)).size()); // output name
				imgpacket->PushInt(img->Width());
				imgpacket->PushInt(img->Height());
				imgpacket->PushInt(img->Channels());
				imgpacket->PushString((const char*)img->Ptr(), img->Height()*img->Width()*img->Channels());

				//Kernel::GetInstance()->Log(m_LogTag | Logger::IS_INFORMATION, "Sending image to client '%d'", packet->GetSender());
				m_Renderer->m_Net->SendAllPacket(imgpacket);
				
				delete imgpacket;
				delete img;
			//}
		}
	}

	// Return
	return Block::Execute(_SendPreviews);
}


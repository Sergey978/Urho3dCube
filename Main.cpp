#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Input/Input.h>
#include<Urho3D\UI\Window.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Input/Input.h>

using namespace Urho3D;
class MyApp : public Application
{
public:
	MyApp(Context* context) :
		Application(context), uiRoot_(GetSubsystem<UI>()->GetRoot()),
		input_(GetSubsystem<Input>())
	{
	}
	virtual void Setup();
	virtual void Start();
	virtual void Stop();
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleUpdate(StringHash eventType, VariantMap& eventData);

private:
	SharedPtr<Scene> scene_;

	// The UI's root UIElement.
	SharedPtr<UIElement> uiRoot_;
	SharedPtr<Input>  input_;

	// Size 
	Text* sizeLabel_;
	float cubeScale_ = 0.5f;

	// x
	Text* xLabel_;
	float xAngle_ = 45.0f;
	float x_ = 0;

	//y
	Text* yLabel_;
	float yAngle_ = 45.0f;
	float y_ = 0;

	// z
	Text* zLabel_;
	float zAngle_ = 45.0f;
	float z_ = 0;

	void CreateScene();
	void CreateUI();
	void CreateInstructions();
	void HandleSizeSliderChanged(StringHash eventType, VariantMap& eventData);
	void HandleXSliderChanged(StringHash eventType, VariantMap& eventData);
	void HandleYSliderChanged(StringHash eventType, VariantMap& eventData);
	void HandleZSliderChanged(StringHash eventType, VariantMap& eventData);

};
URHO3D_DEFINE_APPLICATION_MAIN(MyApp)

inline void MyApp::Setup()
{
	// Called before engine initialization. engineParameters_ member variable can be modified here
	engineParameters_["FullScreen"] = false;

}

inline void MyApp::Start()
{
	// Called after engine initialization. Setup application & subscribe to events here
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MyApp, HandleKeyDown));

	// Enable OS cursor
	GetSubsystem<Input>()->SetMouseVisible(true);

	// Load XML file containing default UI style sheet
	auto* cache = GetSubsystem<ResourceCache>();
	auto* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

	// Set up global UI style into the root UI element
	uiRoot_->SetDefaultStyle(style);

	CreateUI();
	CreateScene();
	CreateInstructions();
}

inline void MyApp::Stop()
{
	// Perform optional cleanup after main loop has terminated
}

inline void MyApp::HandleKeyDown(StringHash eventType, VariantMap & eventData)
{
	using namespace KeyDown;
	// Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
	int key = eventData[P_KEY].GetInt();
	if (key == KEY_ESCAPE)
		engine_->Exit();
}

inline void MyApp::HandleUpdate(StringHash eventType, VariantMap & eventData)
{
	float timeStep = eventData["TimeStep"].GetFloat();
	auto boxNode = scene_->GetChild("MyBox");
	boxNode->SetRotation(Quaternion(xAngle_, yAngle_, zAngle_));
	boxNode->SetScale(cubeScale_);
	boxNode->SetPosition(Vector3(x_, y_, z_));
}

void MyApp::CreateScene()
{
	scene_ = new Scene(context_);
	scene_->CreateComponent<Octree>();

	auto cameraNode = scene_->CreateChild("MyCamera");
	cameraNode->CreateComponent<Camera>();
	cameraNode->SetPosition(Vector3(0.0f, 0.0f, -5.0f));

	auto viewport = new Viewport(context_, scene_, cameraNode->GetComponent<Camera>());
	GetSubsystem<Renderer>()->SetViewport(0, viewport);

	auto boxNode = scene_->CreateChild("MyBox");

	// Set init scale for node
	boxNode->SetScale(cubeScale_);
	auto boxObject = boxNode->CreateComponent<StaticModel>();

	auto cache = GetSubsystem<ResourceCache>();
	boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
	boxObject->SetMaterial(cache->GetResource<Material>("Materials/Editor/MyRed.xml"));

	boxNode->SetRotation(Quaternion(xAngle_, yAngle_, zAngle_));

	auto lightNode = scene_->CreateChild("MyLight");
	auto light = lightNode->CreateComponent<Light>();
	light->SetLightType(LIGHT_DIRECTIONAL);
	lightNode->SetDirection(Vector3(0.6f, -0.6f, 0.8f));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MyApp, HandleUpdate));

}

void MyApp::CreateUI()
{
	auto* cache = GetSubsystem<ResourceCache>();

	// size Label and Slider
	sizeLabel_ = uiRoot_->CreateChild<Text>();
	sizeLabel_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
	sizeLabel_->SetPosition(370, 50);
	sizeLabel_->SetTextEffect(TE_SHADOW);

	auto* sizeSlider = uiRoot_->CreateChild<Slider>();
	sizeSlider->SetStyleAuto();
	sizeSlider->SetPosition(50, 50);
	sizeSlider->SetSize(300, 10);
	sizeSlider->SetRange(2.0f); // 0 - 2 range
	SubscribeToEvent(sizeSlider, E_SLIDERCHANGED, URHO3D_HANDLER(MyApp, HandleSizeSliderChanged));
	sizeSlider->SetValue(1.5f);

	// X Label and Slider
	xLabel_ = uiRoot_->CreateChild<Text>();
	xLabel_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
	xLabel_->SetPosition(370, 70);
	xLabel_->SetTextEffect(TE_SHADOW);

	auto* xSlider = uiRoot_->CreateChild<Slider>();
	xSlider->SetStyleAuto();
	xSlider->SetPosition(50, 70);
	xSlider->SetSize(300, 10);
	xSlider->SetRange(10.0f); // 0 - 10 range
	SubscribeToEvent(xSlider, E_SLIDERCHANGED, URHO3D_HANDLER(MyApp, HandleXSliderChanged));
	xSlider->SetValue(5.0f);

	// Y Label and Slider
	yLabel_ = uiRoot_->CreateChild<Text>();
	yLabel_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
	yLabel_->SetPosition(370, 90);
	yLabel_->SetTextEffect(TE_SHADOW);

	auto* ySlider = uiRoot_->CreateChild<Slider>();
	ySlider->SetStyleAuto();
	ySlider->SetPosition(50, 90);
	ySlider->SetSize(300, 10);
	ySlider->SetRange(10.0f); // 0 - 10 range
	SubscribeToEvent(ySlider, E_SLIDERCHANGED, URHO3D_HANDLER(MyApp, HandleYSliderChanged));
	ySlider->SetValue(5.0f);

	// Z Label and Slider
	zLabel_ = uiRoot_->CreateChild<Text>();
	zLabel_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
	zLabel_->SetPosition(370, 110);
	zLabel_->SetTextEffect(TE_SHADOW);

	auto* zSlider = uiRoot_->CreateChild<Slider>();
	zSlider->SetStyleAuto();
	zSlider->SetPosition(50, 110);
	zSlider->SetSize(300, 10);
	zSlider->SetRange(10.0f); // 0 - 10 range
	SubscribeToEvent(zSlider, E_SLIDERCHANGED, URHO3D_HANDLER(MyApp, HandleZSliderChanged));
	zSlider->SetValue(5.0f);

}

void MyApp::CreateInstructions()
{
	auto* cache = GetSubsystem<ResourceCache>();
	
	// Construct new Text object, set string to display and font to use
	auto* instructionText = uiRoot_->CreateChild<Text>();
	instructionText->SetText("Use Ctrl key and sliders to move cube");
	instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);

	// Position the text relative to the screen center
	instructionText->SetHorizontalAlignment(HA_CENTER);
	instructionText->SetVerticalAlignment(VA_CENTER);
	instructionText->SetPosition(0, uiRoot_->GetHeight() / 4);
}

void MyApp::HandleSizeSliderChanged(StringHash eventType, VariantMap & eventData)
{
	float newValue = eventData[SliderChanged::P_VALUE].GetFloat();
	cubeScale_ = newValue + 0.5f;
	sizeLabel_->SetText("Size: " + String(cubeScale_));
}

void MyApp::HandleXSliderChanged(StringHash eventType, VariantMap & eventData)
{
	float newValue = eventData[SliderChanged::P_VALUE].GetFloat();
	if (input_->GetKeyDown(KEY_CTRL))
	{
		x_ = (newValue - 5) / 5;
	}
	else
	{
		xAngle_ = newValue * 36.0f;
	}
	xLabel_->SetText("X angle: " + String(xAngle_) + "; X pos: " + String(x_));
}

void MyApp::HandleYSliderChanged(StringHash eventType, VariantMap & eventData)
{
	float newValue = eventData[SliderChanged::P_VALUE].GetFloat();
	if (input_->GetKeyDown(KEY_CTRL))
	{
		y_ = (newValue - 5) / 5;
	}
	else
	{
		yAngle_ = newValue * 36.0f;
	}
	yLabel_->SetText("Y angle: " + String(yAngle_) + "; Y pos: " + String(y_));
}

void MyApp::HandleZSliderChanged(StringHash eventType, VariantMap & eventData)
{
	float newValue = eventData[SliderChanged::P_VALUE].GetFloat();
	if (input_->GetKeyDown(KEY_CTRL))
	{
		z_ = (newValue - 5) / 5;
	}
	else
	{
		zAngle_ = newValue * 36.0f;
	}
	zLabel_->SetText("Z angle: " + String(zAngle_) + "; Z pos: " + String(z_));
}



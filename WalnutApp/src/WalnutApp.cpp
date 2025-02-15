#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
//#include "../Generator.h"

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override 
	{
		texture = std::make_shared<Walnut::Image>("stippling_brush_texture.png");
		logo = std::make_shared<Walnut::Image>("LogoV3.png");
		image_number = 6;
	}
	virtual void OnUIRender() override
	{
		//Window1
		ImGui::Begin("Settings");
		ImGui::BeginVertical(1);
		ImGui::SliderInt("Number of Images", &image_number, 4, 10);
		ImGui::Text("Stippling Texture:");
		//ImGui::BeginDragDropSource;
		ImGui::Image(texture->GetDescriptorSet(), { 100.0f, 100.0f });
		//ImGui::EndDragDropSource;
		if (ImGui::Button("Generate")) {
			//generator->generate_TAM(image_number);
		}
		ImGui::EndVertical();
		ImGui::End();

		//Window2
		ImGui::Begin("Preview");
		ImGui::BeginHorizontal(0);
		for (int h = 0; h < image_number; h++) {
			ImGui::BeginVertical(h);
			float width = 50.0f;
			for (int w = 0; w < 4; w++) {
				ImGui::Image(logo->GetDescriptorSet(), { width, width });
				width += 50.0f;
			}
			ImGui::EndVertical();
		}
		ImGui::EndHorizontal();
		ImGui::End();

		//ImGui::ShowDemoWindow();
	}
private:
	std::shared_ptr<Walnut::Image> texture;
	std::shared_ptr<Walnut::Image> logo;
	int image_number;
	//std::shared_ptr<Generator> generator;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "TAMGenerator";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Set Path");
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}
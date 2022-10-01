#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include <Windows.h>
#include <string>
#include <shobjidl.h>
#include <iostream>

#include "Walnut/Image.h"
#include "../Generator.h"

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override 
	{
		texture = std::make_shared<Walnut::Image>("stippling_brush_texture.png");
		cv_texture = std::make_shared<cv::Mat>(cv::imread("stippling_brush_texture.png", cv::IMREAD_UNCHANGED));
		/*cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
		cv::imshow("image", *cv_texture);
		cv::waitKey(30);*/
		logo = std::make_shared<Walnut::Image>("LogoV3.png");
		float initial = leftmost_tone_value;
		for (int i = 0; i < image_number; i++) {
			tone_values.push_back(initial);
			initial += 1.0f;
		}
		std::shared_ptr<Generator> generator = std::make_shared<Generator>();
		tam = generator->generate_TAM(image_number, resolution, path, *cv_texture, stippling_dot_size, tone_values);
	}
	virtual void OnUIRender() override
	{
		//Window1
		ImGui::Begin("Settings");
		ImGui::BeginVertical(1);
		ImGui::SliderInt("Number of Rows", &image_number, 4, 10);
		ImGui::SliderFloat("Initial Brightness", &leftmost_tone_value, 0.0f, 1.0f);
		ImGui::Combo("Resolution", &current_item, " 540x540\0 1080x1080\0 2160x2160");
		ImGui::Text("Stippling Texture:");
		//ImGui::BeginDragDropSource;
		ImGui::Image(texture->GetDescriptorSet(), { 100.0f, 100.0f });
		ImGui::SliderInt("stippling_dot_size", &stippling_dot_size, 1, 400);
		//ImGui::EndDragDropSource;
		if (current_item == 0) {
			resolution = 540;
		}
		else if(current_item == 1)
		{
			resolution = 1080;
		}
		else if (current_item == 2)
		{
			resolution = 2160;
		}

		if (ImGui::Button("Generate")) {
			tone_values.clear();
			double initial = leftmost_tone_value;
			for (int i = 0; i < image_number; i++) {
				tone_values.push_back(initial);
				initial += 1.0;
			}
			std::cout << tone_values.size() << std::endl;
			tam = generator->generate_TAM(image_number, resolution, path, *cv_texture, stippling_dot_size, tone_values);
		}
		
		ImGui::EndVertical();
		ImGui::End();

		//Window2
		ImGui::Begin("Preview");
		ImGui::BeginHorizontal(0);
		int quad = 0;
		for (int h = 0; h < tam.size()/4; h++) {
			ImGui::BeginVertical(h);
			float res = 25;
			for (int w = quad; w < quad + 4; w++) {
				ImGui::Image(tam[w]->GetDescriptorSet(), {res, res});
				res = res * 2;
			}
			ImGui::EndVertical();
			quad += 4;
		}
		ImGui::EndHorizontal();
		ImGui::End();

		//ImGui::ShowDemoWindow();
	}
private:
	std::shared_ptr<Walnut::Image> texture;
	std::shared_ptr<cv::Mat> cv_texture;
	std::shared_ptr<Walnut::Image> logo;
	int image_number = 4;
	int current_item = 1;
	int resolution = 1080;
	std::string path = "./tam_images";
	std::shared_ptr<Generator> generator;
	std::vector<std::shared_ptr<Walnut::Image>> tam;
	int stippling_dot_size = 50;
	std::vector<float> tone_values;
	float leftmost_tone_value = 0.5f;
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
			if (ImGui::MenuItem("Set Path for TAM"))
			{

			}
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}
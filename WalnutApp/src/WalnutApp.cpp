#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include <Windows.h>
#include <string>
#include <shobjidl.h>
#include <iostream>
#include <filesystem>

#include "Walnut/Image.h"
#include "Generator.h"

std::string tam_path = "./tam_images";

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override 
	{
		//load example brush-textures
		for (int i = 1; i <= 8; i++)
		{
			auto path = "example_" + std::to_string(i) + ".png";
			textures.push_back(std::make_shared<Walnut::Image>(path));
			cv_textures.push_back(std::make_shared<cv::Mat>(cv::imread(path, cv::IMREAD_UNCHANGED)));
		}
		
		//initialize brightness values
		texture = textures[0];
		cv_texture = cv_textures[0];
		double initial = leftmost_tone_value;
		double step = (leftmost_tone_value - rightmost_tone_value) / image_number;
		for (int i = 0; i < image_number; i++) {
			initial -= step;
			tone_values.push_back(initial);
		}

		//generate first TAM
		std::shared_ptr<Generator> generator = std::make_shared<Generator>();
		tam = generator->generate_TAM(image_number, resolution, tam_path, *cv_texture, size, tone_values);
	}
	virtual void OnUIRender() override
	{
		//Window1
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.x = 500.0f;
		ImGui::Begin("Settings");
		ImGui::BeginVertical(1);
		ImGui::Text("Number of Columns:");
		ImGui::SliderInt("##cols", &image_number, 4, 10);
		ImGui::Text("Brightness brightest level:");
		ImGui::SliderFloat("##b1", &leftmost_tone_value, 500.0f, 900.0f);
		ImGui::Text("Brightness darkest level:");
		ImGui::SliderFloat("##b2", &rightmost_tone_value, 100.0f, 500.0f);
		ImGui::Text("Resolution:");
		ImGui::Combo("", &current_item, " 540x540\0 1080x1080\0 2160x2160");
		ImGui::Text("Stippling Texture:");
		if (ImGui::ImageButton(texture->GetDescriptorSet(), { 100.0f, 100.0f }))
		{
			ImGui::OpenPopup("select_texture");
		}
		if (ImGui::BeginPopup("select_texture"))
		{
			ImGui::BeginHorizontal(100);
			for (int i = 0; i < textures.size(); i++)
			{
				if (ImGui::ImageButton(textures[i]->GetDescriptorSet(), {50.0f, 50.0f}))
				{
					texture = textures[i];
					cv_texture = cv_textures[i];
				}
			}
			ImGui::EndHorizontal();
			ImGui::BeginHorizontal(101);
			if (ImGui::Button("Add custom texture +", ImVec2(ImGui::GetItemRectSize().x, 0.0f)))
				ImGui::OpenPopup("custom_texture");
			if (ImGui::BeginPopup("custom_texture"))
			{
				ImGui::Text("Enter a valid path to your texture:");
				static char buf1[64] = "";
				ImGui::InputText("##default", buf1, 64);
				bool exists = false;
				for (auto const& element : ex_paths)
				{
					if (element == buf1) {
						exists = true;
					}
				}
				if (std::filesystem::exists(buf1) && !exists) {
					textures.push_back(std::make_shared<Walnut::Image>(buf1));
					cv_textures.push_back(std::make_shared<cv::Mat>(cv::imread(buf1, cv::IMREAD_UNCHANGED)));
					ex_paths.push_back(buf1);
				}
				ImGui::EndPopup();
			}
			ImGui::EndHorizontal();
			ImGui::EndPopup();
		}
		ImGui::SliderInt("size", &size, 1, 200);

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
			double step = (leftmost_tone_value - rightmost_tone_value) / image_number;
			for (int i = 0; i < image_number; i++) {
				initial -= step;
				tone_values.push_back(initial);
			}
			tam = generator->generate_TAM(image_number, resolution, tam_path, *cv_texture, size, tone_values);
		}
		
		ImGui::EndVertical();
		ImGui::End();
		style.WindowMinSize.x = 32.0f;

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
	}
private:
	std::vector<std::shared_ptr<Walnut::Image>> textures;
	std::shared_ptr<Walnut::Image> texture;
	std::vector<std::shared_ptr<cv::Mat>> cv_textures;
	std::shared_ptr<cv::Mat> cv_texture;
	int image_number = 6;
	int current_item = 1;
	int resolution = 1080;
	std::string custom_tex_path = "";
	std::shared_ptr<Generator> generator;
	std::vector<std::shared_ptr<Walnut::Image>> tam;
	int size = 100;
	std::vector<float> tone_values;
	std::vector<std::string> ex_paths;
	float leftmost_tone_value = 700.0f;
	float rightmost_tone_value = 400.0f;
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
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}
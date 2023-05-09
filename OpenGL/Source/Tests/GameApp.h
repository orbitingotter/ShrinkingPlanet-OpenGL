#pragma once

#include "Sandbox.h"

#include "Window.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Graphics/Cubemap.h"

#include "Vendor/glm/gtc/random.hpp"

#include <memory>

class GameApp : public Sandbox
{
public:
	GameApp()
	{
		mShader = std::make_unique<Shader>("Source/Shaders/Shadow.glsl");

		mPlanetModel = std::make_unique<Model>("Resources/Models/Planetv2.obj");
		mCarModel = std::make_unique<Model>("Resources/Models/Camaro.fbx");
		meteorModel = std::make_unique<Model>("Resources/Models/sphere.obj");


		mPlanetMat = glm::scale(glm::mat4(1.0f), glm::vec3(planetStartRadius));

		worldUp = glm::vec3(0, 1, 0);
		carPos = glm::vec3(0, planetStartRadius, 0);
		carMoveDirection = glm::vec3(0, 0, 1);
		carRight = glm::cross(carMoveDirection, worldUp);// move dir x up
		mCarMat = glm::scale(glm::translate(glm::mat4(1.0f), carPos), glm::vec3(0.004f));

		meteorMatList.reserve(maxMeteorCount);

		cameraPos = carPos / glm::length(carPos) * cameraHeight;
		cameraUp = carMoveDirection;

		mView = GetViewMatrix();
		mProj = glm::perspective(glm::radians(60.0f), window.GetAspectRatio(), 0.1f, 1000.0f);

		mDirectionalLight = glm::vec3(0.0f, -1.0f, 2.0f);

		std::vector<std::string> facePaths =
		{
			"Resources/Skybox/Sky/right.png",
			"Resources/Skybox/Sky/left.png",
			"Resources/Skybox/Sky/top.png",
			"Resources/Skybox/Sky/bottom.png",
			"Resources/Skybox/Sky/front.png",
			"Resources/Skybox/Sky/back.png"
		};

		mCubemap = std::make_unique<Cubemap>(facePaths);
		mCubemapShader = std::make_unique<Shader>("Source/Shaders/Cubemap.glsl");


		// shadow map
		glGenFramebuffers(1, &shadowMapFBO);

		glGenTextures(1, &shadowMapTexture);
		glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Shadow FBO Error\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		smProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 1000.0f);
		smLightView = glm::lookAt(50.0f * glm::normalize(mDirectionalLight), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		smFinalProjection = smProjection * smLightView;

		shadowMapShader = std::make_unique<Shader>("Source/Shaders/ShadowMap.glsl");

	}

	~GameApp()
	{
	}

	void OnUpdate() override
	{
		// GAME UPDATE

		// planet
		if (planetRadius > planetMinRadius)
		{
			planetRadius -= shrinkRate;
			cameraHeight -= shrinkRate;
		}

		mPlanetMat = glm::scale(glm::mat4(1.0f), glm::vec3(planetRadius));

		// rotate car along move direction
		glm::vec3 oldCarPos = glm::vec3(mCarMat[3]);
		mCarMat = glm::translate(mCarMat, carMoveDirection * carMoveSpeed);
		glm::vec3 newCarPos = glm::vec3(mCarMat[3]);
		float angle = acos(glm::dot(oldCarPos, newCarPos) / (glm::length(oldCarPos) * glm::length(newCarPos)));
		mCarMat = glm::rotate(mCarMat, angle, -carRight);

		newCarPos = glm::vec3(mCarMat[3]);
		carPos = newCarPos;

		// car height constrains
		float heightDiff = glm::length(carPos) - planetRadius;
		if (heightDiff > 0) // planet not on surface
			mCarMat = glm::translate(mCarMat, glm::vec3(0.0f, -heightDiff * 1000.0f, 0.0f));

		carRight = glm::cross(carMoveDirection, worldUp);// move dir x up

		// meteor

		// meteor spawn
		if (window.GetCurrentTime() - previousTime > meteorSpawnRate)
		{
			if (meteorCurrentCount < maxMeteorCount)
			{
				meteorMatList.push_back(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(meteorScaleFactor)), glm::sphericalRand(meteorHeight)));
				meteorCurrentCount++;
			}
			previousTime = window.GetCurrentTime();
		}
		// meteor fall
		for (int i = 0; i < meteorCurrentCount; i++)
		{
			glm::vec3 meteorPos = glm::vec3(meteorMatList[i][3]);

			if (glm::length(meteorPos) >= planetRadius)
				meteorMatList[i] = glm::translate(meteorMatList[i], -meteorPos / glm::length(meteorPos) * 1.5f);

		}

		// collision detection
		glm::vec3 carPos = glm::vec3(mCarMat[3]);
		for (int i = 0; i < meteorCurrentCount; i++)
		{
			glm::vec3 meteorPos = glm::vec3(meteorMatList[i][3]);

			if (glm::length(carPos - meteorPos) <= hitRadius)
			{
				if (!gameOver)
				{
					std::cout << "GAME OVER!\n";
					gameOver = true;
				}
			}
		}

		if (gameOver)
		{
			// RESTART SCENE
			window.Close();
		}

		// camera changes
		cameraUp = glm::vec3(mCarMat[2][0], mCarMat[2][1], mCarMat[2][2]);;
		cameraPos = (carPos / glm::length(carPos)) * cameraHeight;


		// INPUT

		if (window.IsKeyPressed(GLFW_KEY_LEFT) || window.IsKeyPressed(GLFW_KEY_A))
		{
			mCarMat = glm::rotate(mCarMat, -carTurnSpeed, -glm::cross(carRight, carMoveDirection));
		}

		if (window.IsKeyPressed(GLFW_KEY_RIGHT) || window.IsKeyPressed(GLFW_KEY_D))
		{
			mCarMat = glm::rotate(mCarMat, carTurnSpeed, -glm::cross(carRight, carMoveDirection));
		}

		if (window.IsKeyPressed(GLFW_KEY_ENTER))
		{
			// CLOSE APPLICATION (no restart)
			restart = false;
			window.Close();
		}

		// DEFAULT UPDATE
		mView = GetViewMatrix();

		mShader->Bind();
		mShader->SetUniform("uView", mView);
		mShader->SetUniform("uProj", mProj);
		mShader->SetUniform("uDirectionalLight", mDirectionalLight);
		mShader->SetUniform("uCameraPos", cameraPos);
		mShader->SetUniform("uPcfEnabled", pcfEnabled);
		mShader->SetUniform("uShadowEnabled", shadowEnabled);
		mShader->SetUniform("uSampleRange", sampleRange);

		mCubemapShader->Bind();
		mCubemapShader->SetUniform("uView", glm::mat4(glm::mat3(mView)));
		mCubemapShader->SetUniform("uProj", mProj);


		smProjection = glm::ortho(-ortho, ortho, -ortho, ortho, 0.1f, 100.0f);
		smLightView = glm::lookAt(50.0f * -glm::normalize(mDirectionalLight), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		smFinalProjection = smProjection * smLightView;
	}

	void OnRender() override
	{
		renderer.Clear(0.1f, 0.1f, 0.1f, 0.0f);
		renderer.SetDepthTest(true);

		// shadow pass
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LESS);
		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		shadowMapShader->Bind();
		shadowMapShader->SetUniform("uSMProj", smFinalProjection);
		shadowMapShader->SetUniform("uModel", mPlanetMat);
		renderer.Draw(*mPlanetModel, *shadowMapShader);
		shadowMapShader->SetUniform("uModel", mCarMat);
		renderer.Draw(*mCarModel, *shadowMapShader);
		for (int i = 0; i < meteorCurrentCount; i++)
		{
			shadowMapShader->SetUniform("uModel", meteorMatList[i]);
			renderer.Draw(*meteorModel, *shadowMapShader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// normal render pass
		glCullFace(GL_BACK);
		glViewport(0, 0, window.GetWidth(), window.GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, shadowMapTexture);

		mShader->Bind();
		mShader->SetUniform("uSMProj", smFinalProjection);
		mShader->SetUniform("uModel", mPlanetMat);
		mShader->SetUniform("uModelColor", planetColor);
		renderer.Draw(*mPlanetModel, *mShader);
		mShader->SetUniform("uModel", mCarMat);
		mShader->SetUniform("uModelColor", carColor);
		renderer.Draw(*mCarModel, *mShader);

		mShader->SetUniform("uModelColor", meteorColor);
		for (int i = 0; i < meteorCurrentCount; i++)
		{
			mShader->SetUniform("uModel", meteorMatList[i]);
			renderer.Draw(*meteorModel, *mShader);
		}

		// cubemap render
		glDepthFunc(GL_LEQUAL);
		renderer.Draw(*mCubemap, *mCubemapShader);
	}

	void OnImGuiRender() override
	{
		static bool testWindow = true;
		if (testWindow)
		{
			ImGui::Begin("Info", &testWindow);
			ImGui::Text("FPS : %.1f FPS | Delta : %.2f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

			if (ImGui::Button("VSync"))
			{
				if (window.IsVSync())
					window.SetVSync(false);
				else
					window.SetVSync(true);
			}

			if (ImGui::Button("Wireframe ON"))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			if (ImGui::Button("Wireframe OFF"))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			//ImGui::Text("Vertex Count : %d", mModel->GetVertexCount());
			//ImGui::Text("Index Count : %d", mModel->GetIndexCount());
			//ImGui::Text("Texture Count : %d", mModel->GetTextureCount());
			//ImGui::Text("Mesh Count : %d", mModel->GetNumMeshes());

			ImGui::DragFloat3("Directional Light", &mDirectionalLight.x, 0.05f);
			ImGui::DragFloat("Ortho", &ortho, 1.0f, 5.0f, 500.0f);


			ImGui::Checkbox("Shadow enabled", &shadowEnabled);
			ImGui::Checkbox("PCF enabled", &pcfEnabled);
			ImGui::DragInt("Sample Range", &sampleRange, 2.0f, 1, 15);
			ImGui::Text("Num Samples : %d", sampleRange * sampleRange);

			ImGui::Image((ImTextureID)shadowMapTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
		}

		// GAME STATUS
		static bool statusWindow = true;
		if (statusWindow)
		{
			ImGui::Begin("Shrinking Planet", &statusWindow);

			ImGui::Text("DEBUG");
			ImGui::Text("Press Enter to Exit");


			ImGui::Separator();

			std::string text = "Radius : " + std::to_string(planetRadius).substr(0, std::to_string(planetRadius).find(".") + 2 + 1) + "m";
			ImGui::Text(text.c_str());
			ImGui::Text(std::string("Meteor Count : " + std::to_string(meteorCurrentCount) + " / " + std::to_string(maxMeteorCount)).c_str());

			ImGui::Separator();

			ImGui::SliderFloat("Shrink Rate", &shrinkRate, 0.001f, 0.005f);
			ImGui::SliderFloat("Car Speed", &carMoveSpeed, 2.0f, 40.0f);
			ImGui::SliderFloat("Car Turn Speed", &carTurnSpeed, 0.05f, 0.3f);
			ImGui::SliderFloat("Meteor Spawn Rate(s)", &meteorSpawnRate, 0.1f, 5.0f);
			ImGui::SliderFloat("Camera Height", &cameraHeight, 5.0f, 30.0f);



			ImGui::End();
		}
	}


private:
	inline glm::mat4 GetViewMatrix() { return glm::lookAt(cameraPos, glm::vec3(0.0f), cameraUp); }

private:
	// GAME MANAGER
	bool gameOver = false;

	// PLANET VARIABLES
	std::unique_ptr<Model> mPlanetModel;
	glm::mat4 mPlanetMat;
	float planetStartRadius = 10.0f;
	float planetMinRadius = 1.0f;
	float planetRadius = 10.0f;
	float shrinkRate = 0.001f;
	glm::vec3 planetColor = glm::vec3(0.0f, 1.0f, 0.0f);

	// CAR VARIABLES
	std::unique_ptr<Model> mCarModel;
	glm::mat4 mCarMat;
	glm::vec3 carPos;
	glm::vec3 carMoveDirection;
	glm::vec3 carRight;
	float carMoveSpeed = 20.0f;
	float carTurnSpeed = 0.1f;
	float hitRadius = 0.5f;

	glm::vec3 carColor = glm::vec3(1.0f, 0.0f, 0.0f);

	// METEOR VARIBLES
	const int maxMeteorCount = 100;
	int meteorCurrentCount = 0;
	float meteorHeight = 500.0f;
	std::unique_ptr<Model> meteorModel;
	std::vector<glm::mat4> meteorMatList;
	float meteorSpawnRate = 2.0f;
	float previousTime = 0.0f;
	float meteorScaleFactor = 0.1f;

	glm::vec3 meteorColor = glm::vec3(1.0f, 1.0f, 1.0f);

	// CAMERA VARIABLES
	glm::vec3 cameraPos;
	glm::vec3 worldUp;
	glm::vec3 cameraUp;

	float cameraHeight = 20.0f;


	// ENVIRONMENT VARIABLES
	std::unique_ptr<Shader> mShader;

	std::unique_ptr<Cubemap> mCubemap;
	std::unique_ptr<Shader> mCubemapShader;

	glm::vec3 mDirectionalLight;

	// CAMERA VARIABLES
	glm::mat4 mView;
	glm::mat4 mProj;


	// SHADOW MAPPING VARIABLES
	const int shadowWidth = 4096, shadowHeight = 4096;
	float ortho = 20.0f;
	unsigned int shadowMapFBO;
	unsigned int shadowMapTexture;
	glm::mat4 smProjection, smLightView, smFinalProjection;
	std::unique_ptr<Shader> shadowMapShader;

	bool pcfEnabled = true;
	bool shadowEnabled = true;
	int sampleRange = 5;
};
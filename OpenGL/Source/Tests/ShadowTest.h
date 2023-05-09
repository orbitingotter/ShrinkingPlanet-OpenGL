#pragma once

#include "Sandbox.h"

#include "Window.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Graphics/Cubemap.h"

#include <memory>


class ShadowTest : public Sandbox
{
public:
	ShadowTest()
	{
		mShader = std::make_unique<Shader>("Source/Shaders/Shadow.glsl");
		float start = glfwGetTime();
		mModel = std::make_unique<Model>("Resources/Models/sponza/sponza.obj");
		std::cout << "Model took " << glfwGetTime() - start << "s to load\n";

		mLightShader = std::make_unique<Shader>("Source/Shaders/SolidBasic.glsl");
		mLightModel = std::make_unique<Model>("Resources/Models/cube.obj");
		mLightPos = glm::vec3(0.0f, 0.0f, -2.0f);
		mDirectionalLight = glm::vec3(0.0f, -1.0f, 2.0f);

		mModelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
		//mView = camera.GetViewMatrix();
		mProj = glm::perspective(glm::radians(60.0f), window.GetAspectRatio(), 0.1f, 1000.0f);

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
		//smLightView = glm::lookAt(50.0f * glm::normalize(mDirectionalLight), camera.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
		smFinalProjection = smProjection * smLightView;

		shadowMapShader = std::make_unique<Shader>("Source/Shaders/ShadowMap.glsl");
	}

	~ShadowTest() {}

	void OnUpdate() override
	{
		//mView = camera.GetViewMatrix();

		mShader->Bind();
		mShader->SetUniform("uModel", mModelMat);
		mShader->SetUniform("uView", mView);
		mShader->SetUniform("uProj", mProj);
		mShader->SetUniform("uLightPos", mLightPos);
		mShader->SetUniform("uDirectionalLight", mDirectionalLight);
		//mShader->SetUniform("uCameraPos", camera.GetPosition());
		mShader->SetUniform("uPcfEnabled", pcfEnabled);
		mShader->SetUniform("uShadowEnabled", shadowEnabled);
		mShader->SetUniform("uSampleRange", sampleRange);


		const float radius = 5.0f;
		mLightPos.x = radius * sin(glfwGetTime());
		mLightPos.z = radius * cos(glfwGetTime());
		mLightPos.y = 10.0f;

		mLightShader->Bind();
		mLightShader->SetUniform("uModel", glm::scale(glm::translate(glm::mat4(1.0f), mLightPos), glm::vec3(0.2f)));
		mLightShader->SetUniform("uView", mView);
		mLightShader->SetUniform("uProj", mProj);

		mCubemapShader->Bind();
		mCubemapShader->SetUniform("uView", glm::mat4(glm::mat3(mView)));
		mCubemapShader->SetUniform("uProj", mProj);


		smProjection = glm::ortho(-ortho, ortho, -ortho, ortho, 0.1f, 100.0f);
		smLightView = glm::lookAt(50.0f * -glm::normalize(mDirectionalLight), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		smFinalProjection = smProjection * smLightView;


		if (window.IsKeyPressed(GLFW_KEY_ENTER))
		{
			window.Close();
		}

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
		shadowMapShader->SetUniform("uModel", mModelMat);

		renderer.Draw(*mModel, *shadowMapShader);
		shadowMapShader->SetUniform("uModel", glm::scale(glm::translate(glm::mat4(1.0f), mLightPos), glm::vec3(0.2f)));
		renderer.Draw(*mLightModel, *shadowMapShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// normal render pass
		glCullFace(GL_BACK);
		glViewport(0, 0, window.GetWidth(), window.GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, shadowMapTexture);

		mShader->Bind();
		mShader->SetUniform("uSMProj", smFinalProjection);

		renderer.Draw(*mModel, *mShader);
		renderer.Draw(*mLightModel, *mLightShader);

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

			ImGui::Text("Vertex Count : %d", mModel->GetVertexCount());
			ImGui::Text("Index Count : %d", mModel->GetIndexCount());
			ImGui::Text("Texture Count : %d", mModel->GetTextureCount());
			ImGui::Text("Mesh Count : %d", mModel->GetNumMeshes());

			ImGui::DragFloat3("Directional Light", &mDirectionalLight.x, 0.05f);
			ImGui::DragFloat("Ortho", &ortho, 1.0f, 5.0f, 500.0f);


			ImGui::Checkbox("Shadow enabled", &shadowEnabled);
			ImGui::Checkbox("PCF enabled", &pcfEnabled);
			ImGui::DragInt("Sample Range", &sampleRange, 2.0f, 1, 15);
			ImGui::Text("Num Samples : %d", sampleRange * sampleRange);

			ImGui::Image((ImTextureID)shadowMapTexture, ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
		}
	}

private:

	std::unique_ptr<Model> mModel;
	std::unique_ptr<Shader> mShader;

	std::unique_ptr<Model> mLightModel;
	std::unique_ptr<Shader> mLightShader;
	glm::vec3 mLightPos;
	glm::vec3 mDirectionalLight;

	std::unique_ptr<Cubemap> mCubemap;
	std::unique_ptr<Shader> mCubemapShader;

	glm::mat4 mModelMat;
	glm::mat4 mView;
	glm::mat4 mProj;


	// shadow mapping vars
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
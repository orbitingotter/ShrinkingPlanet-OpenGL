#pragma once


class Framebuffer
{
public:
	Framebuffer(int width, int height);
	~Framebuffer();

	void Bind() const;
	void Unbind() const;

	void BindColorAttachment(int slot = 0) const;
	void BindDepthStencilAttachment(int slot = 0) const;
private:
	unsigned int mID;
	unsigned int mColorAttachment, mDepthStencilAttachment;
};
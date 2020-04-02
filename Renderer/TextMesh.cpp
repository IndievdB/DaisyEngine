#include "TextMesh.hpp"
#include "../Core/ResourceManager.hpp"

void TextMesh::Render(TextMesh textMesh, float x, float y)
{
	auto mesh = ResourceManager::GetInstance()->GetMesh("Resources/textQuad.obj");
	auto material = ResourceManager::GetInstance()->GetMaterial("TextMat", "Resources/text.shader");
	Matrix4x4 projection = Matrix4x4::Orthographic(0, 800.0f, 0, 600.0f, -10.0f, 10.0f);
	material->GetShader()->Use();
	material->GetShader()->SetMatrix4x4("projection", projection);

	std::string::const_iterator c;
	for (c = textMesh.text.begin(); c != textMesh.text.end(); c++)
	{
		Character ch = textMesh.font->characters[*c];

		float xpos = x + ch.Bearing.x * textMesh.scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * textMesh.scale;
		float w = ch.Size.x * textMesh.scale;
		float h = ch.Size.y * textMesh.scale;

		Vector3 positionVector(xpos + w * 0.5f, ypos + h * 0.5f, 0);
		Vector3 scalingVector(w, h, 0);

		Matrix4x4 model = Matrix4x4::Translation(positionVector) * Matrix4x4::Scaling(scalingVector);

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		material->GetShader()->SetMatrix4x4("model", model);
		material->GetShader()->SetVector3("textColor", textMesh.color.x, textMesh.color.y, textMesh.color.z);
		mesh->Render();

		x += (ch.Advance >> 6) * textMesh.scale;
	}
}
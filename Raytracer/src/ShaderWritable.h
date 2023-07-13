#pragma once

#include <vector>

#include "../shaders/AbstractShader.h"
#include "ImGuiEditorInterface.h"

class ShaderWritable : public ImGuiEditorInterface
{
public:
	bool hasWrittenToShader(AbstractShader* shader)
	{
		for (AbstractShader* pointer : shadersWrittenTo)
		{
			if (pointer == shader)
				return true;
		}

		return false;
	}

	void markShaderAsWrittenTo(AbstractShader* shader)
	{
		shadersWrittenTo.push_back(shader);
	}

	void clearShaderWrittenTo()
	{
		shadersWrittenTo.clear();
	}

protected:
	// Abstract class, no need to instantiate this class
	ShaderWritable() : ImGuiEditorInterface() {}
	virtual ~ShaderWritable() {}

private:
	std::vector<AbstractShader*> shadersWrittenTo;
};


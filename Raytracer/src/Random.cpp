#include "Random.h"

namespace Random
{
	namespace
	{
        unsigned int randomTexture{ 0 };

        const unsigned int RANDOM_TEXTURE_SIZE{ 8192 };
	};

    void generateRandomTexture()
    {
        std::vector<float> randomValues(RANDOM_TEXTURE_SIZE);
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

        for (int i = 0; i < RANDOM_TEXTURE_SIZE; i++)
        {
            randomValues[i] = distribution(generator);
        }

        glGenTextures(1, &randomTexture);
        glBindTexture(GL_TEXTURE_1D, randomTexture);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, RANDOM_TEXTURE_SIZE, 0, GL_RED, GL_FLOAT, randomValues.data());
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    }

    void initialise()
    {
        generateRandomTexture();
    }

    void bindRandomTexture()
    {
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_1D, randomTexture);
    }

    void terminate()
    {
        glDeleteTextures(1, &randomTexture);
    }
};
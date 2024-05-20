#pragma once

#include <vector>

enum NOISE_FUNCTION{
    NOISE_LINEAR,
    NOISE_EASE,
    NOISE_PERLIN,
    NOISE_VORONOI
};

class NoiseGenerator
{
    public:
        NoiseGenerator()
        {
            noiseStartSeg = 4;
            noiseLevels = 4;
            noiseStartFactor = 0.7f;
            noiseFactor = 0.4f;
            noiseFunction = NOISE_PERLIN;
            randSeed = 0;
        }

        void NoiseSetup(unsigned int startSeg, unsigned int level, float startFactor, float factor, NOISE_FUNCTION function)
        {
            noiseStartSeg = startSeg;
            noiseLevels = level;
            noiseStartFactor = startFactor;
            noiseFactor = startFactor;
            noiseFunction = function;
        }

        void SetNoiseLevel(unsigned int level)
        {
            noiseLevels = level;
        }

        void SetNoiseFunction(NOISE_FUNCTION function)
        {
            noiseLevels = function;
        }

        std::vector<std::vector<float> > get2dNoise(unsigned int size_2d_x, unsigned int size_2d_y, float min_x, float max_x, float min_y, float max_y){
            std::vector<std::vector<float> > ret_vec;

            ret_vec.resize(size_2d_y);
            for(unsigned int i = 0; i < size_2d_y; i++){
                ret_vec[i].resize(size_2d_x);
                for(unsigned int j = 0; j < size_2d_x; j++){

                    //makes that the relatives go from 0 to 1
                    float relative_x = (float(i)/(size_2d_x-1));
                    //rel_z not y as y is up
                    float relative_y = (float(j)/(size_2d_y-1));

                    //linear interpolation to find needed position
                    relative_x = (1.0f-relative_x)*min_x+relative_x*max_x;
                    relative_y = (1.0f-relative_y)*min_y+relative_y*max_y;

                    //unsigned int cur_pos = (j*sub_x+i)*3;
                    ret_vec[i][j] = Noise(relative_x, relative_y);
                }
            }
            return ret_vec;
        }

        float getNoiseVal(float pos_x, float pos_y){
            return Noise(pos_x, pos_y);
        }

        float Noise(float x, float y){
            unsigned int segmentation = noiseStartSeg; //2 = squares of 0.5 (1/2) (please be a power of two)
            float noiseVal = 0.0f;
            float amplitude = noiseStartFactor;

            for (unsigned int i = 0; i < noiseLevels; i++){
                switch(noiseFunction){
                case NOISE_LINEAR:
                    //noiseVal = (2.0f*function_frac_linear(x, y, segmentation)-1.0f)*amplitude+val_ret;
                    break;
                case NOISE_EASE:
                    //noiseVal = (2.0f*function_frac_ease(x, y, segmentation)-1.0f)*amplitude+val_ret;
                    break;
                case NOISE_PERLIN:
                    noiseVal = (2.0f * fractionalPerlinNoise(x, y, segmentation) - 1.0f) * amplitude + noiseVal;
                    break;
                case NOISE_VORONOI:
                    //noiseVal = (2.0f*function_frac_voronoi(x, y, segmentation)-1.0f)*amplitude+val_ret;
                    break;
                }
                segmentation *= 2;
                amplitude *= noiseFactor;
            }
            return noiseVal;
        }

    protected:
            unsigned int noiseStartSeg;
            unsigned int noiseLevels;
            float noiseStartFactor;
            float noiseFactor;
            NOISE_FUNCTION noiseFunction;
            int randSeed;

            static const unsigned int MAX_RAND = 0xFFFFFFFF;
    
    float fractionalPerlinNoise(float x, float y, unsigned int segmentation)
    {
        float squareSize = 1.0f / (float)segmentation;

        //Correct x&y to boundaries 0 to 1
        float correctedX = (x + 1.0f) / 2.0f;
        float correctedY = (y + 1.0f) / 2.0f;

        float randVal_00;
        float randVal_01;
        float randVal_10;
        float randVal_11;

        float gradient_00[2];
        float gradient_01[2];
        float gradient_10[2];
        float gradient_11[2];

        calculateRandomValues(correctedX, correctedY, squareSize, &randVal_00, &randVal_01, &randVal_10, &randVal_11);
        calculateGRadientFromRandomValue(randVal_00 * 32, gradient_00);
        calculateGRadientFromRandomValue(randVal_01 * 32, gradient_01);
        calculateGRadientFromRandomValue(randVal_10 * 32, gradient_10);
        calculateGRadientFromRandomValue(randVal_11 * 32, gradient_11);

        float fracX = fmod(correctedX, squareSize) / squareSize;
        float fracY = fmod(correctedX, squareSize) / squareSize;

        float vec_00[2] = {fracX, fracY};
        float vec_01[2] = {fracX, -(1.0f-fracY)};
        float vec_10[2] = { -(1.0f-fracX) , fracY};
        float vec_11[2] = { -(1.0f-fracX), -(1.0f-fracY)};

        float dot00 = vec_00[0] * gradient_00[0] + vec_00[1] * gradient_00[1]; 
        float dot01 = vec_01[0] * gradient_01[0] + vec_01[1] * gradient_01[1]; 
        float dot10 = vec_10[0] * gradient_10[0] + vec_10[1] * gradient_10[1]; 
        float dot11 = vec_11[0] * gradient_11[0] + vec_11[1] * gradient_11[1]; 

        fracX = 6.0f * pow(fracX, 5.0f) - 15.0 * pow(fracX, 4) + 10 * pow(fracX, 3); 
        fracY = 6.0f * pow(fracY, 5.0f) - 15.0 * pow(fracY, 4) + 10 * pow(fracY, 3);

        float pixelX1 = (1.0f - fracX) * dot00 + fracX * dot10;
        float pixelY1 = (1.0f - fracX) * dot01 + fracX * dot11;

        float pixel = (1.0f - fracY) * pixelX1 + fracY * pixelY1;
        pixel += 0.5f;

        return pixel;
    }

    void calculateRandomValues(float x, float y, float size, float *randVal_00, float *randVal_01, float *randVal_10, float *randVal_11)
    {
        float nearest_square_x = x - fmod(x, size);
        float nearest_square_y = y - fmod(y, size);
        float nearest_square_x_2 = x-fmod(x, size) + size;
        float nearest_square_y_2 = y-fmod(y, size) + size;

        unsigned int seed_x = nearest_square_x*1024*11;
        unsigned int seed_y = nearest_square_y*1024*11;
        unsigned int seed_x_2 = nearest_square_x_2*1024*11;
        unsigned int seed_y_2 = nearest_square_y_2*1024*11;

        *randVal_00 = rand2d(seed_x, seed_y)/(float)MAX_RAND;
        *randVal_01 = rand2d(seed_x, seed_y_2)/(float)MAX_RAND;
        *randVal_10 = rand2d(seed_x_2, seed_y)/(float)MAX_RAND;
        *randVal_11 = rand2d(seed_x_2, seed_y_2)/(float)MAX_RAND;
    }

    void calculateGRadientFromRandomValue(unsigned int randVal, float gradient[2])
    {
        if(randVal % 4 == 0)
        {
            gradient[0] = 1.0f;
            gradient[1] = 1.0f;
        }
        else if(randVal % 4 == 1)
        {
            gradient[0] = -1.0f;
            gradient[1] = 1.0f;
        }
        else if(randVal % 4 == 2)
        {
            gradient[0] = 1.0f;
            gradient[1] = -1.0f;
        }
        else if(randVal % 4 == 3)
        {
            gradient[0] = -1.0f;
            gradient[1] = -1.0f;
        }                      
    }

   unsigned int rand(){
      static unsigned int seed = 0;
      unsigned int a = 1664525;
      unsigned int c = 1013904223;
      seed = (a * seed + c);
      return seed;
   }

   //deterministic rand, given a v value
   unsigned int rand(unsigned int v){
      unsigned int a = 1664525;
      unsigned int c = 1013904223;
      return ((a * (v + randSeed) + c));
   }

   unsigned int rand2d(unsigned int x, unsigned int y){
      return rand(y+rand(x));
   }

};


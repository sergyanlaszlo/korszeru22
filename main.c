#include <immintrin.h>

#define ARRSIZEF 2160054 
#define ARRSIZE 4320054 
#define HEADER 54

int main()
{

	FILE *original = fopen("space_2.bmp", "rb");
	FILE *output_image = fopen("space_2_vertical.bmp", "wb+");

	char *header = (char*) malloc(HEADER*sizeof(char));
	float *raw_data = (float*) malloc((ARRSIZE-HEADER)*sizeof (float));
	float *output_data = (float*) malloc((ARRSIZE-HEADER)*sizeof (float));
	float *mask = (float*) malloc((ARRSIZE-HEADER)*sizeof (float));

	int i;
	float even = 0;

	for(i=0; i<(ARRSIZE-HEADER); i++)
	{	//RGB minden pixel 3byte 
		if(i%3==0)
		{
			if(even == 0)
				even = 1;
			else
				even = 0;
		}

		if(even == 0)
			mask[i] = even;
		else
			mask[i] = 0.0/0.0;
	}

	__m256 mm_nan = _mm256_set1_ps(0.0/0.0); //rátesszük a maszkot

	for(i=0; i<HEADER; i++)
	{
		header[i] = fgetc(original); //header beolvas
	}

	for(i=0; i<(ARRSIZE-HEADER); i++)
	{
		raw_data[i] = (float)fgetc(original); //adat beolvas
	}

	for(i=0; i<(ARRSIZE-HEADER); i+=8)
	{
		__m256 mm_in = _mm256_loadu_ps(&(raw_data[i])); //adat tárol
		__m256 mm_mask = _mm256_loadu_ps(&(mask[i])); //maszk tárol

		__m256 mm_out = _mm256_blendv_ps(mm_in, mm_nan, mm_mask); 
		//végeredmény = adat+maszk blendelve

		_mm256_storeu_ps(&(output_data[i]), mm_out);
		//eltároljuk
	}

	header[18] = (char)0x58;
	header[19] = (char)0x02;

	for(i=0; i<HEADER; i++)
	{
		fputc(header[i], output_image);
	}

	for(i=0; i<(ARRSIZE-HEADER); i++)
	{
		//amég számadat írjuk
		if(!isnan(output_data[i]))
			fputc((char)output_data[i], output_image);
	}

	fclose(original);
	fclose(output_image);
	
	FILE *original_2 = fopen("space_2_vertical.bmp", "rb");
	FILE *output_image_2 = fopen("space_2_horizontal", "wb+");
	
	char *header_2 = (char*) malloc(HEADER*sizeof (char));
	unsigned char *raw_data_first = (unsigned char*) malloc(((ARRSIZEF-HEADER)/2)*sizeof (unsigned char));
	unsigned char *raw_data_second = (unsigned char*) malloc(((ARRSIZEF-HEADER)/2)*sizeof (unsigned char));
	unsigned char *output_data_2 = (unsigned char*) malloc(((ARRSIZEF-HEADER)/2)*sizeof (unsigned char));
	i=0;
	even = 0;
	int first = 0;
	int second = 0;

	for(i=0; i<HEADER; i++)
	{
		header_2[i] = fgetc(original_2);
	}
	
	for(i=0; i<(ARRSIZEF-HEADER); i++)
	{
		if(i%1800 == 0)
		{
			if(even == 0)
				even = 1;
			else
				even = 0;
		}
		
		if(even == 1)
		{
			raw_data_first[first] = (unsigned char)fgetc(original_2);
			first++;
		}
		else
		{
			raw_data_second[second] = (unsigned char)fgetc(original_2);
			second++;
		}
	}

	for(i=0; i<((ARRSIZEF-HEADER)/2); i+=24)
	{
		__m256i mm_second_in = _mm256_loadu_si256((__m256i *)&(raw_data_second[i]));
		__m256i mm_first_in = _mm256_loadu_si256((__m256i *)&(raw_data_first[i]));
		
		__m256i mm_out = _mm256_avg_epu8(mm_second_in, mm_first_in);

		_mm256_storeu_si256((__m256i *)&(output_data_2[i]), mm_out);
	}
	
	header_2[22] = (char)0x58;
	header_2[23] = (char)0x02;
	
	for(i=0; i<HEADER; i++) //felépítjük az outputot
	{
		fputc(header_2[i],output_image_2); 
	}

	
	for(i=0; i<((ARRSIZEF-HEADER)/2); i++)
	{
		fputc(output_data_2[i], output_image_2);
	
	}
	fclose(original_2);
	fclose(output_image_2);

	
	FILE *original_3 = fopen("space_1.bmp", "rb");
	FILE *original_4 = fopen("space_2_horizontal", "rb");
	FILE *final_image = fopen("space_new.bmp", "wb+");
	
	char *header_3 = (char*) malloc(HEADER*sizeof (char));
	unsigned char *raw_data_1 = (unsigned char*) malloc(((ARRSIZEF-HEADER)/2)*sizeof (unsigned char));
	unsigned char *raw_data_2 = (unsigned char*) malloc(((ARRSIZEF-HEADER)/2)*sizeof (unsigned char));
	unsigned char *output_data_3 = (unsigned char*) malloc(((ARRSIZEF-HEADER)/2)*sizeof (unsigned char));

	i = 0;

	for(i=0; i<HEADER; i++)
	{
		header_3[i] = fgetc(original_3);
	}
	for(i=0; i<HEADER; i++)
	{
		header_3[i] = fgetc(original_4);
	}

for(i=0; i<ARRSIZEF/2; i++)
	{
			raw_data_1[i] = (unsigned char)fgetc(original_3);
			raw_data_2[i] = (unsigned char)fgetc(original_4);
	}
	

	for(i=0; i<((ARRSIZEF-HEADER)/2); i+=24)
	{
		__m256i mm_1_in = _mm256_loadu_si256((__m256i *)&(raw_data_1[i]));
		__m256i mm_2_in = _mm256_loadu_si256((__m256i *)&(raw_data_2[i]));
		
		__m256i mm_out = _mm256_avg_epu8(mm_1_in, mm_2_in);

		_mm256_storeu_si256((__m256i *)&(output_data_3[i]), mm_out);
	}

	
	for(i=0; i<HEADER; i++)
	{
		fputc(header_3[i],final_image);
	}

	
	for(i=0; i<((ARRSIZEF-HEADER)/2); i++)
	{
		fputc(output_data_3[i], final_image);
	
	}
	fclose(original_3);
	fclose(original_4);
	fclose(final_image);

return 0;

}


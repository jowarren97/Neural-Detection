#include "Arduino.h"
#include "Neuro_Sig.h"
#include "math.h"

Neuro_Sig::Neuro_Sig(int pin_in)
{
  pinMode(pin_in, OUTPUT);
  pin = pin_in;
  fs = 20000;
  min = -5;
  max = 5;
  counter = 0;
  threshold = max;
  last_spike_time = 0;
  isi_rate = 0;
  mean_rate = 0;
}

Neuro_Sig::Neuro_Sig(int pin_in, int fs_in)
{
  pinMode(pin_in, OUTPUT);
  pin = pin_in;
  fs = fs_in;
  min = -5;
  max = 5;
  counter = 0;
  threshold = max;
  last_spike_time = 0;
  isi_rate = 0;
  mean_rate = 0;
}

Neuro_Sig::Neuro_Sig(int pin_in, int fs_in, float min_in, float max_in)
{
  pinMode(pin_in, OUTPUT);
  pin = pin_in;
  fs = fs_in;
  min = min_in;
  max = max_in;
  counter = 0;
  threshold = max;
  last_spike_time = 0;
  isi_rate = 0;
  mean_rate = 0;
}

Neuro_Sig::~Neuro_Sig()
{
	
}

void Neuro_Sig::set_pin(int pin_in)
{
	pinMode(pin_in, OUTPUT);
}

//------------------------------------------------------------------------

void Neuro_Sig::append_buffer(float new_data)
//adds new data in place of oldest (counter keeps track of index of oldest element)
{
	data_buffer[counter] = new_data;
	counter++;
	
	if (counter >= buffer_length) //loop to fill elements at beginning of data buffer
	{
		counter = 0;
	}
}

bool Neuro_Sig::detect_spike()
{
	if (counter > 1)
	{
		if ((data_buffer[counter-1] > threshold)&&(data_buffer[(counter-2)] <= threshold))
		//if ((data_buffer[counter]> threshold))
		{
			//spike_train[counter] = 1;
			return true;
		}
		else
		{
			//spike_train[counter] = 0;
			return false;
		}
	}
	
	if (counter == 1)
	{
		if ((data_buffer[0] > threshold)&&(data_buffer[buffer_length-1] <= threshold))
		//if ((data_buffer[counter]> threshold))
		{
			//spike_train[counter] = 1;
			return true;
		}
		else
		{
			//spike_train[counter] = 0;
			return false;
		}
	}
	if (counter == 0)
	{
		if ((data_buffer[buffer_length-1] > threshold)&&(data_buffer[buffer_length-2] <= threshold))
		//if ((data_buffer[counter]> threshold))
		{
			//spike_train[counter] = 1;
			return true;
		}
		else
		{
			//spike_train[counter] = 0;
			return false;
		}
	}
}

void Neuro_Sig::get_bins()
{
	float bin_width = (max - min)/no_bins;
	float spike_count = 0;
	
	for (int i=0; i < no_bins; i++)
	{
		spike_count = 0;
		
		for (int j=0; j < buffer_length; j++)
		{
			if ((data_buffer[j] >= min+(i*bin_width))&&(data_buffer[j] < min+(i+1)*bin_width))
			{
				//spike_count++;
			}
		}
		
		bin_weight[i] = spike_count;
	}
}

void Neuro_Sig::gaussian_smooth(float sigma, int numsteps)
{
    double lambda, dnu;
    float nu, boundaryscale, postscale;
    long i;
    int step;
    
    if(!bin_weight || no_bins < 1 || sigma <= 0 || numsteps < 0)
        return;
    
    lambda = (sigma*sigma)/(2.0*numsteps);
    dnu = (1.0 + 2.0*lambda - sqrt(1.0 + 4.0*lambda))/(2.0*lambda);
    nu = (float)dnu;
    boundaryscale = (float)(1.0/(1.0 - dnu));
    postscale = (float)(pow(dnu/lambda,numsteps));
    
    for(step = 0; step < numsteps; step++)
    {
        bin_weight[0] *= boundaryscale;
        
        /* Filter rightwards (causal) */
        for(i = 1; i < no_bins; i++)
		{
            bin_weight[i] += nu * bin_weight[i - 1];
        }
        bin_weight[i = no_bins - 1] *= boundaryscale;
        
        /* Filter leftwards (anti-causal) */
        for(; i > 0; i--)
		{
            bin_weight[i - 1] += nu*bin_weight[i];
		}
    }

    
    for(i = 0; i < no_bins; i++)
        bin_weight[i] *= postscale;
    
    return;
}

void Neuro_Sig::find_threshold()
{
	int i = no_bins/2;
	int min_idx = 0;
	
	while (i > 0)
	{
		if ((bin_weight[i] < bin_weight[i-1])&&(bin_weight[i] < bin_weight[i+1]))
		{
			min_idx = i;
			break;
		}		
		i--;
	}
	
	if (min_idx > 0)
	{
		threshold = max - min_idx*(max-min)/no_bins;
	}
}

void Neuro_Sig::set_isi_rate()
{
	int current_time = millis();
	isi_rate = 1000/(current_time - last_spike_time);
	last_spike_time = current_time;
}

void Neuro_Sig::set_mean_rate(float period)
{
	
}

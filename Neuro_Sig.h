#ifndef Neuro_Sig_h
#define Neuro_Sig_h

#include "Arduino.h"

class Neuro_Sig
{
  private:
	/*Q1: do i have too many private variables? Means i need a lot more functions just to access them in 
	arduino main code(see Q2), could just assign to global variables in arduino code? Private maybe 
	nicer/more complete as all info about signal stored compactly within class. Personally I prefer private 
	solution but wanted your advice*/
	
	int counter; //counter represents where to input new data into data_buffer
   	int pin; //analog input pin for signal
    	int fs; //sampling frequency
	float min; //min voltage value
	float max; //max voltage value
	float threshold; //threshold for spike detection
	int last_spike_time;
	float isi_rate; //inter spike interval
	float mean_rate; //over time 'float period' specified in function 'void set_mean_rate'
	
    	float data_buffer[5000]; //buffer full of most recent data, used for threshold calculation
	int spike_train[5000];
    	float bin_weight[1000]; //histogram of spike amplitudes
	const int buffer_length = sizeof(data_buffer)/sizeof(*data_buffer);
	const int no_bins = sizeof(bin_weight)/sizeof(*bin_weight);	
	
	
  public:
    	Neuro_Sig(int pin_in);
    	Neuro_Sig(int pin_in, int fs_in);
    	Neuro_Sig(int pin_in, int fs_in, float min_in, float max_in);
    	~Neuro_Sig();
	
	
	/*Q2: should i delete these functions for more simplicity or keep for more user control?*/
	
	void set_counter(int counter_in){counter = counter_in;}
	int get_counter(){return counter;}
	void set_pin(int pin_in);
	int get_pin(){return pin;}
	void set_fs(int fs_in){fs = fs_in;}
	int get_fs(){return fs;}
	void set_min(float min_in){min = min_in;}
	float get_min(){return min;}
	void set_max(float max_in){max = max_in;}
	float get_max(){return max;}
	void set_threshold(float threshold_in){threshold = threshold_in;}
	float get_threshold(){return threshold;}
	//void set_isi_rate(); see below
	float get_isi_rate(){return isi_rate;}
	//void set_mean_rate(); see below
	float get_mean_rate(){return mean_rate;}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	
	
	/*Q3: should these functions be void (i.e assign values to private variables e.g threshold, isi_rate) 
	or return int and then assign to global variables in arduino main code (i.e no need for private variables
	and therefore functions such as get_isi_rate)*/
	
	void append_buffer(float new_data); //updates oldest buffer element with most recent data value
	bool detect_spike();
	void get_bins();
	void gaussian_smooth(float sigma, int numsteps); //sigma = std deviation, numsteps = iterations
	void find_threshold(); 
	void set_isi_rate();
	void set_mean_rate(float period); //period in milliseconds
	
	
	/*Q4: trying to think of an efficient way to count spikes over time period to calculate mean_rate. Storing spikes  
	in an array (spike_train) and then counting whole array seems inefficient. Trying to think of some way to count spikes
	over time period without needing to record and read large data array (i.e large processing cost)*/
};


#endif

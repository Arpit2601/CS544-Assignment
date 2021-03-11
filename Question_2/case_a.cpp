#include<bits/stdc++.h>

using namespace std;


struct Timing // queue entering time, queue leaving time, system leaving time
{ 
   double x;
   double y;
   double z; 
};


int main(){

    double lambda_1;
    double mu_1;



    cout << "Enter the lambda value(incoming rate for each queue) in person/second: " << endl;
    cin >> lambda_1;

    cout << "Enter the mu value(processing rate of each officer) in person/second: " << endl;
    cin >> mu_1;
    

    if(lambda_1 <= 0){
        cout << "Enter valid lambda value" << endl;
        return(0);
    }

    if(mu_1 <= 0){
        cout << "Enter valid mu value" << endl;
        return(0);
    }

    if(lambda_1/mu_1 >= 1){
        cout << "Queue stability condition is violated. Enter valid lambda, mu values" << endl;
        return(0);
    }


    std::random_device rd; 
    std::mt19937 rnd_gen (rd ());

    exponential_distribution<> lambda_generator_1 (lambda_1);
    exponential_distribution<> lambda_generator_2 (lambda_1);
    vector<exponential_distribution<>> lambda_generator = {lambda_generator_1, lambda_generator_2};

    exponential_distribution<> mu_generator_1 (mu_1);
    exponential_distribution<> mu_generator_2 (mu_1);
    vector<exponential_distribution<>> mu_generator = {mu_generator_1, mu_generator_2};

    


    double current_time = 0;
    long long completed_migrants = 0;


    vector<double> next_incoming_timing = {current_time + lambda_generator[0](rnd_gen), current_time + lambda_generator[1](rnd_gen)};
    vector<int> processing_state = {0, 0};
    vector<double> processing_complete_timing = {current_time, current_time};


    vector<double> avg_number_of_migrants_getting_checked = {0, 0};
    vector<double> avg_response_time = {0, 0};
    vector<double> avg_time_for_migrant_in_queue = {0, 0};
    vector<double> avg_number_of_migrant_in_queue = {0, 0};
    long long total_migrants = 1000000;
    vector<long long> migrants = {0, 0};


    vector<queue<Timing>> waiting_q = {queue<Timing> (), queue<Timing> ()};
    vector<Timing> currently_processing = {{0, 0, 0}, {0, 0, 0}};




    while(completed_migrants < total_migrants){


        vector<double> times;

        for(int i = 0; i < 2; i++){
            times.push_back(next_incoming_timing[i]);
        }


        for(int i = 0; i < 2; i++){
            if (processing_state[i] == 1){
                times.push_back(processing_complete_timing[i]);
            }
            else{
                times.push_back(DBL_MAX);
            }
        }
        
        int min_index = 0;
        for(int i = 1; i < 4; i++){
            if(times[i] < times[min_index]){
                min_index = i;
            }
        }

        if(min_index < 2){
            // We have an incoming migrant
            min_index = min_index % 2;
            current_time = next_incoming_timing[min_index];

            if(processing_state[min_index] == 0){  // The server is empty, directly give the migrant to server
                currently_processing[min_index] = {current_time, current_time, 0};
                processing_state[min_index] = 1;
                processing_complete_timing[min_index] = current_time + mu_generator[min_index](rnd_gen);
            }
            else{  // The server is busy,  deposit the migrant to the queue
                waiting_q[min_index].push({current_time, 0, 0});
            }

            next_incoming_timing[min_index] = current_time + lambda_generator[min_index](rnd_gen);
        }
        else{
            // We have an outgoing migrant
            min_index = min_index % 2;
            current_time = processing_complete_timing[min_index];
            currently_processing[min_index].z = current_time;

            avg_number_of_migrants_getting_checked[min_index] += (currently_processing[min_index].z - currently_processing[min_index].y);
            avg_response_time[min_index] += (currently_processing[min_index].z - currently_processing[min_index].x);
            avg_time_for_migrant_in_queue[min_index] += (currently_processing[min_index].y - currently_processing[min_index].x); 
            avg_number_of_migrant_in_queue[min_index] += (currently_processing[min_index].y - currently_processing[min_index].x);

            migrants[min_index]++;
            completed_migrants++;
            if(completed_migrants == total_migrants){
                break;
            }

            if(waiting_q[min_index].empty()){  // Q is empty change the processing state to 1
                processing_state[min_index] = 0;
            }
            else{
                currently_processing[min_index] = waiting_q[min_index].front();
                waiting_q[min_index].pop();
                currently_processing[min_index].y = current_time;
                processing_complete_timing[min_index] = current_time + mu_generator[min_index](rnd_gen);
            }


        }

    }


    for(int i = 0; i < 2; i++){

        avg_number_of_migrants_getting_checked[i] /= current_time;
        avg_response_time[i] /= migrants[i];
        avg_time_for_migrant_in_queue[i] /= migrants[i];
        avg_number_of_migrant_in_queue[i] /= current_time;

    }


    ofstream outfile;
    outfile.open("case_a.txt");


    for(int i = 0; i < 2; i++){
        cout << "Values for Queue " << to_string(i+1) << endl;
        cout << "Average number of workers getting checked: ";
        cout << avg_number_of_migrants_getting_checked[i] << endl;
        cout << "Average response time for workers in getting checked: ";
        cout << avg_response_time[i] << endl;
        cout << "Average time for which a worker has to wait until getting checked: ";
        cout << avg_time_for_migrant_in_queue[i] << endl;
        cout << "Average number of workers waiting in thequeue before each officer: ";
        cout << avg_number_of_migrant_in_queue[i] << endl;


        outfile << "Values for Queue " << to_string(i+1) << endl;
        outfile << "Average number of workers getting checked: ";
        outfile << avg_number_of_migrants_getting_checked[i] << endl;
        outfile << "Average response time for workers in getting checked: ";
        outfile << avg_response_time[i] << endl;
        outfile << "Average time for which a worker has to wait until getting checked: ";
        outfile << avg_time_for_migrant_in_queue[i] << endl;
        outfile << "Average number of workers waiting in thequeue before each officer: ";
        outfile << avg_number_of_migrant_in_queue[i] << endl;


    }

    outfile.close();
    return(0);



}
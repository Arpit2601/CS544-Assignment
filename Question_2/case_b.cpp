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



    cout << "Enter the lambda_1 value in rate/second: " << endl;
    cin >> lambda_1;

    cout << "Enter the mu_1 value in rate/second: " << endl;
    cin >> mu_1;
    

    std::random_device rd; 
    std::mt19937 rnd_gen (rd ());

    exponential_distribution<> lambda_generator (lambda_1);
    exponential_distribution<> mu_generator (mu_1);
    

    double current_time = 0;
    long long completed_migrants = 0;


    double next_incoming_timing = current_time + lambda_generator(rnd_gen);
    int processing_state_1 = 0;
    int processing_state_2 = 0;
    double processing_complete_timing_1 = current_time;
    double processing_complete_timing_2 = current_time;



    double avg_number_of_migrants_getting_checked = 0;
    double avg_response_time = 0;
    double avg_time_for_migrant_in_queue = 0;
    double avg_number_of_migrant_in_queue = 0;
    long long total_migrants = 1000000;

    queue<Timing> waiting_q;

    Timing currently_processing_1;
    Timing currently_processing_2;




    while(completed_migrants < total_migrants){

        if((processing_state_1 == 0) && (processing_state_2 == 0)){
            // The border agent is sitting empty, directly move to the time when the next incoming migrant will come
            current_time = next_incoming_timing;

            currently_processing_1 = {current_time, current_time, 0};

            next_incoming_timing = current_time + lambda_generator(rnd_gen);
            processing_complete_timing_1 = current_time + mu_generator(rnd_gen);
            processing_state_1 = 1;        

        }
        else if((processing_state_1 == 0) && (processing_state_2 == 1)){


        }
        else if((processing_state_1 == 1) && (processing_state_2 == 0)){



        }
        else{



            // The border agent is not empty
            if (next_incoming_timing <= processing_complete_timing){
            // Next migrant is coming
                current_time = next_incoming_timing;
                next_incoming_timing = current_time + lambda_generator(rnd_gen);
                waiting_q.push({current_time, 0, 0});
            }
            else{
            // Migrant is getting processed and leaving
                current_time = processing_complete_timing;
                currently_processing.z = current_time;

                avg_number_of_migrants_getting_checked += (currently_processing.z - currently_processing.y);
                avg_response_time += (currently_processing.z - currently_processing.x);
                avg_time_for_migrant_in_queue += (currently_processing.y - currently_processing.x); 
                avg_number_of_migrant_in_queue += (currently_processing.y - currently_processing.x);
                completed_migrants += 1;

                if (completed_migrants == total_migrants) break;

                // Bring the next one from the queue;
                if(waiting_q.empty()){  // Witing Queue is empty
                    processing_state = 0;
                }
                else{  
                    currently_processing =  waiting_q.front();
                    waiting_q.pop();
                    currently_processing.y = current_time;
                    processing_complete_timing = current_time + mu_generator(rnd_gen);
                }

            }
        }
    }


    avg_number_of_migrants_getting_checked /= current_time;
    avg_response_time /= total_migrants;
    avg_time_for_migrant_in_queue /= total_migrants;
    avg_number_of_migrant_in_queue /= current_time;

    cout << avg_number_of_migrants_getting_checked << endl;
    cout << avg_response_time << endl;
    cout << avg_time_for_migrant_in_queue << endl;
    cout << avg_number_of_migrant_in_queue << endl;




}
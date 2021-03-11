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



    cout << "Enter the lambda(incoming rate for the common queue) value in rate/second: " << endl;
    cin >> lambda_1;

    cout << "Enter the mu(processing rate of each officer) value in rate/second: " << endl;
    cin >> mu_1;

    if(lambda_1 <= 0){
        cout << "Enter valid lambda value" << endl;
        return;
    }

    if(mu_1 <= 0){
        cout << "Enter valid mu value" << endl;
        return;
    }

    if(lambda_1/(2*mu_1) >= 1){
        cout << "Queue stability condition is violated. Enter valid lambda, mu values" << endl;
        return;
    }
    

    random_device rd; 
    mt19937 rnd_gen (rd ());

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
            if (next_incoming_timing <= processing_complete_timing_2){
                // give this worker to first guy

                current_time = next_incoming_timing;

                currently_processing_1 = {current_time, current_time, 0};

                next_incoming_timing = current_time + lambda_generator(rnd_gen);
                processing_complete_timing_1 = current_time + mu_generator(rnd_gen);
                processing_state_1 = 1;

            }
            else{

                current_time = processing_complete_timing_2;
                currently_processing_2.z = current_time;

                avg_number_of_migrants_getting_checked += (currently_processing_2.z - currently_processing_2.y);
                avg_response_time += (currently_processing_2.z - currently_processing_2.x);
                avg_time_for_migrant_in_queue += (currently_processing_2.y - currently_processing_2.x); 
                avg_number_of_migrant_in_queue += (currently_processing_2.y - currently_processing_2.x);
                completed_migrants += 1;

                if (completed_migrants == total_migrants) break;
                processing_state_2 = 0;
            }
        }
        else if((processing_state_1 == 1) && (processing_state_2 == 0)){
            if (next_incoming_timing <= processing_complete_timing_1){
                // give this worker to first guy

                current_time = next_incoming_timing;

                currently_processing_2 = {current_time, current_time, 0};

                next_incoming_timing = current_time + lambda_generator(rnd_gen);
                processing_complete_timing_2 = current_time + mu_generator(rnd_gen);
                processing_state_2 = 1;

            }
            else{

                current_time = processing_complete_timing_1;
                currently_processing_1.z = current_time;

                avg_number_of_migrants_getting_checked += (currently_processing_1.z - currently_processing_1.y);
                avg_response_time += (currently_processing_1.z - currently_processing_1.x);
                avg_time_for_migrant_in_queue += (currently_processing_1.y - currently_processing_1.x); 
                avg_number_of_migrant_in_queue += (currently_processing_1.y - currently_processing_1.x);
                completed_migrants += 1;

                if (completed_migrants == total_migrants) break;
                processing_state_1 = 0;
            }


        }
        else{



            // The border agent is not empty
            if (next_incoming_timing <= min(processing_complete_timing_2, processing_complete_timing_1)){
            // Next migrant is coming
                current_time = next_incoming_timing;
                next_incoming_timing = current_time + lambda_generator(rnd_gen);
                waiting_q.push({current_time, 0, 0});
            }
            else if(processing_complete_timing_2 == min(processing_complete_timing_2, processing_complete_timing_1)){
                current_time = processing_complete_timing_2;
                currently_processing_2.z = current_time;

                avg_number_of_migrants_getting_checked += (currently_processing_2.z - currently_processing_2.y);
                avg_response_time += (currently_processing_2.z - currently_processing_2.x);
                avg_time_for_migrant_in_queue += (currently_processing_2.y - currently_processing_2.x); 
                avg_number_of_migrant_in_queue += (currently_processing_2.y - currently_processing_2.x);
                completed_migrants += 1;

                if (completed_migrants == total_migrants) break;

                // Bring the next one from the queue;
                if(waiting_q.empty()){  // Witing Queue is empty
                    processing_state_2 = 0;
                }
                else{  
                    currently_processing_2 =  waiting_q.front();
                    waiting_q.pop();
                    currently_processing_2.y = current_time;
                    processing_complete_timing_2 = current_time + mu_generator(rnd_gen);
                }   

            }
            else{
                current_time = processing_complete_timing_1;
                currently_processing_1.z = current_time;

                avg_number_of_migrants_getting_checked += (currently_processing_1.z - currently_processing_1.y);
                avg_response_time += (currently_processing_1.z - currently_processing_1.x);
                avg_time_for_migrant_in_queue += (currently_processing_1.y - currently_processing_1.x); 
                avg_number_of_migrant_in_queue += (currently_processing_1.y - currently_processing_1.x);
                completed_migrants += 1;

                if (completed_migrants == total_migrants) break;

                // Bring the next one from the queue;
                if(waiting_q.empty()){  // Witing Queue is empty
                    processing_state_1 = 0;
                }
                else{  
                    currently_processing_1 =  waiting_q.front();
                    waiting_q.pop();
                    currently_processing_1.y = current_time;
                    processing_complete_timing_1 = current_time + mu_generator(rnd_gen);
                } 

            }
        }
    }


    avg_number_of_migrants_getting_checked /= current_time;
    avg_response_time /= total_migrants;
    avg_time_for_migrant_in_queue /= total_migrants;
    avg_number_of_migrant_in_queue /= current_time;

    cout << "Average number of workers getting checked: ";
    cout << avg_number_of_migrants_getting_checked << endl;
    cout << "Average response time for workers in getting checked: ";
    cout << avg_response_time << endl;
    cout << "Average time for which a worker has to wait until getting checked: ";
    cout << avg_time_for_migrant_in_queue << endl;
    cout << "Average number of workers waiting in thequeue before each officer: ";
    cout << avg_number_of_migrant_in_queue << endl;



}
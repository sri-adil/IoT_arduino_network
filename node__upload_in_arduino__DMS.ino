#include <SPI.h>

#include <RH_RF95.h>

#include <SoftwareSerial.h>
#include <TinyGPS.h>

#define RF95_FREQ 865.0

RH_RF95 rf95;

SoftwareSerial gpsSerial(5,6);//rx,tx
TinyGPS gps; // create gps object
float lat ,lon ;

String message = "";
bool messageReady = false;

// =================  id ====================
int const node_id = 0;
int const network_id = 143;
int const broadcast_id = 254;
int const null_int = 255;  // represents no data
// =================  id ====================


// id of the disaster management team 

int const DMS = 2;


//=================== msg length defination =====================
int const no_nodes = 5;
int const vector_len = no_nodes + 4;
int const ack_len = no_nodes + 5;
// array len = network_id + type + from + to + distance_vector

//=================== msg length defination =====================



//=================== msg type defination =====================
int const distance_vector_type = 1;
int const data_type = 2;
int const ack_type = 3;
int const intermediate_msg = 4;
//=================== msg type defination =====================

//=================== msg state defination =====================
int pending[no_nodes] = {0};
//=================== msg state defination =====================

uint8_t indatabuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(indatabuf);


int distance_vector[no_nodes];
int next_hop[no_nodes]={null_int};


int recvd_distance_vector[vector_len]; // = network_id + type + from + to + distance_vector
int recvd_ack[ack_len]; // = network_id + type + from + to + actuall_to  + distance_vector

uint8_t send_distance_vector[vector_len]; // used for distance vector only    = network_id + type + from + to + distance_vector
uint8_t send_ack[ack_len]; // = network_id + type + from + to + actuall_to  + distance_vector

String msg_body;
uint8_t temp_out_msg_data[95];  // used for sneding data_type msgs
uint8_t out_msg_data[100];
uint8_t in_msg_data[95];
    
void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Serial.println("Arduino LoRa TX Test!");

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  Serial.println("================================================");

  for(int i = 0;i < no_nodes ; i++)
  {
    distance_vector[i] = 255;
    if(i == node_id)
    {
      distance_vector[i] = 0;
    }
  }

}

void print_msg(int type)
{
  if(type == distance_vector_type)
  {    
    Serial.print("recived distance vector ");
    for(int i =0 ; i < vector_len; i++)
    {
      Serial.print(recvd_distance_vector[i]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  else if(type == ack_type)
  {
    Serial.print("recived ack ");
    for(int i =0 ; i < ack_len; i++)
    {
      Serial.print(recvd_ack[i]);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  else if(type == data_type)
  {
    Serial.print("recvd data ");
    Serial.print(indatabuf[0]);
    Serial.print(" ");
    Serial.print(indatabuf[1]);
    Serial.print(" ");
    Serial.print(indatabuf[2]);
    Serial.print(" ");
    Serial.print(indatabuf[3]);
    Serial.print(" ");
    Serial.print(indatabuf[4]);
    Serial.print(" ");
    Serial.println((char*)in_msg_data);
  }
  else
  {
    Serial.println("no mesgs  ");
  }
  Serial.println("=====================================================");
}

//returns the message type if msg recv is successful or it returns 0 otherwise
int recv_msg()
{
  if (rf95.waitAvailableTimeout(300))
     {    
      if (rf95.recv(indatabuf, &len))
      {
         if((int)indatabuf[0] == network_id && ((int)indatabuf[3] == broadcast_id || (int)indatabuf[4] == node_id))
         {
            if((int)indatabuf[1] == distance_vector_type)
            {
              for(int i = 0; i<vector_len ; i++)
              {
                recvd_distance_vector[i] = (int)indatabuf[i];
              }
              return distance_vector_type;
            }

            if((int)indatabuf[3] == (int)indatabuf[4])
            {
                if((int)indatabuf[1] == ack_type)
                {
                  for(int i = 0; i<ack_len ; i++)
                  {
                    recvd_ack[i] = (int)indatabuf[i];
                  }
                  return ack_type;
                }
                else if((int)indatabuf[1] == data_type)
                {
                  for(int i = 5 ; i< 100 ; i++)
                  {
                    in_msg_data[i-5] = indatabuf[i];
                  }
                  return data_type;
                }
            }
         }
        else if ((int)indatabuf[3] == node_id)
        {
          return intermediate_msg;
        }
        else
        {
           return 0;
        }
      }
      else
      {
        return 0;
      }
    }
    else
    {
      return 0;
    }
}

void send_msg(int msg_type, int to_id, int actuall_to_id,String msg_body)
{

  if(msg_type == distance_vector_type)
  {
    send_distance_vector[0] = network_id;
    send_distance_vector[1] = msg_type; 
    send_distance_vector[2] = node_id;
    send_distance_vector[3] = broadcast_id;

    for(int i = 4 ; i < vector_len ;i++)
    {
      send_distance_vector[i] = distance_vector[i-4];
    }
    rf95.send(send_distance_vector,sizeof(send_distance_vector));  
  }
  else if (msg_type == ack_type)
  {
      send_ack[0] = network_id;
      send_ack[1] = msg_type; 
      send_ack[2] = node_id;
      send_ack[3] = to_id;
      send_ack[4] = actuall_to_id;  
      for(int i = 5 ; i < ack_len ;i++)
      {
        send_distance_vector[i] = null_int;
      }
      rf95.send(send_ack,sizeof(send_ack));  
  }
  else if (msg_type == data_type)
  {
      msg_body.toCharArray(temp_out_msg_data,95);
      out_msg_data[0] = network_id;
      out_msg_data[1] = msg_type; 
      out_msg_data[2] = node_id;
      out_msg_data[3] = to_id;
      out_msg_data[4] = actuall_to_id;
      for(int i = 5 ; i < 100 ;i++)
      {
        out_msg_data[i] = temp_out_msg_data[i-5];
      }
      rf95.send(out_msg_data,sizeof(out_msg_data));
      pending[actuall_to_id] ++;
  }
  
  delay(100);
}



// this function updates the distance vector according to recvd distance vector 
// this function returns 1 if any changes are there else it retuns 0 
int update_distance_vector()
{
   int updates = 0;
   for(int i = 0; i< no_nodes ; i++)
   {
      if(distance_vector[i] > recvd_distance_vector[i+4] + 1)
      {
          distance_vector[i] = recvd_distance_vector[i+4] + 1;
          next_hop[i] = recvd_distance_vector[2];
          updates ++;
      }
   }
   if(updates == 0)
   {
      return 0;
   }
   else
   {
      return 1;
   }
}


// this function acts according to the type of the message received
void action_center(int num)
{

    if(num == distance_vector_type)
    {
      int temp = update_distance_vector();
      if(temp == 1)
      {
        // add random delay
        send_msg(num,broadcast_id,broadcast_id,"");
      }
    }
    else if(num == ack_type)
    {
      pending[recvd_ack[2]] = pending[recvd_ack[2]] - 1;
    }
    else if(num == data_type)
    {
    }
    else if(num == intermediate_msg)
    {
      Serial.println("recv intermediate ");
      indatabuf[3] = next_hop[(int)indatabuf[4]];
      rf95.send(indatabuf,sizeof(indatabuf));
    }
    print_msg(data_type);
}
void loop() {
  // put your main code here, to run repeatedly:

    send_msg(distance_vector_type,0,0,message+"");
    
    while(Serial.available()) {
    
    message = Serial.readString();
    messageReady = true;
    }
  // Only process message if there's one
  if(messageReady) {
    if (message[0] == '#' && message[1] == '#')
    {
      Serial.println(message);
      Serial.println("sending to node 1");
      delay(100);
      String latitude ;
      String longitude ;
      String position_est;
      while(gpsSerial.available()){ // check for gps data
        
        if(gps.encode(gpsSerial.read()))// encode gps data
        {
            gps.f_get_position(&lat,&lon); // get latitude and longitude
            // display position
            
            latitude = String(lat,6);
            longitude = String(lon,6);
            position_est = latitude+" ; "+longitude;
            
        }
    }


      send_msg(data_type,next_hop[DMS],DMS,message+"$$ ; "+position_est);
      Serial.println(message+" ; "+position_est);
    }
    messageReady = false;
  }

  int action  = recv_msg();
  action_center(action);
  
  delay(100);
}

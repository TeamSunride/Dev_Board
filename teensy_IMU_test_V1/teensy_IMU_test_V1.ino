/* Auther:Abdel Alomari Date:16 june 2020
   Full Serial monitored a
   vionics code 1.0.0
   ADXL377 (high G accel)+ BMX160 (Low G accel + gyro + mag)+ BMP388 (Pressure + temp)
   Check Libraries
 */


#include <DFRobot_BMX160.h>
#include <DFRobot_BMP388.h>
#include <DFRobot_BMP388_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <bmp3_defs.h>
#include <SD.h>
#include <SPI.h>

DFRobot_BMX160 bmx160;
DFRobot_BMP388_I2C bmp388;
File dataFile;

String Raw_data();

const int led = 13, rled = 14 , wled = 15, gled = 16, Rled = 17, chipSelect = BUILTIN_SDCARD;
int  i = 0;
float x,y,z;
char  F_name[80] = "datalog.txt", F[80]= "datalog";
String dataString = "";
String str;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(rled, OUTPUT);
  pinMode(wled, OUTPUT);
  pinMode(gled, OUTPUT);
  pinMode(Rled, OUTPUT);
  pinMode(32, INPUT);
  pinMode(A14, INPUT);
  pinMode(A15, INPUT);
  pinMode(A16, INPUT);
  digitalWrite(rled,HIGH);
  digitalWrite(wled,HIGH);
  digitalWrite(gled,HIGH);
  digitalWrite(Rled,LOW);
  Serial.begin(9600);
  delay(20);
  
  
  while(!Serial){digitalWrite(led, (((millis()/100)%10) < 1) || (((millis()/100)%10) >= 2 && ((millis()/100)%10) <= 4));}
  sys_init_sequence();

}

void loop() {
  digitalWrite(led, ((millis()/2000)%10) < 7);
  while (!Serial){digitalWrite(led, (((millis()/100)%10) < 1) || (((millis()/100)%10) >= 2 && ((millis()/100)%10) <= 4));}

  if(((micros()%10000)<2)){
    str = String(F_name);
    
    dataFile = SD.open( str.c_str() , FILE_WRITE);

    dataString =  String(micros()) + "," + Raw_data();
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      Serial.println(dataString);
    }   
    else {
      Serial.println("error opening " + String(F_name));
    } 
  }
}
/*===========================================Extracting raw data string========================================*/

String Raw_data(){
  String Data = "" ;
  bmx160SensorData Omagn, Ogyro, Oaccel;
  /* Get a new sensor event */
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);
  
  //Serial.println("Ax Ay Az Mx My Mz Gx Gy Gz ax ay az Temp cAlt Alt Pr");
  float Haccel_x = float(analogRead(A14))* 400.0/1023.0-200.0; //map((analogRead(A14)),0,1023,-200,200);
  float Haccel_y = float(analogRead(A15))* 400.0/1023.0-200.0; //map((analogRead(A15)),0,1023,-200,200);
  float Haccel_z = float(analogRead(A16))* 400.0/1023.0-200.0; //map((analogRead(A16)),0,1023,-200,200);
  Data = {
    String(Haccel_x)+ "," + String(Haccel_y)+ "," +String(Haccel_z)+ "," +
    String(Oaccel.x)+ "," + String(Oaccel.y)+ "," +String(Oaccel.z)+ "," +
    String(Ogyro.x) + "," + String(Ogyro.y) + "," +String(Ogyro.z) + "," +
    String(Omagn.x) + "," + String(Omagn.y) + "," +String(Omagn.z) + ","
  };
  Data += {
    String(bmp388.readCalibratedAltitude(bmp388.readSeaLevel(525.0)))+ "," +
    String(bmp388.readAltitude()) + "," + String(bmp388.readPressure())
  };
  return Data;
  }

/*===========================================system initiation sequence======================================*/
void sys_init_sequence(){
  /*Initialize bmx 160*/
  if (bmx160.begin() != true){
    Serial.println("bmx160 init false");
    while(1){digitalWrite(led, ((millis()/20)%10) < 2);};
  }
  bmx160.setGyroRange(eGyroRange_1000DPS);
  bmx160.setAccelRange(eAccelRange_16G);
  delay(20);
  
  /*Initialize bmp388*/
  bmp388.set_iic_addr(BMP3_I2C_ADDR_PRIM);
  if(bmp388.begin()){
    Serial.println("bmp388 Initialize error!");
    while(1){digitalWrite(led, ((millis()/20)%10) < 5);}
  }
  delay(20);

 
  Serial.print("seaLevel : ");Serial.print(bmp388.readSeaLevel(525.0));Serial.println(" Pa");
  // wait for serial port to connect.
  while (!Serial);
  
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) { 
    Serial.println("Card failed, or not present");
  // don't do anything more:
    return;
  }
  
  // Creating new file
  while (SD.exists(F_name)){
      Serial.print(String(F_name));
    if(String(F_name).length()< 11){
      Serial.print(" Exists");
      str = String(i);
      strcat( F ,  str.c_str());
      strcpy(F_name ,F);
      strcat( F_name ,  ".txt");
      i++;
    }else{strcpy(F ,"dl");strcpy(F_name ,"dl.txt");}
    Serial.println(" ");
  }
  Serial.println("card initialized. using file: "+ String(F_name));

}

/*================================================Serial show case of values=====================================*/

void serial_CaseShow(){
  bmx160SensorData Omagn, Ogyro, Oaccel;

  /* Get a new sensor event */
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);

  /* Display the magnetometer results (magn is magnetometer in uTesla) */
  Serial.print("M ");
  Serial.print("X: "); Serial.print(Omagn.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(Omagn.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(Omagn.z); Serial.print("  ");
  Serial.println("uT");

  /* Display the gyroscope results (gyroscope data is in g) */
  Serial.print("G ");
  Serial.print("X: "); Serial.print(Ogyro.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(Ogyro.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(Ogyro.z); Serial.print("  ");
  Serial.println("g");
  
  /* Display the accelerometer results (accelerometer data is in m/s^2) */
  Serial.print("A ");
  Serial.print("X: "); Serial.print(Oaccel.x    ); Serial.print("  ");
  Serial.print("Y: "); Serial.print(Oaccel.y    ); Serial.print("  ");
  Serial.print("Z: "); Serial.print(Oaccel.z    ); Serial.print("  ");
  Serial.println("m/s^2");



  Serial.print("Temperature : "); 
  Serial.print(bmp388.readTemperature()); Serial.println(" C");

  Serial.print("calibrate Altitude : ");
  Serial.print(bmp388.readCalibratedAltitude(bmp388.readSeaLevel(525.0))); Serial.println(" m");
  Serial.print("Altitude : ");
  Serial.print(bmp388.readAltitude()); Serial.println(" m");

  Serial.print("Pressure : ");
  Serial.print(bmp388.readPressure());
  Serial.println(" Pa");

  Serial.println("");
}

/*=====================================Archived code=================================================*/

//  Basic Log 
//  if(((micros()%1000000)<3)){// cycle width = 1 second, pulse width = 3 microseconds
//  Serial.print(String("time ") + (micros()/1000000));
//  Serial.print(String(" ,botton: ") + (digitalRead(32)));
//  Serial.print(String(" ,micros ") + (micros()));
//  x = float(analogRead(A14))* 400.0/1023.0-200.0; //map((analogRead(A14)),0,1023,-200,200);
//  y = float(analogRead(A15))* 400.0/1023.0-200.0; //map((analogRead(A15)),0,1023,-200,200);
//  z = float(analogRead(A16))* 400.0/1023.0-200.0; //map((analogRead(A16)),0,1023,-200,200);
//  Serial.println("x-axis,y-axis,z-axis" );
//  Serial.print( String(x)+ String(","));
//  Serial.print(String(y)+ String(","));
//  Serial.println(String(z));
//  }

//// plotting the values
//if(((micros()%100000)<5)){
//  
//  x = float(analogRead(A14))*400.0/1023.0-200.0; //map((analogRead(A14)),0,1023,-200,200);
//  y = float(analogRead(A14))*400.0/1023.0-200.0; //map((analogRead(A15)),0,1023,-200,200);
//  z = float(analogRead(A14))*400.0/1023.0-200.0; //map((analogRead(A16)),0,1023,-200,200);
//  Serial.println("x-axis,y-axis,z-axis" );
//  Serial.print( String(x)+ String(","));
//  Serial.print(String(y)+ String(","));
//  Serial.println(String(z));
//  
//  if (millis()>60000){return;}
//}

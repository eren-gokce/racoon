// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

#define LED1 33
#define LED2 25
#define LED3 26
#define LED4 27

bool apogee(float altitude, float accel);
bool parachute(float altitude); // if altitude under 600m return true



Adafruit_MPU6050 mpu;
Adafruit_BMP280 bmp; // I2C

  float p0 = 1034.7; // havadurumundan deniz seviyesi basınç değeri hPa

  float axOffset = 0, ayOffset = 0, azOffset = 0;
  float gxOffset = 0, gyOffset = 0, gzOffset = 0;
  float altitudeoffset = 0;

  int x = 0; // used for switch cases

void setup(void) {

  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  //bmp280

  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_1); /* Standby time. */

  //bmp280end

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

    int up_to = 1000;

    for (int i = 0; i < up_to; i++) {

    axOffset += a.acceleration.x;
    ayOffset += a.acceleration.y;
    azOffset += a.acceleration.z;
    gxOffset += g.gyro.x;
    gyOffset += g.gyro.y;
    gzOffset += g.gyro.z;
    altitudeoffset += bmp.readAltitude(p0);

    delay(5);
  }

  axOffset /= up_to;
  ayOffset /= up_to;
  azOffset /= up_to;
  gxOffset /= up_to;
  gyOffset /= up_to;
  gzOffset /= up_to;
  altitudeoffset /= up_to;


  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);


  Serial.println("");
  delay(100);

}

void loop() {

    sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float altitude;
  float altitude0;
  altitude0 = bmp.readAltitude(p0);
  altitude = (altitude0 - altitudeoffset); // altitude

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x - axOffset);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y - ayOffset);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z - azOffset +10.1); // gravity reg
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x - gxOffset);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y - gyOffset);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z - gzOffset);
  Serial.println(" rad/s");

  //Serial.print("Temperature: ");
  //Serial.print(temp.temperature);
  //Serial.println(" degC");

  //Serial.println("");

  //bmp280start

  //Serial.print(F("Temperature = "));
  //Serial.print(bmp.readTemperature());
  //Serial.println(" *C");

  //Serial.print(F("Pressure = "));
  //Serial.print(bmp.readPressure());
  //Serial.println(" Pa");

  Serial.print(F("Approx altitude = "));
  Serial.print(altitude); /* Adjusted to local forecast! */
  Serial.println(" m");

  Serial.println();

  switch (x)
  {
  case 0:
    if(a.acceleration.z < 8)  // launch
    {
        digitalWrite(LED1, HIGH);
        x++;
        break;
    }
    break;
  case 1:
    if(a.acceleration.z > 8) // burnout
    {
        digitalWrite(LED2, HIGH);
        x++;
        break;
    }
    break;
  case 2:
    //if(apogee(altitude, a.acceleration.z)) // apogee
    if(a.acceleration.z < -8)
    {
        digitalWrite(LED3, HIGH);
        x++;
        break;
    }
    break;
  case 3:
    if(parachute(altitude)) // parachute
    {
        digitalWrite(LED4, HIGH);
        x++;
        break;
    }
    break;
  }

  //bmp280end

  delay(500);

}

bool apogee(float altitude, float accel)
'{
  int y = 0;
  float altitude0;
  for(int i = 0; i < 3; i++)
  {
      altitude0 = altitude;
      delay(5);
      if(altitude0 < altitude)
      {
          return false;
      }
  }
  y++;

  if(accel > 0)
  {
    y++;
  }
  if(y = 2)
  {
    return true;
  }
}'

bool parachute(float altitude) // if altitude under 600m return true
{
    for(int i = 0; i < 3; i++)
    {
        if(altitude > 600)
        {
            return false;
        }
    }
    return true;
}
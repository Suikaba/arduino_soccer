#include <Wire.h>
#include <HMC5883L.h>

HMC5883L compass;
int error = 0;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  Serial.println("Constructing new HMC5883L");
  compass = HMC5883L();
    
  Serial.println("Setting scale to +/- 1.3 Ga");
  error = compass.set_scale(1.3); // Set the scale of the compass.
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.get_error_text(error));
  
  Serial.println(compass.calibrate(32));
  
  Serial.println("Setting measurement mode to continous.");
  error = compass.set_measurement_mode(MEASUREMENT_CONTINUOUS); // Set the measurement mode to Continuous
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.get_error_text(error));
}

void loop()
{
  // Retrive the raw values from the compass (not scaled).
  magnetometer_raw raw = compass.read_raw_axis();
  // Retrived the scaled values from the compass (scaled to the configured scale).
  magnetometer_scaled scaled = compass.read_scaled_axis();
  
  float heading_degrees = compass.read_heading();
  float heading = heading_degrees * PI/180;

  // Output the data via the serial port.
  output(raw, scaled, heading, heading_degrees);

  // Normally we would delay the application by 66ms to allow the loop
  // to run at 15Hz (default bandwidth for the HMC5883L).
  // However since we have a long serial out (104ms at 9600) we will let
  // it run at its natural speed.
  // delay(66);
}

// Output the data down the serial port.
void output(magnetometer_raw raw, magnetometer_scaled scaled, float heading, float heading_degrees)
{
   Serial.print("Raw: ");
   Serial.print(raw.x_axis);
   Serial.print("  ");   
   Serial.print(raw.y_axis);
   Serial.print("  ");   
   Serial.print(raw.z_axis);
   Serial.print("  \tScaled: ");
   
   Serial.print(scaled.x_axis);
   Serial.print("  ");   
   Serial.print(scaled.y_axis);
   Serial.print("  ");   
   Serial.print(scaled.z_axis);

   Serial.print("    Heading: ");
   Serial.print(heading);
   Serial.print(" Radians  ");
   Serial.print(heading_degrees);
   Serial.println(" Degrees  ");
}

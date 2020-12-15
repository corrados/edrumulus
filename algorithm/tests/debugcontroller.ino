
int analogPin = A3;

void setup()
{
  Serial.begin ( 9600 );
}

int values[700];

void loop()
{
  for ( int i = 0; i < 700; i++ )
  {
    values[i] = analogRead ( analogPin );  // read the input pin
  }

  for ( int i = 0; i < 700; i++ )
  {
    byte val2 = static_cast<byte> ( values[i] / 4 ); // remove two bits to fit into a byte
    Serial.write(val2);
  }
}

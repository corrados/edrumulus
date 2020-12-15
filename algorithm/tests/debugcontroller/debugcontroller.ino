


int analogPin = 34;
volatile int value;
volatile int iCnt = 0;
const int iNumSamples = 20000;
volatile int values[iNumSamples];
volatile bool sending = false;
int outCnt = 0;
 
hw_timer_t*  timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void onTimer()
{
  if ( !sending )
  {
    portENTER_CRITICAL_ISR(&timerMux);
    if ( iCnt < iNumSamples )
    {
      values[iCnt] = analogRead ( analogPin );
      //Serial.println ( iCnt );
      iCnt++;
    }
    portEXIT_CRITICAL_ISR(&timerMux);
  }
}
 
void setup()
{
  Serial.begin ( 115200 );
 
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 125, true);
  timerAlarmEnable(timer);
}
 
void loop()
{
  if ( iCnt >= iNumSamples )
  {
    sending = true;
    
    portENTER_CRITICAL(&timerMux);
    
    
/*
    for ( int i = 0; i < iNumSamples; i++ )
    {
      Serial.println ( values[i] );
    }
*/

    Serial.println ( values[outCnt] );
    outCnt++;

    if ( outCnt >= iNumSamples )
    {
      outCnt  = 0;
      iCnt    = 0;
      sending = false;
    }

/*    
    byte val1 = static_cast<byte> ( ( value >> 8 ) & 0xFF );
    byte val2 = static_cast<byte> ( value & 0xFF );

    Serial.write ( val1 );
    Serial.write ( val2 );

    if ( iCnt < 10 )
    {
      iCnt++;
    }
    else
    {
      // marker
      Serial.write ( 0xFF );  
      Serial.write ( 0xFF );        
      iCnt = 0;
    }
*/    
    
    portEXIT_CRITICAL(&timerMux);
    
  }
}


/*
int analogPin = 34;

hw_timer_t* timer = NULL;


void setup()
{
  Serial.begin ( 115200 );
  delay ( 1000 );
  timer = timerBegin ( 0, 100, true );// 10000, true );
  timerAttachInterrupt ( timer, &onTime, true );
  timerAlarmEnable ( timer );
}

const int iBlockSize = 1; // 700
int values[iBlockSize];

void onTime()
{
  const int value = analogRead ( analogPin );
  byte val2 = static_cast<byte> ( value / 4 ); // remove two bits to fit into a byte
  Serial.write ( val2 );  
}

void loop()
{
  for ( int i = 0; i < iBlockSize; i++ )
  {
    values[i] = analogRead ( analogPin ); // read the input pin
  }

  for ( int i = 0; i < iBlockSize; i++ )
  {
    byte val2 = static_cast<byte> ( values[i] / 4 ); // remove two bits to fit into a byte
    Serial.write(val2);
  }
}
*/

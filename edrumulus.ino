

/*
int analogPin = 34;
volatile int value;
volatile int iCnt = 0;
const int iNumSamples = 20000;
volatile int values[iNumSamples];
volatile bool sending = false;
int outCnt = 0;
hw_timer_t*  timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
*/

int in;
 

/*
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
*/
 
void setup()
{
  Serial.begin ( 115200 );

/*
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 125, true);
  timerAlarmEnable(timer);
*/
}
 
void loop()
{


  if ( Serial.available() > 0 )
  {
    //in = Serial.read();
    float f = Serial.parseFloat();
    Serial.println ( f, 7 );
  }


/*  
  if ( iCnt >= iNumSamples )
  {
    sending = true;
    
    portENTER_CRITICAL(&timerMux);
    
    Serial.println ( values[outCnt] );
    outCnt++;

    if ( outCnt >= iNumSamples )
    {
      outCnt  = 0;
      iCnt    = 0;
      sending = false;
    }
    
    portEXIT_CRITICAL(&timerMux);
  }
*/
}

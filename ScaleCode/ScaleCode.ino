void setupPWM16() {
    DDRB |= _BV(PB1) | _BV(PB2);
    TCCR1A = _BV(COM1A1) | _BV(COM1B1)
        | _BV(WGM11);
    TCCR1B = _BV(WGM13) | _BV(WGM12)
        | _BV(CS10);
    ICR1 = 0xffff;
}
 
void analogWrite16(uint16_t val){
  OCR1B = val;
}
 
static int threshold = 325;
static int red = 5;
static int green = 6;
static int blue = 7;
 
void setup() {
  setupPWM16();
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  Serial.begin(9600);
}
 
bool check(uint16_t n){
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
 
  analogWrite16(0);
   while(analogRead(0)<threshold){
    delay(10);
  }


  analogWrite16(65535);
  delay(50);
 
  while(analogRead(0)>threshold){
    delay(10);
  }
 
  delay(50);
 
  analogWrite16(n);
  for(int i=0; i<75; i++){
    if(analogRead(0)>threshold){
      return false;
    }
    delay(5);
  }
 
  return true;
}



bool temp(unsigned long val){
  bool a = check(val);
  bool b = check(val);
  if(a==b){
    return a;
  }else{
    return check(val);
  }
}
unsigned long binary_search(){
  unsigned long bounds[2] = {0, 65535};
  unsigned long mid;
 
  while(bounds[1]-bounds[0]>1){
    mid=(bounds[0]+bounds[1]+1)/2;
    if(temp(mid)){
      bounds[1]=mid;
    } else {
      bounds[0]=mid;
    }
  }


  analogWrite16(0);
  return bounds[0];
}
 
void voltage_to_mass(unsigned long v){
  float m;
  if(v < 13600){
    float x = ((float)v)/1000.0;
    m = 0.15773622*x*x*x - 3.22783206*x*x + 24.44927985*x - 23.02787114;
    Serial.println(m);
  }else{
    float x = ((float)v)/10000.0;
    m = -2.557609228*x*x*x*x*x + 36.282856592*x*x*x*x - 204.718969989*x*x*x + 607.09632994*x*x - 637.038525095*x + 255.907582753;
  }

  
  digitalWrite(blue, LOW);
  digitalWrite(green, LOW);
  digitalWrite(red, LOW);
  if(m<105){
    digitalWrite(red, HIGH);
  }
  if(m>105 && m<445){
    digitalWrite(blue, HIGH);
  }
  if(m>445){
    digitalWrite(green, HIGH);
  }


  Serial.print("voltage: ");
  Serial.print(((float)v)/10000.0);
  Serial.println(" v");
  Serial.print("mass: ");
  Serial.print(m);
 
  Serial.println(" g");
}
 
 
void loop() {
  digitalWrite(12, LOW);
  digitalWrite(11, HIGH);
  analogWrite16(0);
 
  if(Serial.available()){
    while(Serial.available()){
      Serial.read();
    }
    // Serial.println(binary_search());
    voltage_to_mass(binary_search());
  }
}

#define INTERNAL_LED D0            // Led in NodeMCU at pin GPIO16 (D0).
#define LED_SWITCH D1

void setup() {
  // put your setup code here, to run once:
  pinMode(INTERNAL_LED, OUTPUT);
  pinMode(LED_SWITCH, OUTPUT);

  digitalWrite(INTERNAL_LED, LOW);
  digitalWrite(LED_SWITCH, LOW);

  Serial.begin(9600);
  while(!Serial){
    digitalWrite(INTERNAL_LED, LOW);
    delay(100);
    digitalWrite(INTERNAL_LED, HIGH);}

  delay(500);
  Serial.println("Starting...");
  digitalWrite(INTERNAL_LED, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  digitalWrite(LED_SWITCH, HIGH);
  //digitalWrite(INTERNAL_LED, HIGH);
  Serial.println(".");

  delay(5000);

  digitalWrite(LED_SWITCH, LOW);
  //digitalWrite(INTERNAL_LED, LOW);

  delay(5000);

}
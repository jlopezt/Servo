// Una vuela de loop son ANCHO_INTERVALO segundos 
#define MULTIPLICADOR_ANCHO_INTERVALO 5 //Multiplica el ancho del intervalo para mejorar el ahorro de energia
#define ANCHO_INTERVALO            1200 //Ancho en milisegundos de la rodaja de tiempo
#define FRECUENCIA_ORDENES            2 //cada cuantas vueltas de loop atiende las ordenes via serie 

#define MAX_VUELTAS  UINT16_MAX// 32767 

//Includes generales
#include <TimeLib.h>  // download from: http://www.arduino.cc/playground/Code/Time
#include <Servo.h>

/*-----------------Variables comunes---------------*/
time_t anchoLoop= ANCHO_INTERVALO;//inicialmente desactivado el ahorro de energia
int debugGlobal=0; //por defecto desabilitado
uint16_t vuelta = MAX_VUELTAS-100;//0; //vueltas de loop

Servo myservo;  // crea el objeto servo
 
void setup() 
  {
  Serial.begin(115200);  
  anchoLoop=ANCHO_INTERVALO;

  //Ordenes serie
  Serial.println("Init Ordenes ----------------------------------------------------------------------");  
  inicializaOrden();//Inicializa los buffers de recepcion de ordenes desde PC

  //Servo
  Serial.println("Init Servo ------------------------------------------------------------------------");  
  myservo.attach(9);  // vincula el servo al pin digital 9
  
  for(int i=0;i<180;i+=2)
    {
    Serial.println(i);
    myservo.write(i);
    delay(10);                       
    }

  for(int i=180;i>0;i-=2)
    {
    myservo.write(i);
    Serial.println(i);
    delay(10);                       
    }

  Serial.println("--------------------------¡¡Listo!!----------------------------");
/*    
  myservo.write(0);
  delay(200);                       
  myservo.write(180);
  delay(200);                       
  myservo.write(90);                  
  */
}


void  loop()
  {  
  while(HayOrdenes(debugGlobal)) EjecutaOrdenes(debugGlobal); //Lee ordenes via serie
  }
  
void  loop_old()
  {  
  //referencia horaria de entrada en el bucle
  time_t EntradaBucle=millis();//Hora de entrada en la rodaja de tiempo

  //------------- EJECUCION DE TAREAS --------------------------------------
  //Acciones a realizar en el bucle   
  if ((vuelta % FRECUENCIA_ORDENES)==0) while(HayOrdenes(debugGlobal)) EjecutaOrdenes(debugGlobal); //Lee ordenes via serie
  //------------- FIN EJECUCION DE TAREAS ---------------------------------  

  //sumo una vuelta de loop, si desborda inicializo vueltas a cero
  vuelta++;//sumo una vuelta de loop  
      
  //Espero hasta el final de la rodaja de tiempo
  while(millis()<EntradaBucle+anchoLoop)//ANCHO_INTERVALO)
    {
    if(millis()<EntradaBucle) break; //cada 49 dias el contador de millis desborda
    //delayMicroseconds(1000);
    delay(1);
    }
  }
  

/*
 * ordenes.ino
 *
 * Comandos para la actualizacion de la hora del reloj
 *
 * Permite la puesta en hora del reloj a traves de comandos enviados por el puesto
 * serie desde el PC.
 *
 * Para actualizar la hora <comado> <valor>; Ejemplo: "hora 3;"
 * Se pueden anidar: "hora 2;minuto 33;"
 *
 */

#define LONG_COMANDO 40
#define LONG_PARAMETRO 30
#define LONG_ORDEN 22 //Comando (espacio) Parametros (fin de cadena)
#define MAX_COMANDOS   35

#include <Time.h>

char ordenRecibida[LONG_ORDEN]="";
int lonOrden=0;

typedef struct 
  {
  String comando;
  String descripcion;
  void (*p_func_comando) (int, char*, float)=NULL;
  }tipo_comando;
tipo_comando comandos[MAX_COMANDOS];

int HayOrdenes(int debug)
  {
  char inChar=0;
  
  while (Serial.available())
    {
    inChar=(char)Serial.read(); 
    switch (inChar)
      {
      case ';':
        //Recibido final de orden
        //if (debug) Serial.printf("Orden recibida: %s\n",ordenRecibida);
        return(1);
        break;
      default:
        //Nuevo caracter recibido. Añado y sigo esperando
        ordenRecibida[lonOrden++]=inChar;
        ordenRecibida[lonOrden]=0;
        break;
      }
    }  
  return(0); //No ha llegado el final de orden
  }

int EjecutaOrdenes(int debug){
  String comando="";
  String parametros="";
  int iParametro=0;
  char sParametro[LONG_PARAMETRO]="";//LONG_PARAMETRO longitud maxmima del parametro
  float fParametro;
  int inicioParametro=0;

  if (debug) 
    {
    Serial.print("Orden recibida: #");
    Serial.print(ordenRecibida);
    Serial.println("#");
    }
  
  for(int i=0;i<LONG_COMANDO;i++)
    {
    switch (ordenRecibida[i])
      {
      case ' ':
        //fin del comando, hay parametro
        inicioParametro=i+1;
        
        //Leo el parametro
        for (int j=0;j<LONG_ORDEN;j++)
          {  //Parsea la orden      
          if(ordenRecibida[j+inicioParametro]==0) 
            {
            strncpy(sParametro,ordenRecibida+inicioParametro,j+1);//copio el parametro como texto
            break;//j=LONG_ORDEN;//Cuando encuentro el final de la cadena
            }
          else iParametro=(iParametro*10)+(int)ordenRecibida[j+inicioParametro]-48; //hay que convertir de ASCII a decimal
          }
        fParametro=String(sParametro).toFloat();
        
        i=LONG_COMANDO;
        break;
      case 0:
        //fin de la orden. No hay parametro
        i=LONG_COMANDO;
        break;
      default:
        comando+=ordenRecibida[i];
        break;
      }
    }

  //Limpia las variables que3 recogen la orden
  lonOrden=0;
  ordenRecibida[0]=0;

  if (debug) 
    {
    Serial.println("comando: ");
    Serial.print("#"); Serial.println(comando.c_str()); Serial.println("#");
    Serial.print("#"); Serial.print(comando);          Serial.println("#");
    Serial.println(iParametro);
    Serial.println(sParametro);
    Serial.println(fParametro);
    }
    
/**************Nueva funcion ***************************/
  int8_t indice=0;
  for(indice=0;indice<MAX_COMANDOS;indice++)
    {
    if (comandos[indice].comando==comando) 
      {
      //Ejecuta la funcion asociada
      comandos[indice].p_func_comando(iParametro, sParametro, fParametro);
      return(0);
      }    
    }

  //Si llega aqui es que no ha encontrado el comando
  Serial.println("Comando no encontrado");
  return(-1);//Comando no encontrado  
/*******************************************************/
}

void limpiaOrden(void)
  {
  lonOrden=0;
  ordenRecibida[0]=0;
  }
  
void inicializaOrden(void)
  { 
  int i =0;  

  limpiaOrden();
  
  comandos[i].comando="help";
  comandos[i].descripcion="Listado de comandos";
  comandos[i++].p_func_comando=func_comando_help;
   
  comandos[i].comando="debug";
  comandos[i].descripcion="Activa/desactiva el modo debug";
  comandos[i++].p_func_comando=func_comando_debug;
    
  comandos[i].comando="go";
  comandos[i].descripcion="Ir a la posicion indicada go <posicion> 0<=posicion<=180";
  comandos[i++].p_func_comando=func_comando_go;
    
  //resto
  for(;i<MAX_COMANDOS;)
    {
    comandos[i].comando="vacio";
    comandos[i].descripcion="Comando vacio";
    comandos[i++].p_func_comando=func_comando_vacio;  
    }

  func_comando_help(0,"",0.0);
  }


/*********************************************************************/
/*  Funciones para los comandos                                      */
/*  void (*p_func_comando) (int, char*, float)                       */
/*********************************************************************/  
void func_comando_vacio(int iParametro, char* sParametro, float fParametro) //"vacio"
{}
 
void func_comando_help(int iParametro, char* sParametro, float fParametro) //"help"
  {
  Serial.println("\nComandos:");  
  for(int8_t i=0;i<MAX_COMANDOS;i++) 
  {
  if (comandos[i].comando!="vacio") 
    {
    Serial.print("Comando ");
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(comandos[i].comando.c_str());
    }
  }

  Serial.print("\n------------------------------------------------------------------------------\n");
  }

void func_comando_debug(int iParametro, char* sParametro, float fParametro)//"debug")
  {
  ++debugGlobal=debugGlobal % 2;
  if (debugGlobal) Serial.println("debugGlobal esta on");
  else Serial.println("debugGlobal esta off");
  }
  
void func_comando_go(int iParametro, char* sParametro, float fParametro) //"IP"
  {
  if(iParametro<0 || iParametro>180)   Serial.println("Valor de posicion no valido");  
  else 
    {
    myservo.write(iParametro);
    Serial.print("Go ");  
    Serial.print(iParametro);  
    Serial.println(" OK");  
    }
  }  
  

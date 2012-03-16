/** 
* @file main.c
*
* @brief Addressing multiple BeagleBone GPIO pins in C
* @details Self-contined program that will turn ON/OFF a specified group of
* of GPIO pins in the beaglebone board. This code was possible due to prior 
* efforts by Christian Mitchell, Markus Klink and Matt Richardson.
*
* @author Nuno Alves
* @date 16/March/2012 
**/

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <assert.h>

//If DEBUG is defined to TRUE (1), intermediate debug messages will be displayed 
//when code is executing. You may set it to FALSE (0) for no debug information.
#define DEBUG 1 

#define UNEXPORT                fopen( "/sys/class/gpio/unexport", "w" ) 
#define EXPORT                  fopen( "/sys/class/gpio/export", "w" ) 
#define GSTRING( s, i, g )      sprintf( s, "/sys/class/gpio/gpio%d %s", i, g ); 
#define GDIRECTION              "/direction" 
#define GVALUE                  "/value" 
#define GOPEN( s )              fopen( (s), "w" ) 

//The following #defines make it easier for the user to add differnt pins
#define P8_3  0
#define P8_4  1
#define P8_5  2
#define P8_11 3
#define P8_12 4

/** @brief Contains all elements which we need to pass into the beagleboard 
* to specify which GPIO pin we want to turn ON/OFF **/ 
struct gpioID
{
	char *PINNAME;  //eg. P8_3
	char * GPIOID; 	//e.g: gpio1[6] 
	int GPIONUMBER; //e.g: 38
	char *GPIOMUX;  //e.g: gpmc_ad6; 
};


int gpio_init ( int pin ) 
{ 
        FILE *p = NULL; 
        char gpio[50]; 

        if( ( p = EXPORT ) == NULL ) 
        { 
                printf( "Problem opening gpio%d\n", pin ); 
                return -1; 
        } 
        fprintf( p, "%d", pin ); 
        fclose( p ); 

        GSTRING( gpio, pin, GDIRECTION ); 
        if( ( p = GOPEN( gpio ) ) == NULL ) 
        { 
                printf( "Problem opening gpio%d\n %s\n", pin, gpio ); 
                return -1; 
        } 
        fprintf( p, "out"); 
        fclose( p ); 

        return 0; 
} 
int gpio_clean ( int pin ) 
{ 
        FILE *p = NULL; 
        if( ( p = UNEXPORT) == NULL ) 
        { 
                printf( "Problem closing gpio%d\n", pin ); 
                return -1; 
        } 
        fprintf( p, "%d", pin ); 
        fclose( p ); 
        return 0; 
} 

int gpio_control ( int pin, int value ) 
{ 
        FILE *p = NULL; 
        char gpio[50]; 

        GSTRING( gpio, pin, GVALUE ); 
        if( ( p = GOPEN( gpio ) ) == NULL ) 
        { 
                printf( "Problem controlling gpio%d\n", pin ); 
                return -1; 
        } 
        fprintf( p , "%d", value ); 
        pclose( p ); 
}


/** 
* @brief Initializes all the user specified pins  
* @param selected_GPIOs[] Un-initialized array of gpioID. At the end of this 
* function, the selected_GPIOs[] array will be initialized.
* @param selectedPins[] Array with the user specified pins
* @param sizeof_selectedPins Number of pins that were specified by the user
**/ 
void initialize_each_enabled_gpio(struct gpioID selected_GPIOs[],int selectedPins[], int nbr_selectedPins)
{
	int i;

	//You are not allowed to use more than 32 active pins. This is not a 
	//beagleboard limitation, but a limitation of this code.
	assert (nbr_selectedPins<=32);
	
	//cycles every userdefined pin, and inializes its contents in the 
	//selected_GPIOs[] array.
	for (i=0; i<nbr_selectedPins;i++)
	{
		switch (selectedPins[i])
		{
			case P8_3:
				selected_GPIOs[i].PINNAME="P8_3";
				selected_GPIOs[i].GPIOID="gpio1[6]";
				selected_GPIOs[i].GPIONUMBER=38;
				selected_GPIOs[i].GPIOMUX="gpmc_ad6";
				break;

			case P8_4:
				selected_GPIOs[i].PINNAME="P8_4";
				selected_GPIOs[i].GPIOID="gpio1[7]";
				selected_GPIOs[i].GPIONUMBER=39;
				selected_GPIOs[i].GPIOMUX="gpmc_ad7";
				break;

			case P8_5:
				selected_GPIOs[i].PINNAME="P8_5";
				selected_GPIOs[i].GPIOID="gpio1[2]";
				selected_GPIOs[i].GPIONUMBER=34;
				selected_GPIOs[i].GPIOMUX="gpmc_ad2";
				break;

			case P8_11:
				selected_GPIOs[i].PINNAME="P8_11";
				selected_GPIOs[i].GPIOID="gpio1[13]";
				selected_GPIOs[i].GPIONUMBER=45;
				selected_GPIOs[i].GPIOMUX="gpmc_ad13";
				break;

			case P8_12:
				selected_GPIOs[i].PINNAME="P8_12";
				selected_GPIOs[i].GPIOID="gpio1[12]";
				selected_GPIOs[i].GPIONUMBER=44;
				selected_GPIOs[i].GPIOMUX="gpmc_ad12";
				break;

			default:
				//This statement should never be reached... since
				//the program won't even compile if the user 
				//specified a bad pin specification.
				break;
		}
	}
    
    printf("TODO in initialize_each_enabled_gpio() : initilize pins to OFF\n"); 
}

/** 
* @brief Displays all information for each of the user specified pins. 
* This function has no purpose besides live debugging.
* @param selected_GPIOs[] An valid (initialized) array of gpioID
* @param nbr_selectedPins Number of pins that were specified by the user
**/ 
void display_each_enabled_gpio(struct gpioID selected_GPIOs[],int nbr_selectedPins)
{
	int i;
	printf("\n");
	printf("PINNAME\tGPIOID\t\tGPIOMUX\t\tGPIONBR\n");
	printf("++++++++++++++++++++++++++++++++++++++++++++++++\n");

	for (i=0; i< nbr_selectedPins; i++)
	{
		printf("%s\t%s\t%s\t%d\n",	
								selected_GPIOs[i].PINNAME,
								selected_GPIOs[i].GPIOID,
								selected_GPIOs[i].GPIOMUX,
								selected_GPIOs[i].GPIONUMBER
								);
	}
	printf("\n");
}

/** 
* @brief Cycles through the entire array of selected_GPIOs[] and turns ON/OFF
* each associated GPIO.
* @param selected_GPIOs[] An valid (initialized) array of gpioID
* @param data_to_write An unsigned integer (32 bits), where each bit contains the 
* information regarding the status of each pin: 1=turn pin ON, 0=turn pin OFF
* @param nbr_selectedPins Number of pins that were specified by the user
**/ 
void turn_ON_OFF_pins(struct gpioID selected_GPIOs[],unsigned int data_to_write,int nbr_selectedPins)
{
	int i;
	
	for (i=0;i<nbr_selectedPins;i++)
	{
		//code that turns ON/OFF a pin
		//printf("TODO in turn_ON_OFF_pins() : actually  turn ON/OFF pins\n"); 
		//(some function call goes here)

		//this is just for debugging purposes
		if (DEBUG)
		{ 
			if (bitRead(data_to_write,i) == 1) printf("turning ON");	
			else printf("turning OFF");	
			printf(	"\t%s (%s %d %s)\n",
						selected_GPIOs[i].PINNAME,
						selected_GPIOs[i].GPIOID,
						selected_GPIOs[i].GPIONUMBER,
						selected_GPIOs[i].GPIOMUX);
				
		}
	}
	if (DEBUG) printf("\n");
}

/** 
* @brief Writes a bit into a particular location in an unsigned int variable (32 bits)
* @param groupOfBits Some data resides in these 32 bits 
* @param bitVal The bitVal is wither a 1 or a 0
* @param bitLoc Where in the groupOfBits, the value of bitVal will be written. 
* Keep in mind that the bitLoc values ranges 0 to 31
* @return Returns the updated groupOfBits
**/ 
unsigned int bitWrite(unsigned int groupOfBits, int bitVal, int bitLoc)
{
    if (bitVal==1) groupOfBits |= 1 << bitLoc;
    else groupOfBits &= ~(1 << bitLoc);     
    return(groupOfBits);
}

/** 
* @brief Returns the bit value of a particular location in an unsigned int variable (32 bits)
* @param groupOfBits Some data resides in these 32 bits 
* @param bitLoc Where in the groupOfBits, the value of bitVal will be written 
* @return Retuns an integer with the bit value (1 or 0) that resides in bitLoc
**/ 
int bitRead(unsigned int groupOfBits, int bitLoc)
{
    unsigned int bit = groupOfBits & (1 << bitLoc);
    if (bit!=0) bit=1;

    return(bit);
}

/** 
* @brief This is the main function with a sample test program.
* @return Returns a 1 upon succesful program termination
**/ 
int main()
{
	
	int selectedPins[]={P8_3,P8_4,P8_5,P8_11,P8_12};
	int nbr_selectedPins=sizeof(selectedPins)/sizeof(*selectedPins);

	//This variable data_to_write will contain the information which specified
	//the pins that will be ON or OFF
	unsigned int data_to_write=0;
	
	//Define an array of gpios which will contain all the user selected pins and
	//its associated information
	struct gpioID enabled_gpio[nbr_selectedPins];

	//Initializes all user defined pins
	initialize_each_enabled_gpio(enabled_gpio,selectedPins,nbr_selectedPins);
	if (DEBUG) display_each_enabled_gpio(enabled_gpio,nbr_selectedPins);
	
	
	//At this point in the code there are 5 selected pins:
	//P8_3, P8_4, P8_5, P8_11, P8_12
	//All these pins are intially OFF.
	
	//If I just want to turn ON pins P8_4 and P8_11, I need to look at my 
	//selectedPins[] array: Since positions start at 0, pin P8_4 is on position 1
	//and P8_11 is on position 3. Therefore I need to set the bits 1 and 3 on 
	//the variable data_to_write. In base 2, setting bit1 and 3 looks like: 
	//00000000 00000000 00000000 00001010 (base 2)
	//Which is equivalent to the value 10, in base 10.	
    data_to_write=10; 
	turn_ON_OFF_pins(enabled_gpio,data_to_write,nbr_selectedPins);

	//printf("TODO in main() : put a small delay here\n"); 

	//In addition to the pins that are already ON (P8_4, P8_11), I now
	//also want to turn ON pin P8_12. Since P8_12 is on position 4, I 
	//need to write a 1 to position 4.
    data_to_write=bitWrite(data_to_write,1,4);
	turn_ON_OFF_pins(enabled_gpio,data_to_write,nbr_selectedPins);
 
    //Of course, I could have done something like
    /*
      data_to_write=26; 
      turn_ON_OFF_pins(enabled_gpio,data_to_write,nbr_selectedPins);
	*/
	//Which would have yielded the exact same outcome.
 
    //printf("TODO in main() : put a small delay here\n"); 

    //Now I would like to turn OFF pin P8_11, while leaving pins P8_4 & P8_12 ON
	//Since pin P8_11 is on position 3
  	 data_to_write=bitWrite(data_to_write,0,3);
	turn_ON_OFF_pins(enabled_gpio,data_to_write,nbr_selectedPins);
}
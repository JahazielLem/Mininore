

#define SCREEN_ADDRESS 0x3c
#define LOGO_WIDTH    84
#define LOGO_HEIGHT   52
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)





uint8_t advertisement[31] = { //Data to be advertise
  0x1E, 0xFF, 0x4C, 0x00, 0x12, 0x19, 0x10, 0x12, 0x12, 0x34, 0x56, 
  0x78, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78, 0x90, 
  0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x90
  };  

  
int selected = 0;
int Layer = -1;


uint8_t limitDevices = 10;//How many devices able to detect (if this variable change, must change the next variables section)
unsigned long delayStart[10];//Activity per AirTag
bool actives[10];         //To check if the Airtag is around more than 10 minutes
String mairtags[10];      //Save Mac AirTags for reference
//uint8_t advertisement[31];//Save actual AirTag Advertising
uint8_t advertags[10][31];//Save active AirTags Advertinsings
uint8_t positiona = 0;    //The last Active AirTag
bool playSoundm = false;  //Write chrac to play a sound without waiting 10 minutes
float MINUTES = 10.2;     //Minimum 10 minutes to make it play a sound
unsigned long ONEMINUTE = 60000;//1 minute
unsigned long DELAY_TIME = ONEMINUTE * MINUTES;
uint8_t delayRunning = 1; 

const unsigned char PROGMEM logo[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x7f, 0xe3, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf0, 0xf8, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x1f, 0xff, 0xf8, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xf1, 0xff, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xe3, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xff, 0xff, 0xc3, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xc3, 0x00, 0x7f, 
0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0x82, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x01, 0xfe, 0x00, 0x00, 0x0f, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xfc, 0x00, 0x00, 
0x07, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x38, 0x03, 0xfc, 0x03, 0x32, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x09, 0x90, 0x0f, 0xff, 0x00, 0x4e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x60, 0x3f, 
0xff, 0x80, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x80, 0x7f, 0xff, 0xc0, 0x67, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x1c, 0x00, 0x7f, 0xff, 0xe0, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 
0x7f, 0xff, 0xe0, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x7f, 0xff, 0xf0, 0x3f, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x3f, 0xff, 0xf0, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x3f, 
0x81, 0x3f, 0xff, 0xf0, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x81, 0x1f, 0xff, 0xf0, 0x1f, 
0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x01, 0x3f, 0xff, 0xf0, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 
0x7f, 0x01, 0x33, 0xf9, 0xf0, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x20, 0x70, 
0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x80, 0x20, 0x20, 0x1f, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x7f, 0x00, 0x00, 0x60, 0x20, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x01, 0x10, 
0x60, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x33, 0x80, 0xcf, 0x1f, 0xe0, 0x3f, 0xc0, 0x00, 0x00, 
0x00, 0x00, 0x30, 0x00, 0xff, 0x0f, 0xe0, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x7f, 
0x1f, 0xc0, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x07, 0x1c, 0x00, 0xff, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x03, 0xc0, 0x03, 0xfc, 0x00, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf8, 
0x03, 0xf8, 0x00, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x03, 0xf8, 0x00, 0x7f, 0xc0, 
0x00, 0x00, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x60, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 
0xf8, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x00, 0x00, 0x06, 0x60, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x0f, 0xff, 0x80, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00, 0xff, 
0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x7f, 0xff, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x8f, 
0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0xd7, 0xfe, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x03, 0xfe, 0xd7, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 
0xd7, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0xd7, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00
};

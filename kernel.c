#define VGA_BUFFER ((volatile unsigned short *)0xB8000)
#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define COLOUR_WHITE_ON_BLACK 0x0F

static int cursor_row = 0;
static int cursor_col = 0;

void scroll(void){
    for(int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++){
        VGA_BUFFER[i] = VGA_BUFFER[i + VGA_WIDTH];
    }
    for(int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++){
        VGA_BUFFER[i] = (0x0F << 8) | ' ';
    }
}

void clear_screen(void){
    for(int i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++){
        VGA_BUFFER[i] = (0x0F << 8) | ' ';
    }
}

void put_char(const char c){
    if(c == '\n'){
        cursor_col = 0;
        cursor_row++;
    }
    else{
        VGA_BUFFER[cursor_row * VGA_WIDTH + cursor_col] = (COLOUR_WHITE_ON_BLACK << 8) | c;
        cursor_col++;
    }
    if(cursor_col >= VGA_WIDTH){
        cursor_col = 0;
        cursor_row++;
    }
    if(cursor_row >= VGA_HEIGHT){
        scroll();
        cursor_row = VGA_HEIGHT - 1;
    }
}

void put_string(const char *str){
    for(int i = 0; str[i] != '\0'; i++){
        put_char(str[i]);
    }
}

void kernel_main(void){
    clear_screen();
    for(int i = 0; i < 40; i++){
        put_string("Hello Kernel!\n");  
    }
}
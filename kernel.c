#define VGA_BUFFER ((volatile unsigned short *)0xB8000)
#define VGA_HEIGHT 25
#define VGA_WIDTH 80

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
    cursor_col = 0;
    cursor_row = 0;
}

void put_char(const char c){
    if(c == '\n'){
        cursor_col = 0;
        cursor_row++;
    }
    else{
        VGA_BUFFER[cursor_row * VGA_WIDTH + cursor_col] = (0x0F << 8) | c;
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

void put_int(int n){
    char buffer[12];
    int count = 0;
    unsigned int u;

    if(n == 0){
        put_char('0');
        return;
    }

    if(n < 0){
        put_char('-');
        u = 0u - (unsigned int)n;
    } else{
        u = (unsigned int)n;
    }

    while(u > 0){
        buffer[count++] = u % 10 + '0';
        u /= 10;
    }

    for(int i = count - 1; i >= 0; i--){
        put_char(buffer[i]);
    }
}

int strlen(const char *str){
    int count = 0;
    for(int i = 0; str[i] != '\0'; i++){
        count++;
    }
    return count;
}

void put_string(const char *str){
    for(int i = 0; str[i] != '\0'; i++){ //or for(int i = 0; strlen(str); i++){
        put_char(str[i]);
    }
}

void kernel_main(void){
    clear_screen();
    put_int(42);      
    put_char('\n');
    put_int(0);       
    put_char('\n');
    put_int(-1234);   
    put_char('\n');
    put_int(2147483647); 
    put_char('\n');
    put_int(-2147483647 - 1); 
    put_char('\n');
}


namespace led_matrix {
    [CCode (cname = "led_matrix_init", cheader_filename = "led_matrix.h")]
    public static int init(string matrix_ip);

    [CCode (cname = "led_matrix_finish", cheader_filename = "led_matrix.h")]
    public static void finish();

    [CCode (cname = "led_matrix_update", cheader_filename = "led_matrix.h")]
    public static void update(line *ledLine);

    [CCode (cname = "led_matrix_print", cheader_filename = "led_matrix.h")]
    public static int16 print(string msg,line *ledLine);

    [CCode (cname = "led_matrix_line", cheader_filename = "led_matrix.h")]
    public static struct line{
        uint16 *buffer_red; /**< the red part of the string will be put here */
        uint16 *buffer_green; /**< the green part of the string will be put here */
        public int16 x; /**< current x position */
        public int16 y; /**< current y position */
    }

    [CCode (cname = "led_matrix_allocate_line", cheader_filename = "led_matrix.h")]
    public static int allocate_line(line *ledLine);

    [CCode (cname = "led_matrix_clear_screen", cheader_filename = "led_matrix.h")]
    public static int clear_screen(line *ledLine);
    
    [CCode (cname = "led_matrix_reset", cheader_filename = "led_matrix.h")]
    public static void reset();
    
    [CCode (cname = "led_matrix_select_font", cheader_filename = "led_matrix.h")]
    public static void select_font(int font);
    
    [CCode (cname = "led_matrix_print_direct", cheader_filename = "led_matrix.h")]
    public static void print_direct(string msg);
}

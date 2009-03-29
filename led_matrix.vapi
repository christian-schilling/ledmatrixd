

namespace led_matrix {
    [CCode (cname = "led_matrix_init", cheader_filename = "led_matrix.h")]
    public static int init(string matrix_ip);

    [CCode (cname = "led_matrix_finish", cheader_filename = "led_matrix.h")]
    public static void finish();

    [CCode (cname = "led_matrix_update", cheader_filename = "led_matrix.h")]
    public static void update(line *ledLine);

    [CCode (cname = "led_matrix_print", cheader_filename = "led_matrix.h")]
    public static void print(string msg,line *ledLine);

    [CCode (cname = "led_matrix_line", cheader_filename = "led_matrix.h")]
    public static struct line{
        uint16 *column_red; /**< the red part of the string will be put here */
        uint16 *column_green; /**< the green part of the string will be put here */
        uint16 *column_red_output; /**< red part of the string, possibly shifted */
        uint16 *column_green_output; /**< green part of the string, possibly shifted */
        int x; /**< current x position */
        int y; /**< current y position */
        int shift_position; /**< position of the output arrays */
    }
    
    [CCode (cname = "led_matrix_allocate_line", cheader_filename = "led_matrix.h")]
    public static int allocate_line(line *ledLine, int line_length);

    [CCode (cname = "led_matrix_shift_left", cheader_filename = "led_matrix.h")]
    public static int shift_left(line *ledLine);
}

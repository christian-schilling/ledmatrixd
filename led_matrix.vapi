

namespace led_matrix {
    [CCode (cname = "led_matrix_init", cheader_filename = "led_matrix.h")]
    public static int init(string matrix_ip);

    [CCode (cname = "led_matrix_finish", cheader_filename = "led_matrix.h")]
    public static void finish();

    [CCode (cname = "led_matrix_update", cheader_filename = "led_matrix.h")]
    public static void update();

    [CCode (cname = "led_matrix_print", cheader_filename = "led_matrix.h")]
    public static void print(string msg);
}
